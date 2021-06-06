#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <mysql/mysql.h>
#include <unistd.h>

#include "s_d.h"

#define OPEN_MAX 1024

void my_error(char *str, int line);
void login(MYSQL *mysql, per_info *info);
MYSQL mysql_con();
void mysql_op(MYSQL mysql);

int main()
{
    int i;
    socklen_t clen;
    int ready;
    MYSQL mysql;
    int cfd, sfd, lfd, efd;
    struct sockaddr_in caddr, saddr;
    struct epoll_event tem, ep[OPEN_MAX];
    per_info info;
    char str[16];

    mysql = mysql_con();

    bzero(&saddr, sizeof(saddr));
    saddr.sin_port = htons(PORT);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,&opt, sizeof(opt));

    if((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        my_error("socket failed", __LINE__);

    if(bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
        my_error("bind failed", __LINE__);

    if(listen(lfd, OPEN_MAX) < 0)
        my_error("listen failed", __LINE__);

    if((efd = epoll_create(OPEN_MAX)) < 0)
        my_error("epoll_create failed", __LINE__);
    
    tem.events = EPOLLIN;
    tem.data.fd = lfd;

    if(epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &tem) < 0)
        my_error("epoll_ctl failed", __LINE__);

    int x = 3; 
    while(x)
    {
        if((ready = epoll_wait(efd, ep, OPEN_MAX, -1)) < 0)
            my_error("epoll_wait failed", __LINE__);


 
        for(i = 0; i < ready; i++)
        {
            if(ep[i].events & EPOLLIN)
            {
                if(ep[i].data.fd == lfd)
                {
                    clen = sizeof(caddr);
                    if((cfd = accept(lfd, (struct sockaddr *)&caddr, &clen)) < 0)
                        my_error("accept failed", __LINE__);

                    printf("connection--port:%d\tip:%s\n", ntohs(caddr.sin_port), inet_ntop(AF_INET, &caddr.sin_addr.s_addr, str, sizeof(str)));

                    tem.data.fd = cfd;
                    tem.events = EPOLLIN;

                    if(epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &tem) < 0)
                        my_error("epoll_ctl failed", __LINE__);
                }
                else
                {
                    recv(ep[i].data.fd, &info, sizeof(info), 0);
                    info.sfd = ep[i].data.fd;
                    if(info.choice == 4)
                    {
                        if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, NULL) < 0)
                            my_error("epoll_ctl failed", __LINE__);
                        printf("cfd :%d exit\n", info.cfd);
                        close(cfd);
                    }
                    if(info.choice < 4)
                        login(&mysql, &info);
                }
            }

            if(ep[i].events & EPOLLOUT)
            {

            }
        }
        x--;
    }
    close(lfd);
    mysql_close(&mysql);
	mysql_library_end();
    return 0;
}

void my_error(char *str, int line)
{
    fprintf(stderr, "line : %d \n", line);
    perror(str);
    exit(1);
}

void login(MYSQL *mysql, per_info *Info)
{
    char buf[200];
    per_info info = *Info;
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;
    switch(info.choice)
    {
        case LOGIN:
            memset(buf, sizeof(buf), 0);
            sprintf(buf, "select id from login_info where id = %d", info.id);
            if(mysql_query(mysql,buf))
                my_error("mysql_query", __LINE__);

            result = mysql_store_result(mysql);
            row = mysql_fetch_row(result);
            
            if(!row)
	        {
                info.status = ERROR_ID;
                printf("id error\n");
                send(info.sfd, &info.status, sizeof(info.status), 0);
                break;
            }

            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select password from login_info where password = %s AND id = %d", info.password, info.id);
            if(mysql_query(mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(mysql);
            row = mysql_fetch_row(result);
            if(!row)
            {

                info.status = ERROR_PASSWORD;
                printf("password error\n");
            }
            else
            {
                printf("login success\n");
                memset(buf, sizeof(buf), 0);    
                sprintf(buf, "update login_info set status = 1 where id = %d", info.id);
                if(mysql_query(mysql,buf))
                    my_error("mysql_query", __LINE__);
                info.status = SUCCESS_LOGIN;
            }
            send(info.sfd, &info.status, sizeof(info.status), 0);
            break;

        case REGISTER:
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "insert into login_info values(NULL, \"%s\", \"%s\", \"%s\", \"%s\", 0)", info.name, info.password, info.question, info.answer);
            if(mysql_query(mysql, buf))
                my_error("mysql_query failed", __LINE__);

            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select id from login_info where password = %s", info.password);
            if(mysql_query(mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(mysql);
            num_fields = mysql_num_fields(result);
            row = mysql_fetch_row(result);
            info.id = atoi(row[num_fields-1]);

            info.status = SUCCESS_REGISTER;
            send(info.sfd, &info, sizeof(info), 0);
            break;

        case FIND_PASSWORD:
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select question from login_info where id = %d", info.id);
            if(mysql_query(mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(mysql);
            num_fields = mysql_num_fields(result);
            row = mysql_fetch_row(result);
            strcpy(info.question, row[0]);
            send(info.sfd, info.question, sizeof(info.question), 0);
            recv(info.sfd, info.answer, sizeof(info.answer), 0);

            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select answer from login_info where answer = %s AND id = %d", info.answer, info.id);
            if(mysql_query(mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(mysql);
            row = mysql_fetch_row(result);
            if(!row)
            {
                info.status = ERROR_ANSWER;
                printf("answer error\n");
            }
            else
            {
                printf("answer right\n");
                info.status = RIGHT_ANSWER;
            }
            send(info.sfd, &info.status, sizeof(info.status), 0);

            recv(info.sfd, &info, sizeof(info), 0);
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "update login_info set password = \"%s\" where id = %d", info.password, info.id);                if(mysql_query(mysql,buf))
                my_error("mysql_query", __LINE__);
            info.status = SUCCESS_FIND;
            send(info.sfd, &info.status, sizeof(info.status), 0);
            break;
    }
}

MYSQL mysql_con()
{
    MYSQL mysql;
    if(NULL == mysql_init(&mysql))
        my_error("mysql_init", __LINE__);

	if(mysql_library_init(0, NULL, NULL) != 0)
        my_error("mysql_library_init", __LINE__);

	if(NULL == mysql_real_connect(&mysql, "127.0.0.1", "root", "1551097460", "bxchat", 0, NULL, 0))
        my_error("mysql_real_connect", __LINE__);

	if(mysql_set_character_set(&mysql, "utf8") < 0)
        my_error("mysql_set_character_set failed", __LINE__);

    return mysql;
}
