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
void Opt1(Pack *pack);
void Opt2(Pack *pack);
void Check(Pack *pack);
void Per_set(Pack *pack);
void *Recv_pthr(void *arg);
void View_friendlist(Pack *pack);
void View_friendrq(Pack *pack);
void Add_friend(Pack *pack);
void Process_friendrq(Pack *pack);
void Chat_sb(Pack *pack);
void Send_file(Pack *pack);
void Friend_msg(Pack *pack);
void Delete_friend(Pack *pack);
void Set_group(Pack *pack);
void Add_group(Pack *pack);
void view_grouprq(Pack *pack);
void View_grouplist(Pack *pack);
void View_groupinfo(Pack *pack);
void Remove_member(Pack *pack);
void Exit_group(Pack *pack);
void Set_admini(Pack *pack);
void Dissolve_group(Pack *pack);
void Transfer_group(Pack *pack);
void Group_msg(Pack *pack);
void Get_status(Pack *pack);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ccid;
int ggid

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
			{
				Login_opt(pack);
				Check(pack);
			}
		if(pack->choice == EXIT)
			break;
		Opt2(pack);
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
	int i;

    while(1)
    {
		//pthread_mutex_lock(&mutex);
		tmp = *pack;
        int n = recv(pack->data.sfd, pack, sizeof(Pack), 0);
		i++;
		//printf("%d\n", i);
		//ttid = pack->info.id;
		tmp2 = *pack;
		//memset(recbuf, 0, sizeof(recbuf));
		//strcpy(recbuf, pack->data.sendbuf);
		//printf("*%d\n", pack->status);
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

			case mem:
				*pack = tmp;
				//printf("%d* %d %d\n",pack->choice, ttid,ccid);
				
				if(((pack->choice == GROUP_msg) || (pack->choice == CHAT_WITH_GROUPS)) && (tmp2.info.id == ccid))
				{
					printf("[%s %d-%d %d:%d:%d]\n", tmp2.fmnode.name1, tmp2.fmnode.date.month, tmp2.fmnode.date.day,
																	tmp2.fmnode.time.hour, tmp2.fmnode.time.minute, tmp2.fmnode.time.second);
					printf("%s\n", tmp2.data.sendbuf);
				}
				else
					printf("\n[There's a new group message.]\n");
				fflush(stdin);
				break;			

			case gro:
				printf("\n[There's a new group message.]\n");
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
		printf("[1]登陆\n");
		printf("[2]注册\n");
		printf("[3]找回密码\n");
		printf("[4]退出\n");
		printf("-------------------\n");
		printf("输入你的选择: ");
		scanf("%d", &pack->choice);

		switch(pack->choice)
		{
			case LOGIN:
				printf("账号：");
				scanf("%d", &pack->info.id);
				printf("密码：");
				scanf("%s", pack->info.password);
			//	printf("choice = %d  id:%d  pass:%s\n",pack->choice, pack->info.id, pack->info.password);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
                pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
				break;

			case REGISTER:
				printf("请输入名字:");
				scanf("%s", pack->info.name);
				printf("请输入密码:");
				scanf("%s", password_buf1);
				printf("请再次输入密码:");
				scanf("%s", password_buf2);	
				printf("设置密保:");
				scanf("%s", pack->info.question);	
				printf("请输入答案:");
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
				printf("请输入待找回的账号:");
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
				printf("该账号不存在\n");
				break;
			case ERROR_password:
				printf("密码错误\n");
				break;
			case ERROR_match:
				printf("两次密码不一致\n");
				break;
			case SUCCESS_login:
				printf("登陆成功\n");
				return ;

			case SUCCESS_register:
				printf("注册成功, 你的账号为%d\n", pack->info.id);
				break;
			case ERROR_answer:
				printf("答案错误\n");
				break;
			case RIGHT_answer:
				do
				{
					printf("请输入新的密码:");
					scanf("%s", password_buf1);
					printf("请再次输入密码:");
					scanf("%s", password_buf2);
					if(strcmp(password_buf2, password_buf1) != 0)
					{
						printf("两次密码不一致\n");
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
				printf("找回密码成功\n");
				break;
			case ERROR_choice:
				printf("请输入正确选择\n");
				break;
		}
	//printf("***%d\n", pack->status);
	}while(pack->status != SUCCESS_login);
}

void Check(Pack *pack)
{
	int i;

	pack->choice = CHECK;
	send(pack->data.sfd, pack, sizeof(Pack), 0);


    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
	if(pack->num)
		printf("[There's a new friend request.]\n");

	printf("%d\n", pack->num);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
	printf("%d\n", pack->num);
	for(i = 0; i < pack->num; i++)
	{
		pthread_cond_wait(&cond, &mutex);
    	pthread_mutex_unlock(&mutex);
		printf("[There's a new message of %s(%d).]\n", pack->fnode.name, pack->fnode.id);
	}

	/*

	*/
}

void Opt1(Pack *pack)
{

	printf("-----------------------\n");
	printf("[1]个人设置.\n");
	printf("[2]添加好友.\n");
	printf("[3]查看好友请求.\n");
	printf("[4]查看好友列表.\n");
	printf("[5]Chat with sb.\n");
	printf("[6]View group list.\n");
	printf("[7]Chat with group.\n");
	printf("[8]退出.\n");
	printf("[9]删除好友\n");
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
		View_grouplist(pack);
		break;

	case CHAT_WITH_GROUPS:
		break;

	case RETURN_LOGIN:
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		break;
	
	case DELETE_FRIEND:
		Delete_friend(pack);
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
		printf("[1]查看个人信息\n");
		printf("[2]更改名字\n");
		printf("[3]更改密码\n");
		printf("[4]更改密保\n");
		printf("[5]更改答案\n");
		printf("[6]退出\n");
		printf("----------------------------------------\n");
		printf("Please enter your choice: ");
		scanf("%d", &a);
		pack->choice = a+25;

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
			pthread_mutex_lock(&mutex);
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

	pack->choice = IS_FRIEND;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);

	switch (pack->status)
	{
	case ERROR_id:
		printf("The ID does not exist.\n");
		return ;
	
	case NOT_FRIEND:
		printf("The ID is not a friend with you.\n");
		return;

	case SUCCESS:
		break;

	default:
		return ;
	}

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
		{
			ccid = 0;
			return ;
		}
		pack->fmnode.date = DateNow();
		pack->fmnode.time = TimeNow();
		pack->choice = CHAT_WITH_SB;
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		//printf("                                                    [%s %d-%d %d:%d:%d]\n", pack->info.name, pack->fmnode.date.month, pack->fmnode.date.day,
		//															pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
		//printf("%-s\n",pack->data.sendbuf);
		system("clear");
		Friend_msg(pack);
	}
}

