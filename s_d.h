#include <stdio.h>

#define PORT 8888

#define LOGIN 1
#define REGISTER 2
#define FIND_PASSWORD 3
#define FIND_ANSWER 4
#define GET_NEW_PW 5
#define EXIT 6

#define ERROR_ID 7
#define ERROR_PASSWORD 8
#define ERROR_REGISTER 9
#define ERROR_NUM 10
#define SUCCESS_LOGIN 11
#define SUCCESS_REGISTER 12
#define ERROR_ANSWER 13
#define RIGHT_ANSWER 14
#define SUCCESS_FIND 15

#define PERSONAL_SETTINGS 16
#define ADD_FRIEND 17
#define VIEW_FRIENDS_LIST 18
#define CHAT_WITH_SB 19
#define VIEW_GROUPS_LIST 20
#define CHAT_WITH_GROUPS 21
#define RETURN_LOGIN 22

#define VIEW_PER_INFO 23
#define CHANGE_NAME 24
#define CHANGE_PASSWORD 25 
#define CHANGE_QUESTION 26
#define CHANGE_ANSWER 27
#define RETURN_OPT1 28

#define RETURN_PER_SET 29
#define SUCCESS_CH_NA 30
#define ERROR_CH_PW 31
#define SUCCESS_CH_PW 32


typedef struct Per_Info
{
    int id;
    int choice;
    char name[20];
    char password[16];
    char question[30];
    char answer[20];
    int cfd;
    int sfd;
    int status;

}per_info;