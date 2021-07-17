#include <stdio.h>

#include "common.h"

#define PORT 7777

#define LOGIN 1
#define REGISTER 2
#define FIND_PASSWORD 3
#define EXIT 4
#define FIND_ANSWER 5
#define GET_NEW_PW 6

#define ERROR_id 7
#define RIGHT_id 8
#define ERROR_password 9
#define ERROR_match 10
#define ERROR_choice 11
#define SUCCESS_login 12
#define SUCCESS_register 13
#define ERROR_answer 14
#define RIGHT_answer 15
#define SUCCESS_find 16

#define PERSONAL_SETTINGS 17
#define ADD_FRIEND 18
#define VIEW_FRIENDS_RQ 19
#define VIEW_FRIENDS_LIST 20
#define CHAT_WITH_SB 21
#define VIEW_GROUPS_LIST 22
#define CHAT_WITH_GROUPS 23
#define RETURN_LOGIN 24
#define DELETE_FRIEND 25

#define VIEW_PER_INFO 26
#define CHANGE_NAME 27
#define CHANGE_PASSWORD 28
#define CHANGE_QUESTION 29
#define CHANGE_ANSWER 30
#define RETURN_OPT1 31

#define RETURN_PER_SET 32
#define SUCCESS_CH_NA 33
#define ERROR_CH_PW 34
#define SUCCESS_CH_PW 35
#define SUCCESS_CH_Q 36
#define SUCCESS_CH_A 37

#define NOT_FRIEND 43
#define SUCCESS 44
#define IS_FRIEND 45

#define FRIEND 38
#define HAVE_SENT 39
#define SEND_SUCCESS 40

#define PROCESS_FRQ 44


#define SET_group 50
#define ADD_group 51
#define VIEW_grouprq 52
#define VIEW_grouplist 53
#define VIEW_groupinfo 54
#define REMOVE_member 55
#define EXIT_group 56
#define SET_admini 57
#define DISSOLVE_group 58
#define GROUP_msg 59

#define GET_FRIMSG 42


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
    int sfd;
    int serfd;
    int cid;
    char sendbuf[200];
    user_date_t date;
    user_time_t time;
}Data;

typedef struct Friend
{
    int id;
    char name[20];
    int status;
}friend_t;

typedef struct Frind_msg
{
    char name1[20];
    char name2[20];
    char msgbuf[200];
    user_date_t date;
    user_time_t time;
}friendmsg_t;

typedef struct Pack
{
    int num;
    Per_info info;
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
    int num;
	friend_t data;
	struct Friend_node *next;
	struct Friend_node *prev;
}friend_node_t, *friend_list_t;

typedef struct Friendmsg_node
{
    int num;
    friendmsg_t data;
    struct Friendmsg_node *next;
    struct Friendmsg_node *prev;
}friendmsg_node_t, *friendmsg_list_t;
