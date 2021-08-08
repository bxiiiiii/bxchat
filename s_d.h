#ifndef _S_H
#define _S_H

#include "common.h"

#define PORT 7878

#define LOGIN 1
#define REGISTER 2
#define FIND_PASSWORD 3
#define EXIT1 4
#define FIND_ANSWER 5
#define GET_NEW_PW 6

#define PER_set 10
#define FRIEND 11
#define GROUP 12
#define EXIT2 13


#define VIEW_perinfo 20
#define CHANGE_name 21
#define CHANGE_password 22
#define CHANGE_question 23
#define CHANGE_answer 24
#define EXIT3 25


#define VIEW_friendlist 30

#define ADD_friend 31
#define VIEW_friendrq 32
#define OPT5 33
#define EXIT4 34


#define VIEW_friendinfo 40
#define FRIEND_chat 41
#define FILE_send 42
#define FILE_recv 43
#define SHLELD_friend 44
#define DELETE_friend 45
#define EXIT5 46

#define IS_friend 47
#define PROCESS_frq 48
#define GET_friendmsg 49
#define CANCEL_shield 50
#define IS_shield 51
#define VIEW_filelist 52
#define FILE_info 53

#define VIEW_grouplist 54

#define SET_group 55
#define VIEW_grouprq 56
#define ADD_group 57
#define OPT3 58
#define EXIT6 59

#define GROUP_chat 65
#define VIEW_groupinfo 66
#define EXIT_group 67
#define EXIT7 68
#define REMOVE_member 69
#define SET_admini 70
#define TRANSFER_group 71
#define DISSOLVE_group 72
#define CANCEL_admini 73

#define GET_groupmsg 74
#define GET_status 75
#define PROCESS_grq 76
#define IS_group 77

#define SHIELD 151
#define CHECK 222

#define fri 100
#define sb 101
#define  mem 102
#define gro 103

typedef struct Per_Info
{
    int id;
    char name[20];
    char password[16];
    char question[30];
    char answer[20];
    int choice;
    int status;
}Per_info;


typedef struct Data
{
    int efd;
    int sfd;
    int serfd;
    int cid;
    char sendbuf[200];
}Data;

typedef struct Friend
{
    int id;
    char name[20];
    int status;
}friend_t;

typedef struct Frind_msg
{   
    int id;
//    int num;
    char name1[20];
    char name2[20];
    char msgbuf[200];
    user_date_t date;
    user_time_t time;
}friendmsg_t;

typedef struct{
    int num;
    int file_size;
    char file_name[128];
    //char buf[1024];
}file_t;

typedef struct Pack
{
    int num;
    Per_info info;
    file_t finode;
    friend_t fnode;
    friendmsg_t fmnode;
    Data data;
    int choice;
    int status;
    int type;
}Pack;

typedef struct Per_node
{
    int id;
    int serfd;
    struct Per_node *next;
    struct Per_node *prev;
}per_node_t, *per_list_t;

typedef struct Friend_node
{
    //int num;
	friend_t data;
	struct Friend_node *next;
	struct Friend_node *prev;
}friend_node_t, *friend_list_t;

typedef struct Friendmsg_node
{
   // int num;
    friendmsg_t data;
    struct Friendmsg_node *next;
    struct Friendmsg_node *prev;
}friendmsg_node_t, *friendmsg_list_t;

typedef struct File_node
{
    file_t data;
    struct File_node *next;
    struct File_node *prev;
}file_node_t, *file_list_t;

typedef struct Pack_node
{
    Pack data;
    struct Pack_node *next;
    struct Pack_node *prev;   
}pack_node_t, *pack_list_t;

#endif