void Send_file(Pack *pack)
{

}

void Friend_msg(Pack *pack)
{
	pack->choice = GET_FRIMSG;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
	printf("**%d\n", pack->num);

	if(pack->num == 0)
	{
		printf("No record.\n");
	}
	else
	{
		printf("---------%s---------\n", pack->fmnode.name2);
		for(int i = 0; i < pack->num; i++)
		{
			printf("**%d\n", i);
			//	    printf("%d**\n", pack->num);
			pthread_mutex_lock(&mutex);
			pthread_cond_wait(&cond, &mutex);
    		pthread_mutex_unlock(&mutex);
			//			    printf("**\n");
			if(strcmp(pack->fmnode.name1, pack->info.name))
			{
				printf("[%s %d-%d %d:%d:%d]\n", pack->fmnode.name1, pack->fmnode.date.month, pack->fmnode.date.day,
																	pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
				printf("%s\n", pack->fmnode.msgbuf);
			}
			else
			{
				printf("                                        [%s %d-%d %d:%d:%d]\n", pack->fmnode.name1, pack->fmnode.date.month, pack->fmnode.date.day,
																	pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
				printf("%s\n", pack->fmnode.msgbuf);
			}
		}
	}
	printf("***\n");
}

void Delete_friend(Pack *pack)
{
	printf("Enter friend id you want to delete:");
	scanf("%d", &pack->data.cid);
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);

	switch (pack->status)
	{
	case ERROR_id:
		printf("The ID does not exist.\n");
		break;
	
	case NOT_FRIEND:
		printf("The ID is not a friend with you.\n");
		break;

	case SUCCESS:
		printf("Delete successfully.\n");
		break;

	default:
		break;
	}
}

void Set_group(Pack *pack)
{
	printf("Please enter new group name:");
	scanf("%s", pack->fnode.name);
	pack->choice = SET_group;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	printf("创建成功\n");
}

void Add_group(Pack *pack)
{
    printf("输入待加入群id:: ");
	scanf("%d", &pack->data.cid);
	pack->choice = ADD_group;
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    switch (pack->status)
    {
        case -1:
            printf("该群ID不存在\n");
            break;
        case -3:
            printf("你已经是该群成员\n");
            break;
        case -2:
            printf("加群申请已发出，请勿重复发送\n");
            break;
        case 0:
            printf("发送成功\n");
            break;
    }
}

void View_grouprq(Pack *pack)
{
	int i;
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

	if(pack->num == 0)
	{
		printf("No record.\n");
	}
	else
	{
		printf("-----------------Group request List---------------------\n");
		printf("     Group                      Applicant\n");
		printf("ID   Name                  ID   Name\n");
		printf("--------------------------------------------------------\n");
		for(i = 0; i < pack->num; i++)
		{
			pthread_mutex_lock(&mutex);
			pthread_cond_wait(&cond, &mutex);
    		pthread_mutex_unlock(&mutex);
			printf("%-3d  %-20s  %-3d  %-20s\n", pack->fnode.id, pack->fnode.name, pack->data.cid, pack->data.sendbuf);
		}
		Process_friendrq(pack);
	}
}

void View_grouplist(Pack *pack)
{
	int i;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

	if(pack->num == 0)
	{
		printf("No record.\n");
	}
	else
	{
		printf("------------Group List------------\n");
		printf("id   name                  status\n");
		for(i = 0; i < pack->num; i++)
		{
			pthread_mutex_lock(&mutex);
			pthread_cond_wait(&cond, &mutex);
    		pthread_mutex_unlock(&mutex);
			printf("%-3d  %-20s  ", pack->fnode.id, pack->fnode.name);
			switch (pack->fnode.status)
			{
				case 0:
					printf("NOT\n");
				case 1:
					printf("Member\n");
					break;
				case 2:
					printf("Administrator\n");
					break;
				case 9:
					printf("Owner\n");
					break;
				default:
				printf("\n");
					break;
			}
		}
	}
}

void Group_msg(Pack *pack)
{
	
}

void View_groupinfo(Pack *pack)
{
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

	printf("---------%s---------", pack->fmnode.name1);
	printf("群id:%d\n", pack->data.cid);
	printf("群名:%s\n", pack->fmnode.name1);
	printf("群主id:%d", pack->fmnode.id);
	printf("群主:%s\n", pack->fmnode.name2);
	printf("群人数:%d\n", pack->num);
	printf("创建日期:%d-%d-%d\n", pack->fmnode.date.year, pack->fmnode.date.month, pack->fmnode.date.day);
}

void Remove_member(Pack *pack)
{
	printf("输入你要踢出该群聊的id:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
	switch (pack->status)
	{
		case -2:
			printf("不是该群成员\n");
		case -3:
			printf("无权限\n");
		case 0:
			printf("操作成功\n");
			break;
	}
}

void Exit_group(Pack *pack)
{
	printf("Enter group id you want to exit:");
	scanf("%d", &pack->data.cid);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
	switch (pack->status)
	{
		case -1 :
			printf("该群不存在\n");
			break;
		case 0:
			printf("退群成功\n");
			break;
	}
}

void Set_admini(Pack *pack)
{
	printf("输入需要设置的id:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
	switch (pack->status)
	{
		case -4 :
			printf("该id不存在\n");
			break;
		case -1:
			printf("不是该群成员\n");
			break;
		case -2:
			printf("该成员已是管理员,请勿重复设置\n");
			break;
		case -3:
			printf("你已经是群主\n");
			break;
		case 0:
			printf("设置成功\n");
			break;
	}
}

void Dissolve_group(Pack *pack)
{
	char ch;
	while(1)
	{
		printf("确认解散群（y\n):");
		fflush(stdin);
		ch = getchar();
		switch(ch)
		{
			case 'n':
				return;
			case 'y':
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				printf("解散成功\n");
				return;
			default:
				break;
		}
	}
}

void Transfer_group(Pack *pack)
{
	printf("输入需要转让的id:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
	switch (pack->status)
	{
		case -1 :
			printf("该id不存在\n");
			break;
		case -2:
			printf("不是该群成员\n");
			break;
		case -3:
			printf("你已经是群主\n");
			break;
		case 0:
			printf("设置成功\n");
			break;
	}
}


void Opt2(Pack *pack)
{
	while(1)
	{
		View_grouplist(pack);
		printf("---------------------");
		printf("[1]建群\n");
		printf("[2]查看群通知\n");
		printf("[3]加群\n");
		printf("[4]选择群\n");
		printf("[5]返回\n")
		printf("输入你的选择:\n");

		scanf("%d", &pack->choice);
		pack->choice = 45;
		switch(pack->choice)
		{
			case SET_group:
				Set_group(pack);
				break;
			case ADD_group:
				Add_group(pack);
				break;
			case VIEW_grouprq:
				View_grouprq(pack);
				break;
			case VIEW_grouprq+1:
				Opt3(pack);
				break;
			case EXIT5:
				return;
			default:
				printf("请输入正确选择\n");
				break;

		}
	}
}

void Get_status(Pack *pack)
{
	pack->choice = GET_status；
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);
}

void Opt3(Pack *pack)
{
	printf("输入待选择的群id:");
	scanf("%d", &pack->data.cid);
	ggid = pack->data.cid;
	
	Get_status(pack);
	switch (pack->status)
	{
		case 1:
		while(1)
			{
				printf("[1]聊天\n");
				printf("[2]查看群\n");
				printf("[3]退群\n");
				printf("[4]返回\n");
				printf("输入你的选择:\n");

				scanf("%d", pack->choice);
				pack->choice += 50;
				switch (pack->choice)
				{
					case EXIT_group:
						Exit_group(pack);
						break;
					case EXIT6:
						pack->data.cid = 0;
						return ;
					case VIEW_groupinfo:
						View_groupinfo(pack);
						break;
					case GROUP_chat:

						break;
					default:
						printf("请输入正确选择\n");
						break;

				}
			}
			break;

		case 2:
			while(1)
			{
				printf("[1]聊天\n");
				printf("[2]查看群\n");
				printf("[3]退群\n");
				printf("[4]返回\n");
				printf("[5]踢人\n");
				printf("输入你的选择:\n");

				scanf("%d", pack->choice);
				pack->choice += 50;
				switch (pack->choice)
				{
					case EXIT_group:
						Exit_group(pack);
						break;
					case EXIT6:
						pack->data.cid = 0;
						return ;
					case REMOVE_member:
						Remove_member(pack);
						break;
					case VIEW_groupinfo:
						View_groupinfo(pack);
						break;
					case GROUP_chat:

						break;
					default:
						printf("请输入正确选择\n");
						break;

				}
			}
				break;
		
		case 9:
			while(1)
			{
				printf("[1]聊天\n");
				printf("[2]查看群\n");
				printf("[3]退群\n");
				printf("[4]返回\n");
				printf("[5]踢人\n");
				printf("[6]设置管理员\n");
				printf("[7]转让群\n")
				printf("[8]解散群\n");
				printf("输入你的选择:");

				scanf("%d", pack->choice);
				pack->choice += 50;
				switch (pack->choice)
				{
					case DISSOLVE_group:
						Dissolve_group(pack);
						break;
					case TRANSFER_group:
						Transfer_group(pack);
						break;
					case Set_admini:
						Set_admini(pack);
						break;
					case EXIT_group:
						printf("群主不能直接退群，可以转让群再退群或解散群\n");
						break;
					case EXIT6:
						pack->data.cid = 0;
						return ;
					case REMOVE_member:
						Remove_member(pack);
						break;
					case VIEW_groupinfo:
						View_groupinfo(pack);
						break;
					case GROUP_chat:

						break;
					default:
						printf("请输入正确选择\n");
						break;

				}
			}
			break;
			default:
				break;
	}
}