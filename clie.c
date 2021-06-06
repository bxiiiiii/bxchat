#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "s_d.h"

#define SERV_IP "127.0.0.1"

void login_opt(per_info *info);
void opt1(per_info *Info);
void per_set(per_info *Info);

int main()
{
	char buf[BUFSIZ] = "hello";
	int cfd, choice1, ret;
	struct sockaddr_in s_addr;
	per_info info;

	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERV_IP, &s_addr.sin_addr.s_addr);
	
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	connect(cfd, (struct sockaddr *)&s_addr, sizeof(s_addr));

	info.cfd = cfd;

	login_opt(&info);

	

	return 0;
}

void login_opt(per_info *Info)
{
	char password_buf1[16];
	char password_buf2[16];
	per_info info = *Info;

	printf("1.登陆\n");
	printf("2.注册\n");
	printf("3.忘记密码\n");
	printf("4.退出\n");
	scanf("%d", &info.choice);

	switch(info.choice)
	{
		case LOGIN:
			printf("输入账号：\n");
			scanf("%d", &info.id);
			printf("输入密码：\n");
			scanf("%s", info.password);
			printf("id = %d, pw = %s, choice = %d\n", info.id, info.password, info.choice);
			send(info.cfd, &info, sizeof(info), 0);
			recv(info.cfd, &info.status, sizeof(info.status), 0);
			break;
		case REGISTER:
			printf("please enter name:");
			scanf("%s", info.name);
			printf("please enter password:");
			scanf("%s", password_buf1);
			printf("please enter password again:");
			scanf("%s", password_buf2);	
			printf("please enter your question for finding password:");
			scanf("%s", info.question);	
			printf("please enter your answer:");
			scanf("%s", info.answer);

			if(strcmp(password_buf2, password_buf1) != 0)
				info.status = ERROR_REGISTER;
			else
			{
				strcpy(info.password, password_buf1);
				info.choice = REGISTER;
				send(info.cfd, &info, sizeof(info), 0);
				recv(info.cfd, &info, sizeof(info), 0);
			}
			break;
		case FIND_PASSWORD:
			printf("please enter your id:");
			scanf("%d", &info.id);
			info.status = FIND_PASSWORD;
			send(info.cfd, &info, sizeof(info), 0);
			recv(info.cfd, info.question, sizeof(info.question), 0);
			printf("%s:",info.question);
			scanf("%s", info.answer);
			send(info.cfd, info.answer, sizeof(info.answer), 0);
			recv(info.cfd, &info.status, sizeof(info.status), 0);
			break;
		case EXIT:
			send(info.cfd, &info, sizeof(info), 0);
			exit(0);
			break;
		default:
			info.status = ERROR_NUM;
			break;
	}

	switch(info.status)
	{
		case ERROR_ID:
			printf("id error\n");
			login_opt(&info);
			break;
		case ERROR_PASSWORD:
			printf("password error\n");
			login_opt(&info);
			break;
		case ERROR_REGISTER:
			printf("The two passwords do not match\n");
			login_opt(&info);
		case SUCCESS_LOGIN:
			printf("login success\n");
			break;
		case SUCCESS_REGISTER:
			printf("register success, your id is %d\n", info.id);
			break;
		case ERROR_ANSWER:
			printf("answer error\n");
			login_opt(&info);
			break;
		case RIGHT_ANSWER:
			do
			{
				printf("please enter password:");
				scanf("%s", password_buf1);
				printf("please enter password again:");
				scanf("%s", password_buf2);
				if(strcmp(password_buf2, password_buf1) != 0)
				{
					printf("The two passwords do not match\n");
				}
				else
				{
					strcpy(info.password, password_buf1);
					send(info.cfd, &info, sizeof(info), 0);
					recv(info.cfd, &info, sizeof(info), 0);
				}
			}while(info.status != SUCCESS_FIND);
			printf("find password success");
			login_opt(&info);
			break;
		case ERROR_NUM:
			printf("请输入正确序号！\n");
			login_opt(&info);
			break;
	}
}

void opt1(per_info *Info)
{
	per_info info = *Info;

	printf("-----------------------\n");
	printf("1.personal settings\n");
	printf("2.add friend\n");
	printf("3.view friends list\n");
	printf("4.chat with sb.\n");
	printf("5.view group list\n");
	printf("6.chat with group\n");
	printf("7.exit\n");

	scanf("%d", &info.choice);
	info.choice += 13;

	switch (info.choice)
	{
	case PERSONAL_SETTINGS:
		
		break;
	
	default:
		break;
	}
}

void per_set(per_info *Info)
{
	per_info info = *Info;

	do
	{
		printf("----------------------------------------\n");
		printf("1.view your infomation\n");
		printf("2.change name\n");
		printf("3.change password\n");
		printf("4.change question for finding password\n");
		printf("5.change answer for question\n");

		scanf("%d", &info.choice);
		info.choice += 20;

		switch (info.choice)
		{
			case VIEW_PER_INFO:
				info.status = VIEW_PER_INFO;
				send(info.cfd, &info.status, sizeof(info.status), 0);
				recv(info.cfd, &info, sizeof(info), 0);
				printf("id : %d\n", info.id);
				printf("name : %s\n", info.name);
				printf("question for finding password: %s\n", info.question);
				printf("are you return? (y/n):");
				getchar();
				break;
			case CHANGE_NAME:
				printf("please enter your new name:");
				scanf("%s", info.name);
				send(info.cfd, info.name, sizeof(info.name), 0);
				recv(info.cfd, &info.status, sizeof(info.status), 0);
				if(info.status == SUCCESS_CH_NA)
					printf("change name success\n");
				else
					printf("change name failed\n");
				break;
			case CHANGE_PASSWORD:
				break;
		}
	}while(info.choice != EXITT);
}