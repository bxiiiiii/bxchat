#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "s_d.h"
#include "list.h"
#include "common.c"

#define SERV_IP "127.0.0.1"

void Login_opt(Pack *pack);
void Opt1();
void Check(Pack *pack);
void Per_set(Pack *pack);
void *Recv_pthr(void *arg);
void View_friendlist(Pack *pack);
void View_friendrq(Pack *pack);
void Add_friend(Pack *pack);
void Process_friendrq(Pack *pack);
void Chat_sb(Pack *pack);
void Friend_msg(Pack *pack);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ccid;

int main()
{
    pthread_t tid;
	int cfd, choice1, ret;
	struct sockaddr_in s_addr;
	Pack* pack = (Pack *)malloc(sizeof(Pack));


	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERV_IP, &s_addr.sin_addr.s_addr);
	
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	connect(cfd, (struct sockaddr *)&s_addr, sizeof(s_addr));

    pthread_create(&tid, NULL, Recv_pthr, (void *)pack);

	pack->data.sfd = cfd;
	Login_opt(pack);
	if(pack->choice == EXIT)
		return 0;

	//Check(pack);
	do
	{
		Opt1(pack);
		if(pack->choice == RETURN_LOGIN)
			Login_opt(pack);
		if(pack->choice == EXIT)
			break;
	}while(1);
	

    pthread_join(tid, NULL);
	return 0;
}

void* Recv_pthr(void *arg)
{
   // pthread_detach(tid);
   	Pack tmp, tmp2;
	//int ttid;
	char recbuf[200];
	Pack *pack = (Pack *)arg;

    while(1)
    {
		tmp = *pack;
        int n = recv(pack->data.sfd, pack, sizeof(Pack), 0);
		//ttid = pack->info.id;
		tmp2 = *pack;
		//memset(recbuf, 0, sizeof(recbuf));
		//strcpy(recbuf, pack->data.sendbuf);
		printf("*%d\n", pack->status);
		switch (pack->status)
		{
			case fri:
				printf("\n[There's a new friend request.]\n");
				*pack = tmp;
				fflush(stdin);
				break;
			
			case sb:
				*pack = tmp;
				//printf("%d* %d %d\n",pack->choice, ttid,ccid);
				
				if(((pack->choice == GET_FRIMSG) || (pack->choice == CHAT_WITH_SB)) && (tmp2.info.id == ccid))
				{
					printf("[%s %d-%d %d:%d:%d]\n", tmp2.fmnode.name1, tmp2.fmnode.date.month, tmp2.fmnode.date.day,
																	tmp2.fmnode.time.hour, tmp2.fmnode.time.minute, tmp2.fmnode.time.second);
					printf("%s\n", tmp2.data.sendbuf);
				}
				else
					printf("\n[There's a new friend message.]\n");
				fflush(stdin);
				break;

			case gro:
				printf("\n[T]here's a new group message.]\n");
				*pack = tmp;
				fflush(stdin);
				break;

			default:
				pthread_cond_signal(&cond);
				break;
		}
    }
}


void Login_opt(Pack *pack)
{
	char password_buf1[16];
	char password_buf2[16];

	do
	{
		printf("-------------------\n");
		printf("[1]Login.\n");
		printf("[2]Register.\n");
		printf("[3]Find password.\n");
		printf("[4]Exit.\n");
		printf("-------------------\n");
		printf("Please enter your choice: ");
		scanf("%d", &pack->choice);

		switch(pack->choice)
		{
			case LOGIN:
				printf("Please enter your ID：");
				scanf("%d", &pack->info.id);
				printf("Please enter your password：");
				scanf("%s", pack->info.password);
			//	printf("choice = %d  id:%d  pass:%s\n",pack->choice, pack->info.id, pack->info.password);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
                pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
				break;

			case REGISTER:
				printf("Please enter name:");
				scanf("%s", pack->info.name);
				printf("Please enter password:");
				scanf("%s", password_buf1);
				printf("Please enter password again:");
				scanf("%s", password_buf2);	
				printf("Please enter your question for finding password:");
				scanf("%s", pack->info.question);	
				printf("Please enter your answer:");
				scanf("%s", pack->info.answer);

				if(strcmp(password_buf2, password_buf1) != 0)
					pack->status = ERROR_match;
				else
				{
					strcpy(pack->info.password, password_buf1);
					pack->choice = REGISTER;
					send(pack->data.sfd, pack, sizeof(pack), 0);
                    pthread_cond_wait(&cond, &mutex);
                    pthread_mutex_unlock(&mutex);
				}
				break;

			case FIND_PASSWORD:
				printf("Please enter your id:");
				scanf("%d", &pack->info.id);
				pack->choice = FIND_PASSWORD;
				send(pack->data.sfd, pack, sizeof(pack), 0);
				pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
				if(pack->status == ERROR_id)
					break;
				printf("%s: ",pack->info.question);
				scanf("%s", pack->info.answer);
				
				pack->choice = FIND_ANSWER;
				send(pack->data.sfd, pack, sizeof(pack), 0);
				pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
				break;

			case EXIT:
				send(pack->data.sfd, pack, sizeof(pack), 0);
				exit(0);

			default:
				pack->status = ERROR_choice;
				break;
		}

		//printf("***%d\n", pack->status);
		switch(pack->status)
		{
			case ERROR_id:
				printf("ID error.\n");
				break;
			case ERROR_password:
				printf("Password error.\n");
				break;
			case ERROR_match:
				printf("The two passwords do not match.\n");
				break;
			case SUCCESS_login:
				printf("Login success.\n");
				return ;

			case SUCCESS_register:
				printf("Register success, your id is %d.\n", pack->info.id);
				break;
			case ERROR_answer:
				printf("Answer error.\n");
				break;
			case RIGHT_answer:
				do
				{
					printf("Please enter new password: ");
					scanf("%s", password_buf1);
					printf("Please enter new password again: ");
					scanf("%s", password_buf2);
					if(strcmp(password_buf2, password_buf1) != 0)
					{
						printf("The two passwords do not match.\n");
					}
					else
					{
						strcpy(pack->info.password, password_buf1);
						pack->choice = GET_NEW_PW;
                        send(pack->data.sfd, pack, sizeof(pack), 0);
                        pthread_cond_wait(&cond, &mutex);
                        pthread_mutex_unlock(&mutex);
					}
				}while(pack->status != SUCCESS_find);
				printf("Find password success.\n");
				break;
			case ERROR_choice:
				printf("Please enter the correct number！\n");
				break;
		}
	//printf("***%d\n", pack->status);
	}while(pack->status != SUCCESS_login);
}

