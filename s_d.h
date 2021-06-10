#include <stdio.h>

#define PORT 9999

#define LOGIN 1
#define REGISTER 2
#define FIND_PASSWORD 3
#define EXIT 4
#define FIND_ANSWER 5
#define GET_NEW_PW 6

#define ERROR_ID 7
#define RIGHT_ID 8
#define ERROR_PASSWORD 9
#define ERROR_REGISTER 10
#define ERROR_NUM 11
#define SUCCESS_LOGIN 12
#define SUCCESS_REGISTER 13
#define ERROR_ANSWER 14
#define RIGHT_ANSWER 15
#define SUCCESS_FIND 16

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


typedef struct Per_Info
{
    int id;
    char name[20];
    char password[16];
    char question[30];
    char answer[20];
    int cfd;
    int sfd;
    int choice;
    int status;
    int fid; 
}per_info;