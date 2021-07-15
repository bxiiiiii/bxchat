#include <stdio.h>

#include "common.h"

#define PORT 8888

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

#define VIEW_PER_INFO 25
#define CHANGE_NAME 26
#define CHANGE_PASSWORD 27 
#define CHANGE_QUESTION 28
#define CHANGE_ANSWER 29
#define RETURN_OPT1 30

#define RETURN_PER_SET 31
#define SUCCESS_CH_NA 32
#define ERROR_CH_PW 33
#define SUCCESS_CH_PW 34
#define SUCCESS_CH_Q 35
#define SUCCESS_CH_A 36

#define FRIEND 37
#define HAVE_SENT 38
#define SEND_SUCCESS 39

#define PROCESS_FRQ 40

#define GET_FRIMSG 41


#define CHECK 222
#define fri 100
#define sb 101
#define gro 102

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