void Check(Pack *pack)
{
	pack->choice = CHECK;
	send(pack->data.sfd, pack, sizeof(Pack), 0);


    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
	while(1)
	{
		switch (pack->status)
		{
			case ADD_FRIEND:
				printf("\n[There's a new friend request.]\n");
				break;

			case CHAT_WITH_SB:
				printf("\n[There's a new friend message.]\n");
				break;

			case CHAT_WITH_GROUPS:
				printf("\n[T]here's a new group message.]\n");
				break;
			default:
				printf("[There's nothing.]\n");
				break;
		}
	}
}

void Opt1(Pack *pack)
{

	printf("-----------------------\n");
	printf("[1]Personal settings.\n");
	printf("[2]Add friend.\n");
	printf("[3]View friend requests.\n");
	printf("[4]View friends list.\n");
	printf("[5]Chat with sb.\n");
	printf("[6]View group list.\n");
	printf("[7]Chat with group.\n");
	printf("[8]exit.\n");
	printf("[9]Delete friend.\n");
	printf("-----------------------\n");
	printf("Please enter your choice: ");
	scanf("%d", &pack->choice);

	pack->choice += 16;

	switch (pack->choice)
	{
	case PERSONAL_SETTINGS:
		Per_set(pack);
		break;

	case ADD_FRIEND:
		Add_friend(pack);
		break;
	
	case VIEW_FRIENDS_RQ:
		View_friendrq(pack);
		break;

	case VIEW_FRIENDS_LIST:
		View_friendlist(pack);
	 	break;

	case CHAT_WITH_SB:
		Chat_sb(pack);
		break;
	
	case VIEW_GROUPS_LIST:
		break;

	case CHAT_WITH_GROUPS:
		break;

	case RETURN_LOGIN:
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		break;
	
	//case DELETE_FRIEND:
	default:
		printf("Please enter the correct number！\n");
		break;
	}
}

void Per_set(Pack *pack)
{
	char password_buf1[16];
	char password_buf2[16];
	int a;
	
	while(1)
	{
		printf("----------------------------------------\n");
		printf("[1]View your infomation.\n");
		printf("[2]Change name.\n");
		printf("[3]Change password.\n");
		printf("[4]Change question for finding password.\n");
		printf("[5]Change answer for question.\n");
		printf("[6]Exit.\n");
		printf("----------------------------------------\n");
		printf("Please enter your choice: ");
		scanf("%d", &a);
		pack->choice = a+24;

		switch (pack->choice)
		{
			case VIEW_PER_INFO:
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
				printf("Name : %s\n", pack->info.name);
				printf("Question for finding password: %s\n", pack->info.question);
				pack->status = RETURN_PER_SET;
				break;

			case CHANGE_NAME:
				printf("Please enter your new name: ");
				scanf("%s", pack->info.name);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
				break;

			case CHANGE_PASSWORD:
				printf("Please enter new password: ");
				scanf("%s", password_buf1);
				printf("Please enter new password again: ");
				scanf("%s", password_buf2);
				if(strcmp(password_buf2, password_buf1) != 0)
					pack->status = ERROR_CH_PW;
				else
				{
					strcpy(pack->info.password, password_buf1);
					pack->choice = GET_NEW_PW;
					send(pack->data.sfd, pack, sizeof(Pack), 0);
					pthread_cond_wait(&cond, &mutex);
					pthread_mutex_unlock(&mutex);
				}
				break;

			case CHANGE_QUESTION:
				printf("Please enter your new question: ");
				scanf("%s", pack->info.question);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
				break;

			case CHANGE_ANSWER:
				printf("Please enter your new answer: ");
				scanf("%s", pack->info.answer);
				//printf("%s\n", pack->info.answer);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
				break;

			case RETURN_OPT1:
				return ;

			default:
				pack->info.status = ERROR_choice;
				break;
		}


		switch (pack->status)
		{
			case SUCCESS_find:
				printf("Change password success.\n");
				break;
			case SUCCESS_CH_NA:
				printf("Change name success.\n");
				break;
			case ERROR_CH_PW:
				printf("The two passwords do not match.\n");
				break;
			case SUCCESS_CH_Q:
				printf("Change question success.\n");
				break;
			case SUCCESS_CH_A:
				printf("Change answer success.\n");
				break;
			case ERROR_choice:
				printf("Please enter the correct number！\n");
				break;
		}
	}
}

