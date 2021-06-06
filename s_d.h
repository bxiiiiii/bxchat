#include <stdio.h>

#define PORT 9999

#define LOGIN 1
#define REGISTER 2
#define FIND_PASSWORD 3
#define EXIT 4

#define ERROR_ID 5
#define ERROR_PASSWORD 6
#define ERROR_REGISTER 7
#define ERROR_NUM 8
#define SUCCESS_LOGIN 9
#define SUCCESS_REGISTER 10
#define ERROR_ANSWER 11
#define RIGHT_ANSWER 12
#define SUCCESS_FIND 13

#define PERSONAL_SETTINGS 14
#define ADD_FRIEND 15
#define VIEW_FRIENDS_LIST 16
#define CHAT_WITH_SB 17
#define VIEW_GROUPS_LIST 18
#define CHAT_WITH_GROUPS 19
#define EXITT 20

#define VIEW_PER_INFO 21
#define CHANGE_NAME 22
#define CHANGE_PASSWORD 23 
#define CHANGE_QUESTION 24
#define CHANGE ANSWER 25
#define SUCCESS_CH_NA 26


typedef struct Per_Info
{
    int choice;
    int id;
    char name[20];
    char password[16];
    char question[30];
    char answer[20];
    int cfd;
    int sfd;
    int status;

}per_info;