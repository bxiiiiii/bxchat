#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <mysql/mysql.h>
#include <unistd.h>

#include "s_d.h"
#include "list.h"

#define OPEN_MAX 1024

void my_error(char *str, int line);
void login(Pack *pack);
MYSQL mysql_con();
void per_setting(Pack *pack);
void Add_friend(Pack *pack);
void View_friendrq(Pack *pack);
void View_friendlist(Pack *pack);
void Process_friendrq(Pack *pack);

MYSQL mysql;

int main()
{
    int i;
    socklen_t clen;
    int ready;
    int cfd, sfd, lfd, efd;
    struct sockaddr_in caddr, saddr;
    struct epoll_event tem, ep[OPEN_MAX];
    Pack *pack = (Pack *)malloc(sizeof(Pack));
    memset(pack, 0, sizeof(pack));
    char str[16];
    char buf[200];

    mysql = mysql_con();

    bzero(&saddr, sizeof(saddr));
    saddr.sin_port = htons(PORT);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));


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

    while(1)
    {
        if((ready = epoll_wait(efd, ep, OPEN_MAX , -1)) < 0)
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
                    int ret = recv(ep[i].data.fd, pack, sizeof(Pack), 0);
                    pack->data.serfd = ep[i].data.fd;
                    if(ret == 0)
                    {
                        if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, NULL) < 0)
                            my_error("epoll_ctl failed", __LINE__);
                        printf("disconnection--cfd :%d \n", pack->data.serfd);
                        close(pack->data.serfd);
                    }
                    printf("--choice = %d, id = %d\n", pack->choice, pack->info.id);
                    //printf("id:%d  pass:%s  ans:%s\n", pack->info.id, pack->info.password, pack->info.answer);
                    if(pack->choice == EXIT)
                    {
                        if(epoll_ctl(efd, EPOLL_CTL_DEL, cfd, NULL) < 0)
                            my_error("epoll_ctl failed", __LINE__);
                        printf("disconnection--cfd :%d \n", pack->data.serfd);
                        close(pack->data.serfd);
                    }
                    if(pack->choice == RETURN_LOGIN)
                    {
                        printf("id : %d logout\n", pack->info.id);
                        memset(buf, 0, sizeof(buf));
                        sprintf(buf, "update login_info set status = 0 where id = %d", pack->info.id);
                        if(mysql_query(&mysql, buf) < 0)
                            my_error("mysql_query failed", __LINE__);
                    }
                    if(pack->choice <= GET_NEW_PW)
                        login(pack);
                    if((pack->choice >= VIEW_PER_INFO) && (pack->choice <= RETURN_OPT1))
                        per_setting(pack);
                    if(pack->choice == ADD_FRIEND)
                        Add_friend(pack);
                    if(pack->choice == VIEW_FRIENDS_RQ)
                        View_friendrq(pack);
                    if(pack->choice == VIEW_FRIENDS_LIST)
                        View_friendlist(pack);
                    if(pack->choice == PROCESS_FRQ)
                        Process_friendrq(pack);                

                }
            }
        }
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


void login(Pack *pack)
{
    char buf[200];
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;

    switch(pack->choice)
    {
        case LOGIN:
            printf("id:%d  pass:%s\n", pack->info.id, pack->info.password);
            memset(buf, sizeof(buf), 0);
            sprintf(buf, "select id from login_info where id = %d", pack->info.id);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);

            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);       
            if(!row)
	        {
                pack->status = ERROR_id;
                printf("id error\n");
                send(pack->data.serfd, pack, sizeof(Pack), 0);
                break;
            }

            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select password from login_info where password = %s AND id = %d", pack->info.password, pack->info.id);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(!row)
                pack->status = ERROR_password;
            else
            {
                printf("id : %d login\n", pack->info.id);
                memset(buf, sizeof(buf), 0);    
                sprintf(buf, "update login_info set status = 1 where id = %d", pack->info.id);
                if(mysql_query(&mysql,buf))
                    my_error("mysql_query", __LINE__);
                pack->status = SUCCESS_login;
            }

            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;

        case REGISTER:
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "insert into login_info values(NULL, \"%s\", \"%s\", \"%s\", \"%s\", 0)", pack->info.name, pack->info.password, pack->info.question, pack->info.answer);
            if(mysql_query(&mysql, buf))
                my_error("mysql_query failed", __LINE__);

            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select id from login_info where password = %s", pack->info.password);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            num_fields = mysql_num_fields(result);
            row = mysql_fetch_row(result);
            pack->info.id = atoi(row[num_fields-1]);

            pack->status = SUCCESS_register;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;

        case FIND_PASSWORD:
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select id from login_info where id = %d", pack->info.id);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(!row)
            {
                pack->status = ERROR_id;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
            }
            else
            {
                memset(buf, sizeof(buf), 0);    
                sprintf(buf, "select question from login_info where id = %d", pack->info.id);
                if(mysql_query(&mysql,buf))
                    my_error("mysql_query", __LINE__);
                result = mysql_store_result(&mysql);
                row = mysql_fetch_row(result);
                strcpy(pack->info.question, row[0]);
                pack->status = RIGHT_id;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
            }
            break;

        case FIND_ANSWER:
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select answer from login_info where answer = %s AND id = %d", pack->info.answer, pack->info.id);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(!row)
                pack->status = ERROR_answer;
            else
                pack->status = RIGHT_answer;
            send(pack->data.serfd, pack, sizeof(Pack), 0);

        case GET_NEW_PW:
            printf("%s\n", pack->info.password);
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "update login_info set password = \"%s\" where id = %d", pack->info.password, pack->info.id);                
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            pack->status = SUCCESS_find;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;
    }
}