void Add_friend(Pack *pack)
{
    printf("please enter id of friend: ");
	scanf("%d", &pack->data.cid);
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    switch (pack->status)
    {
        case ERROR_id:
            printf("The ID does not exist.\n");
            break;
        case FRIEND:
            printf("The ID is already a friend with you.\n");
            break;
        case HAVE_SENT:
            printf("The friend request has been sent, please do not send it again.\n");
            break;
        case SEND_SUCCESS:
            printf("Friend request sent successfully.\n");
            break;
    }
}

void View_friendrq(Pack *pack)
{
	int i;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

	if(pack->status == 0)
	{
		printf("No record.\n");
	}
	else
	{
		printf("--------Friend request List---------\n");
		printf("ID   Name\n");
		printf("------------------------------------\n");
		for(i = 0; i < pack->num; i++)
		{
			pthread_cond_wait(&cond, &mutex);
    		pthread_mutex_unlock(&mutex);
			printf("%-3d  %-20s  \n", pack->fnode.id, pack->fnode.name);
		}
		Process_friendrq(pack);
	}
}

void View_friendlist(Pack *pack)
{
	int i;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

	if(pack->status == 0)
	{
		printf("No record.\n");
	}
	else
	{
		printf("--------Friends List---------\n");
		printf("id   name                  status\n");
		for(i = 0; i < pack->num; i++)
		{
			pthread_cond_wait(&cond, &mutex);
    		pthread_mutex_unlock(&mutex);
			printf("%-3d  %-20s  ", pack->fnode.id, pack->fnode.name);
			if(pack->fnode.status == 1)
				printf("online\n");
			else
				printf("offline\n");
		}
	}
}

void Process_friendrq(Pack *pack)
{
	pack->choice = PROCESS_FRQ;
	while(1)
	{
		printf("Please enter the id you want to process:");
		scanf("%d", &pack->data.cid);
		printf("[1]Agree.    [2]Disagree.    [3]Return.\n");
		printf("Please enter your choice: ");
		scanf("%d", &pack->status);
		if(pack->status < 1  && pack->status > 2)
			break;
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		printf("Process succseefully.\n");

		printf("[1]Continue to process    [2]Return\n");
		scanf("%d", &pack->status);
		if(pack->status != 1)
			break;
	}
}

void Chat_sb(Pack *pack)
{
	printf("Please enter the id you want to chat: ");
	scanf("%d", &pack->data.cid);
	ccid = pack->data.cid;
	Friend_msg(pack);
	
	while(1)
	{
		//printf("Enter your msg or enter 'y' to exit: ");
		memset(pack->data.sendbuf, 0, sizeof(pack->data.sendbuf));
		fflush(stdin);
		//fgets(pack->data.sendbuf, 200, stdin);
		scanf("%s", pack->data.sendbuf);
		//pack->data.sendbuf[strlen(pack->data.sendbuf)-1] = '\0';
		if(!(strcmp(pack->data.sendbuf, "y")))
			return ;
		pack->fmnode.date = DateNow();
		pack->fmnode.time = TimeNow();
		pack->choice = CHAT_WITH_SB;
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		printf("                                        [%s %d-%d %d:%d:%d]\n", pack->info.name, pack->fmnode.date.month, pack->fmnode.date.day,
																	pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
		printf("%s\n",pack->data.sendbuf);
	}
}

void Friend_msg(Pack *pack)
{
	pack->choice = GET_FRIMSG;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

	if(pack->num == 0)
	{
		printf("No record.\n");
	}
	else
	{
	//	printf("---------%s---------\n", pack->fmnode.name2);
		for(int i = 0; i < pack->num; i++)
		{
				    printf("%d**\n", pack->num);
			pthread_cond_wait(&cond, &mutex);
    		pthread_mutex_unlock(&mutex);
						    printf("**\n");
			if(strcmp(pack->fmnode.name1, pack->info.name))
			{
				printf("[%s %d-%d %d:%d:%d]\n", pack->fmnode.name1, pack->fmnode.date.month, pack->fmnode.date.day,
																	pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
				printf("%s\n", pack->fmnode.msgbuf);
			}
			else
			{
				printf("***\n");
				printf("                                        [%s %d-%d %d:%d:%d]\n", pack->fmnode.name1, pack->fmnode.date.month, pack->fmnode.date.day,
																	pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
				printf("%s\n", pack->fmnode.msgbuf);
			}
		}
	}
}