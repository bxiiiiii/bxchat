#include "serv_task.c"
#define OPEN_MAX 1024

int main()
{
    int i;
    socklen_t clen;
    int ready;
    int cfd, sfd, lfd, efd, fd;
    per_list_t curPos;
    pthread_t tid;
    struct sockaddr_in caddr, saddr;
    struct epoll_event tem, ep[OPEN_MAX];
    Pack *pack = (Pack *)malloc(sizeof(Pack));
    memset(pack, 0, sizeof(pack));
    user_time_t time;
    user_date_t date;
    char str[16];
    char buf[200];
    char logbuf[300];

    mysql = Mysql_con();

    bzero(&saddr, sizeof(saddr));
    saddr.sin_port = htons(PORT);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);


    List_Init(plist, per_node_t);


    if((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        my_error("socket failed", __LINE__);


    int opt = 1;
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));


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

                    printf("connection--port:%d\tip:%s\tcfd:%d\n", ntohs(caddr.sin_port), inet_ntop(AF_INET, &caddr.sin_addr.s_addr, str, sizeof(str)), cfd);
                    
                    date = DateNow();
                    time = TimeNow();
                    memset(logbuf, sizeof(logbuf), 0);
                    sprintf(logbuf, "%d/%02d/%02d %02d:%02d:%02d  %3d   %3d   listen\n", date.year, date.month, date.day, time.hour, time.minute, time.second, pack->info.id, cfd);
                    fd = open("log.txt", O_RDWR|O_APPEND);
                    write(fd, logbuf,  strlen(logbuf));
                    close(fd);

                    tem.data.fd = cfd;
                    tem.events = EPOLLIN;

                    if(epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &tem) < 0)
                        my_error("epoll_ctl failed", __LINE__);
                }
                else
                {
                    int ret = recv(ep[i].data.fd, pack, sizeof(Pack), 0);
                    
                    date = DateNow();
                    time = TimeNow();
                    memset(logbuf, sizeof(logbuf), 0);
                    sprintf(logbuf, "%d/%02d/%02d %02d:%02d:%02d  %3d   %3d   %3d\n", date.year, date.month, date.day, time.hour, time.minute, time.second, pack->info.id, ep[i].data.fd, pack->choice);
                    fd = open("log.txt", O_RDWR|O_APPEND);
                    write(fd, logbuf, strlen(logbuf));
                    close(fd);

                    pack->data.serfd = ep[i].data.fd;
                    pack->data.efd = efd;
                    if(pack->info.id == 0)
                        continue;
                    if(ret == 0)
                    {
                        if(epoll_ctl(efd, EPOLL_CTL_DEL, pack->data.serfd, NULL) < 0)
                            my_error("epoll_ctl failed", __LINE__);
                        printf("disconnection--cfd :%d \n", pack->data.serfd);
                        close(pack->data.serfd);
                        continue;
                    }
                    printf("--choice = %d, id = %d\n", pack->choice, pack->info.id);
                    //printf("id:%d  pass:%s  ans:%s\n", pack->info.id, pack->info.password, pack->info.answer);
                    if(pack->choice == EXIT1)
                    {
                        if(epoll_ctl(efd, EPOLL_CTL_DEL, pack->data.serfd, NULL) < 0)
                            my_error("epoll_ctl failed", __LINE__);
                        printf("disconnection--cfd :%d \n", pack->data.serfd);
                        close(pack->data.serfd);
                        continue;
                    }
                    if(pack->choice == EXIT2)
                    {
                        printf("id : %d logout\n", pack->info.id);
                        memset(buf, 0, sizeof(buf));
                        sprintf(buf, "update login_info set status = 0 where id = %d", pack->info.id);
                        if(mysql_query(&mysql, buf) < 0)
                            my_error("mysql_query failed", __LINE__);

                        pthread_mutex_lock(&mutex);
                        List_ForEach(plist, curPos)
                        {
                            if(curPos->id == pack->info.id)
                            {
                                List_FreeNode(curPos);
                                break;
                            }
                        }
                        pthread_mutex_unlock(&mutex);
                    }
                    pthread_create(&tid, NULL, task, (void *)pack);
                }
            }
        }
    }
    close(lfd);
    mysql_close(&mysql);
	mysql_library_end();
    free(pack);
    return 0;
}