void per_setting(Pack *pack)
{

    char buf[200];
    MYSQL_RES *result;
    MYSQL_ROW row;

    switch (pack->choice)
    {
        case VIEW_PER_INFO:
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select name from login_info where id = %d", pack->info.id);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            strcpy(pack->info.name, row[0]);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select question from login_info where id = %d", pack->info.id);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            strcpy(pack->info.question, row[0]);
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;
        case CHANGE_NAME:
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update login_info set name = \"%s\" where id = %d", pack->info.name, pack->info.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            pack->status = SUCCESS_CH_NA;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;

        case CHANGE_QUESTION:
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update login_info set question = \"%s\" where id = %d", pack->info.question, pack->info.id);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            pack->status = SUCCESS_CH_Q;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;

        case CHANGE_ANSWER:
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update login_info set answer = \"%s\" where id = %d", pack->info.answer, pack->info.id);
            if(mysql_query(&mysql,buf))
                my_error("mysql_query", __LINE__);
            pack->status = SUCCESS_CH_A;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;
    }
}

void Add_friend(Pack *pack)
{
    char buf[200];
    MYSQL_RES *result;
    MYSQL_ROW row;

   // printf("***\n");
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = ERROR_id;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id2 from friends where id2 = %d and id1 = %d", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(row)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "select status from friends where id1 = %d and id2 = %d", pack->info.id, pack->data.cid);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if(atoi(row[0]) == 1)
            pack->status = FRIEND;
        else
            pack->status = HAVE_SENT;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "insert into friends values(%d, %d, 0)", pack->info.id, pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);

    pack->status = SEND_SUCCESS;
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
   /* if(atoi(row[0]) == 1)
    {
        printf("111111111\n");
        pack->status = SEND_SUCCESS;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
     //   pack->choice = ADD_FRIEND;
     //   send(pack->data.serfd, pack, sizeof(Pack), 0);
    }*/
}

void View_friendrq(Pack *pack)
{
    char buf[200];
    int i, rq_num = 0;
    friend_list_t list, curPos, newNode;
    friend_t data;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, friend_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id1 from friends where id2 = %d and status = 0", pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);

    row = mysql_fetch_row(result);
    if(row == NULL)
    {
        pack->status = 0;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }
    else
    {
        pack->status = 1;
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        rq_num++;
    }
    

    while(row = mysql_fetch_row(result))
    {
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        rq_num++;
    }
    pack->rqf_num = rq_num;
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    List_ForEach(list, curPos)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "select name from login_info where id = %d", curPos->data.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        strcpy(curPos->data.name,row[0]);
        pack->fnode = curPos->data;
        int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
        //printf("%d %s %d\n", pack->fnode.id, pack->fnode.name, pack->rqf_num);
        //printf("%d\n", n);
    }
}

void View_friendlist(Pack *pack)
{
    char buf[200];
    int i, fri_num = 0;
    friend_list_t newNode;
    friend_list_t list, curPos;
    friend_t data;
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, friend_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id2 from friends where id1 = %d and status = 1", pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);

    row = mysql_fetch_row(result);
    if(row == NULL)
    {
        pack->status = 0;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }
    else
    {
        pack->status = 1;
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        fri_num++;
    }

    while(row = mysql_fetch_row(result))
    {
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        fri_num++;
    }
    pack->rqf_num = fri_num;

    List_ForEach(list, curPos)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "select name, status from login_info where id = %d", curPos->data.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        strcpy(curPos->data.name,row[0]);
        curPos->data.status = atoi(row[1]);
        pack->fnode = curPos->data;
        int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
    }
}

void Process_friendrq(Pack *pack)
{
    char buf[200];

    if(pack->status == 1)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update friends set status = 1 where id1 = %d and id2 = %d", pack->data.cid, pack->info.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "insert into friends values(%d, %d, 1)", pack->info.id, pack->data.cid);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "delete from friends where id1 = %d and id2 = %d;", pack->data.cid, pack->info.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
    }
}

void Chat_sb(Pack *pack)
{
    
}