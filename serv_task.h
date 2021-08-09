#ifndef _STASK_H
#define _STASK_H

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>

#include "s_d.h"
#include "list.h"
#include "common.c"

MYSQL Mysql_con();
void my_error(char *str, int line);
void Login(Pack *pack);
void Check(Pack *pack);
void Per_set(Pack *pack);
void Add_friend(Pack *pack);
void View_friendrq(Pack *pack);
void View_friendlist(Pack *pack);
void Process_friendrq(Pack *pack);
void Chat_sb(Pack *pack);
void Friend_msg(Pack *pack);
void Delete_friend(Pack *pack);
void Is_friend(Pack *pack);
void Set_group(Pack *pack);
void Add_group(Pack *pack);
void view_grouprq(Pack *pack);
void View_grouplist(Pack *pack);
void Transfer_group(Pack *pack);
void Group_msg(Pack *pack);
void View_groupinfo(Pack *pack);
void Remove_member(Pack *pack);
void Exit_group(Pack *pack);
void Set_admini(Pack *pack);
void Dissolve_group(Pack *pack);
void Get_status(Pack *pack);
void Process_grouprq(Pack *pack);
void View_friendinfo(Pack *pack);
void Group_chat(Pack *pack);
void Shield_friend(Pack *pack);
void Send_file(Pack *pack);
void Recv_file(Pack *pack);
void Cancel_shield(Pack *pack);
void Is_shield(Pack *pack);
void File_info(Pack *pack);
void View_filelist(Pack *pack);
void Cancel_admini(Pack *pack);
void Shield(Pack *pack);
void Is_group(Pack *pack);
void *task(void *arg);

MYSQL mysql;
per_list_t plist;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#endif