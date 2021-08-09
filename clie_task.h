#ifndef _CTASK_H
#define _CTASK_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>

#include "s_d.h"
#include "list.h"
#include "common.c"

void Login_opt(Pack *basepack);
void Opt1(Pack *basepack);
void Opt2(Pack *basepack);
void Check(Pack basepack);
void Per_set(Pack basepack);
void *Recv_pthr(void *arg);
void View_friendlist(Pack basepack);
void View_friendrq(Pack basepack);
void Add_friend(Pack basepack);
void Process_friendrq(Pack basepack);
void View_friendinfo(Pack *pack);
void Friend_chat(Pack *pack);
void Send_file(Pack *pack);
void Recv_file(Pack *pack);
void Shield_friend(Pack *pack);
void Friend_msg(Pack *pack);
void Delete_friend(Pack *pack);
void Set_group(Pack *pack);
void Add_group(Pack *pack);
void View_grouprq(Pack *pack);
void View_grouplist(Pack *pack);
void View_groupinfo(Pack *pack);
void Remove_member(Pack *pack);
void Exit_group(Pack *pack);
void Set_admini(Pack *pack);
void Dissolve_group(Pack *pack);
void Transfer_group(Pack *pack);
void Group_msg(Pack *pack);
void Group_chat(Pack *pack);
void Get_status(Pack *pack);
void Opt3(Pack *pack);
void Process_grouprq(Pack *pack);
void Opt4(Pack basepack);
void Opt5(Pack basepack);
int Is_shield(Pack *pack);
int Is_friend(Pack *pack);
int Shield(Pack *pack);
void View_filelist(Pack *pack);
void Cancel_admini(Pack *pack);
int Is_group(Pack *pack);
void Recv_func(Pack *pack);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pack_list_t plist;
int ccid;
int ggid;

#endif