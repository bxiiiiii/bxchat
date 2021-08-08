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

#define SERV_IP "192.168.2.173"

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
int kk;
int tt;

int main()
{
    pthread_t tid;
	int cfd, choice1, ret;
	struct sockaddr_in s_addr;
	List_Init(plist, pack_node_t);
	Pack basepack;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERV_IP, &s_addr.sin_addr.s_addr);
	
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	connect(cfd, (struct sockaddr *)&s_addr, sizeof(s_addr));

    

	basepack.data.sfd = cfd;
	pthread_create(&tid, NULL, Recv_pthr, (void *)&basepack);
	Login_opt(&basepack);
	if(basepack.choice == EXIT1)
		return 0;

	Check(basepack);
	do
	{
		Opt1(&basepack);
		if(basepack.choice == EXIT2)
			{
				Login_opt(&basepack);
				Check(basepack);
			}
		if(basepack.choice == EXIT3)
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
	int ret, sum, fd;
	char buf[1024];
	Pack *pack = (Pack *)arg;
	int i = 0;

	Pack tem;
	pack_list_t newNode;

    while(1)
    {
		//pthread_mutex_lock(&mutex);
		tmp = *pack;
		plist->data.num = 0;
		newNode = (pack_list_t)malloc(sizeof(pack_node_t));
        int n = recv(pack->data.sfd, &tem, sizeof(Pack), 0);
		if(n == 0)
			exit(0);
		newNode->data = tem;

		//pthread_mutex_lock(&mutex); 
		
		/*printf("recv:%d\n", n);
		printf("id : %d\n", tem.info.id);*/
		switch (tem.status)
		{
			case fri:
				printf("\n[There's a new friend request.]\n");
				*pack = tmp;
				//fflush(stdin);
				break;
			
			case sb:
				*pack = tmp;
				//printf("%d* %d %d\n",pack->choice, tmp2.info.id,ccid);
				
				if(tmp2.info.id == ccid)
				{
					printf("[%s %d-%d %d:%d:%d]\n", tmp2.fmnode.name1, tmp2.fmnode.date.month, tmp2.fmnode.date.day,
																	tmp2.fmnode.time.hour, tmp2.fmnode.time.minute, tmp2.fmnode.time.second);
					printf("%s\n", tmp2.data.sendbuf);
					//Friend_msg(pack);
				}
				else
					printf("\n[There's a new friend message of %s.]\n", tmp2.info.name);
				//fflush(stdin);
				//printf("**\n");
				break;

			case mem:
				*pack = tmp;
				//printf("%d* %d %d\n",pack->choice, ttid,ccid);
				if(tmp2.data.cid == ggid)
				{
					printf("[%s(%d) %d-%d %d:%d:%d]:", tmp2.info.name, tmp2.fmnode.id, tmp2.fmnode.date.month, tmp2.fmnode.date.day,
																	tmp2.fmnode.time.hour, tmp2.fmnode.time.minute, tmp2.fmnode.time.second);
					printf("%s\n", tmp2.data.sendbuf);
					//Group_msg(pack);
				}
				else
					printf("\n[There's a new group message of %s.]\n", tmp2.fmnode.name1);
				//fflush(stdin);
				break;			

			case gro:
				printf("\n[There's a new group notice.]\n");
				*pack = tmp;
				//fflush(stdin);
				break;

			case FILE_recv:
				//printf("**\n");
				fd = open(pack->finode.file_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
				sum = 0;
				while(1)
				{
					
					if(sum >= pack->finode.file_size)
					{
						printf("接收成功\n");
						pack->status = FILE_send;
						break;
					}
					bzero(buf, 1024);
					//printf("**\n");
					ret = recv(pack->data.sfd, buf, sizeof(buf), 0);
					write(fd, buf, ret);
					sum+=ret;
					//		printf("%d %d\n", ret, pack->finode.file_size);
				}
				close(fd);
				break;

			default:
			//	while(kk == 0)
			//		pthread_cond_signal(&cond);
				pthread_cond_signal(&cond);
				pthread_mutex_lock(&mutex);
				//printf("*%d\n", List_IsEmpty(plist));
				List_AddTail(plist, newNode);
				//printf(" : %d\n", newNode->data.num);
				plist->data.num++;
				//printf("*%d\n", List_IsEmpty(plist));
				pthread_mutex_unlock(&mutex);
				break;
		}
    }
}

void Recv_func(Pack *pack)
{
	pthread_mutex_lock(&mutex);
	while(List_IsEmpty(plist))
		pthread_cond_wait(&cond, &mutex);
	*pack = plist->next->data;
	plist->next->next->prev = plist;
	plist->next = plist->next->next;
	plist->data.num--;
	pthread_mutex_unlock(&mutex);
}


void Login_opt(Pack *basepack)
{
	//Pack* pack = (Pack *)malloc(sizeof(Pack));
	struct termios oldt,newt;
	tcgetattr(0,&oldt);
	newt = oldt;
	newt.c_lflag &= ~(ECHO|ICANON);
	char ch;
	int i= 0;
	char password_buf1[16];
	char password_buf2[16];

	while(1)
	{
		system("clear");
		//printf("-------------------\n");
		printf("\n\n\n\n\n\n");
		printf("\t\t\t\tbxchat\n");
		printf("\n\n");
		printf("\t\t\t[1]登陆\n");
		printf("\t\t\t[2]注册\n");
		printf("\t\t\t[3]找回密码\n");
		printf("\t\t\t[4]退出\n");
		//printf("-------------------\n");
		printf("\n");
		printf("\t\t\t输入你的选择: ");
		scanf("%d", &basepack->choice);

		//printf("**%d\n", pack->choice);
		switch(basepack->choice)
		{
			case LOGIN:
				//
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\tlogin in\n");
				printf("\n\n");
				printf("\t\t\t账号：");
				scanf("%d", &basepack->info.id);
				printf("\t\t\t密码：");
				/*i = 0;
				while(1)
				{
				setbuf(stdin, NULL);
				tcsetattr(0,TCSANOW,&newt);
				scanf("%c",&ch);
				tcsetattr(0,TCSANOW,&oldt);
				if(i == 16 || ch == '\n')
				break;
				basepack->info.password[i] = ch;
				printf("*");
				i++;
				}*/
				scanf("%s", basepack->info.password);
				send(basepack->data.sfd, basepack, sizeof(Pack), 0);
				Recv_func(basepack);
					
				break;

			case REGISTER:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\tlogin up\n");
				printf("\n\n");
				printf("\t\t\t请输入名字:");
				scanf("%s", basepack->info.name);
				printf("\t\t\t请输入密码:");
				i = 0;
				while(1)
				{
				setbuf(stdin, NULL);
				tcsetattr(0,TCSANOW,&newt);
				scanf("%c",&ch);
				tcsetattr(0,TCSANOW,&oldt);
				if(i == 16 || ch == '\n')
				break;
				password_buf1[i] = ch;
				printf("*");
				i++;
				}
				//scanf("%s", password_buf1);
				printf("\t\t\t请再次输入密码:");
				i = 0;
				while(1)
				{
				setbuf(stdin, NULL);
				tcsetattr(0,TCSANOW,&newt);
				scanf("%c",&ch);
				tcsetattr(0,TCSANOW,&oldt);
				if(i == 16 || ch == '\n')
				break;
				password_buf2[i] = ch;
				printf("*");
				i++;
				}
				//scanf("%s", password_buf2);	
				printf("\t\t\t设置密保:");
				scanf("%s", basepack->info.question);	
				printf("\t\t\t请输入答案:");
				scanf("%s", basepack->info.answer);

				if(strcmp(password_buf2, password_buf1) != 0)
					basepack->status = -3;
				else
				{
					strcpy(basepack->info.password, password_buf1);
					send(basepack->data.sfd, basepack, sizeof(Pack), 0);
					Recv_func(basepack);
				}
			//	printf("%d*\n", pack->status);
				break;

			case FIND_PASSWORD:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\t找回密码\n\n\n");
				printf("\t\t\t请输入待找回的账号:");
				scanf("%d", &basepack->info.id);
				send(basepack->data.sfd, basepack, sizeof(Pack), 0);

				Recv_func(basepack);
				*basepack = plist->data;
				if(basepack->status == -1)
					break;
				
				printf("%s: ",basepack->info.question);
				scanf("%s", basepack->info.answer);
				
				basepack->choice = FIND_ANSWER;
				send(basepack->data.sfd, basepack, sizeof(Pack), 0);
				
				Recv_func(basepack);
				break;

			case EXIT1:
				send(basepack->data.sfd, basepack, sizeof(Pack), 0);
				exit(0);

			default:
				basepack->status = -4;
				break;
		}
		//kk=1;

		//printf("***%d\n", pack->status);
		switch(basepack->status)
		{
			case -1:
				printf("该账号不存在\n");
				break;
			case -2:
				printf("密码错误\n");
				break;
			case -3:
				printf("两次密码不一致\n");
				break;
			case 2:
				printf("登陆成功\n");
				printf("输入任意字符继续...\n");
				setbuf(stdin, NULL);
				getchar();
				return ;

			case 0:
				printf("注册成功, 你的账号为%d\n", basepack->info.id);
				break;
			case -5:
				printf("答案错误\n");
				break;
			case 1:
				do
				{
					printf("\t\t\t请输入新的密码:");
					scanf("%s", password_buf1);
					printf("\t\t\t请再次输入密码:");
					scanf("%s", password_buf2);
					if(strcmp(password_buf2, password_buf1) != 0)
					{
						printf("两次密码不一致\n");
					}
					else
					{
						strcpy(basepack->info.password, password_buf1);
						basepack->choice = GET_NEW_PW;
                        send(basepack->data.sfd, basepack, sizeof(Pack), 0);

						Recv_func(basepack);
					}
				}while(basepack->status != 7);
				printf("找回密码成功\n");
				break;
			case -4:
				printf("请输入正确选择\n");
				break;
		}
		printf("输入任意字符继续...\n");
		setbuf(stdin, NULL);
		getchar();
	//printf("***%d\n", pack->status);
	}
}

void Check(Pack basepack)
{
	int i;
	int j = 0;
	Pack* pack = (Pack *)malloc(sizeof(Pack));
	*pack = basepack;
	pack->choice = CHECK;
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	Recv_func(pack);
	if(pack->num)
	{
		printf("[There's a new friend request.]\n");
		j++;
	}

	Recv_func(pack);
	for(i = 0; i < pack->num; i++)
	{
	    Recv_func(pack);
		printf("[There's a new friend message of %s(%d).]\n", pack->fnode.name, pack->fnode.id);
		j++;
	}
	
	Recv_func(pack);
	if(pack->num)
	{
		printf("[There's a new group notice.]\n");
		kk=1;
		j++;
	}

	Recv_func(pack);
	//printf("%d %d\n", pack->info.id, pack->info.choice);
	//printf("%d\n", pack->num);
	for(i = 0; i < pack->num; i++)
	{
	    Recv_func(pack);
		printf("[There's a new group message of %s(%d).]\n", pack->fnode.name, pack->fnode.id);
	}
	if(j)
	{
		printf("输入任意字符继续...\n");
		setbuf(stdin, NULL);
		getchar();
	}
	free(pack);
}

void Opt1(Pack *basepack)
{
	while(1)
	{
		system("clear");
		//printf("%d %d\n", pack->info.id, pack->info.choice);
		printf("\n\n\n\n\n\n");
		printf("\t\t\t\t%s\n\n\n", basepack->info.name);
		printf("\t\t\t[1]个人设置\n");
		printf("\t\t\t[2]好友\n");
		printf("\t\t\t[3]群聊\n");
		printf("\t\t\t[4]退出登陆\n");
		printf("\n");
		printf("\t\t\t请输入你的选择:");

		scanf("%d", &basepack->choice);
		basepack->choice += 9;
		//printf("%d*\n", pack->choice);
		switch (basepack->choice)
		{
			case PER_set:
				Per_set(*basepack);
				break;

			case FRIEND:
				Opt4(*basepack);
				break;
			
			case GROUP:
				Opt2(basepack);
				break;

			case EXIT2:
				send(basepack->data.sfd, &basepack, sizeof(Pack), 0);
				return ;
			
			default:
				printf("请输入正确选择\n");
				break;
		}
	}
}

void Per_set(Pack basepack)
{
	Pack* pack = (Pack *)malloc(sizeof(Pack));
	*pack = basepack;
	char password_buf1[16];
	char password_buf2[16];
	
	while(1)
	{
		system("clear");
		printf("\n\n\n\n\n\n");
		printf("\t\t\t[1]查看个人信息\n");
		printf("\t\t\t[2]更改名字\n");
		printf("\t\t\t[3]更改密码\n");
		printf("\t\t\t[4]更改密保\n");
		printf("\t\t\t[5]更改答案\n");
		printf("\t\t\t[6]返回\n");
		printf("\n");
		printf("\t\t\t请输入你的选择:");

		scanf("%d", &pack->choice);
		pack->choice +=19;
		switch (pack->choice)
		{
			case VIEW_perinfo:
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				Recv_func(pack);
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\t个人信息\n\n");
				printf("\t\t\t名字:%s\n", pack->info.name);
				printf("\t\t\t密保:%s\n", pack->info.question);
				pack->status = 10;
				break;

			case CHANGE_name:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\t修改名字\n\n");
				printf("\t\t\t请输入新名字:");
				scanf("%s", pack->info.name);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				Recv_func(pack);
				break;

			case CHANGE_password:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\t修改密码\n\n");
				printf("\t\t\t请输入新密码:");
				scanf("%s", password_buf1);
				printf("\t\t\t请再次输入新密码：");
				scanf("%s", password_buf2);
				if(strcmp(password_buf2, password_buf1) != 0)
					pack->status = -1;
				else
				{
					strcpy(pack->info.password, password_buf1);
					pack->choice = GET_NEW_PW;
					send(pack->data.sfd, pack, sizeof(Pack), 0);
					Recv_func(pack);
				}
				break;

			case CHANGE_question:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\t修改密保\n\n");
				printf("\t\t\t请输入新的密保:");
				scanf("%s", pack->info.question);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				Recv_func(pack);
				break;

			case CHANGE_answer:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\t修改密保答案\n\n");
				printf("\t\t\t请输入新的密保答案:");
				scanf("%s", pack->info.answer);
				//printf("%s\n", pack->info.answer);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				Recv_func(pack);
				break;

			case EXIT3:
				return ;

			default:
				pack->info.status = -4;
				break;
		}

		switch (pack->status)
		{
			case 7:
				printf("更改密码成功\n");
				break;
			case 0:
				printf("更改名字成功\n");
				break;
			case -1:
				printf("两次输入的密码不一致\n");
				break;
			case 1:
				printf("更改密保成功\n");
				break;
			case 2:
				printf("更改密保答案成功\n");
				break;
			case -4:
				printf("请输入正确选择\n");
				break;
			default:
				break;
		}
		printf("输入任意字符返回...\n");
		setbuf(stdin, NULL);
		getchar();
	}
}

void Opt4(Pack basepack)
{
	while(1)
	{
		View_friendlist(basepack);

		printf("\n");
		printf("\t\t\t[1]添加好友\n");
		printf("\t\t\t[2]查看好友请求\n");
		printf("\t\t\t[3]选择好友\n");
		printf("\t\t\t[4]返回\n");
		printf("\n");
		printf("\t\t\t请输入你的选择:");

		scanf("%d", &basepack.choice);
		basepack.choice +=30;

		switch (basepack.choice)
		{
			case ADD_friend:
				Add_friend(basepack);
				break;

			case VIEW_friendrq:
				View_friendrq(basepack);
				break;
			
			case OPT5:
				Opt5(basepack);
				break;

			case EXIT4:
				return ;
			
			default:
				printf("请输入正确选择\n");
				break;
		}
		printf("输入任意字符返回...\n");
		setbuf(stdin, NULL);
		getchar();
	}
}

void View_friendlist(Pack basepack)
{
	Pack* pack = (Pack *)malloc(sizeof(Pack));
	*pack = basepack;
	int i;
	pack->choice = VIEW_friendlist;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	//system("clear");
	printf("\n\n");
	printf("\t\t\t\t好友列表\n\n");
	printf("%d*\n", pack->num);
	if(pack->num == 0)
	{
		printf("\t\t\t\t暂无记录\n\n");
	}
	else
	{
		//printf("--------------好友列表------------\n");
		printf("\t\tid   name                  status\n");
		printf("\t\t----------------------------------------\n");
		for(i = 0; i < pack->num; i++)
		{
			Recv_func(pack);
			printf("\t\t%-3d  %-20s  ", pack->fnode.id, pack->fnode.name);
			if(pack->fnode.status == 1)
				printf("在线\n");
			else
				printf("离线\n");
		}
	}
}

void Add_friend(Pack basepack)
{
	Pack *pack = (Pack *)malloc(sizeof(Pack));
	*pack = basepack;
    printf("\t\t\t请输入你要添加的用户账号:");
	scanf("%d", &pack->data.cid);
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

    switch (pack->status)
    {
        case -1:
            printf("该账号不存在\n");
            break;
        case -3:
            printf("该用户已是你的好友\n");
            break;
        case -2:
            printf("好友申请已发出，请勿重复申请\n");
            break;
        case 0:
            printf("好友申请已发出\n");
            break;
    }
	free(pack);
}

void View_friendrq(Pack basepack)
{
	int i, num;
	Pack *pack = (Pack *)malloc(sizeof(Pack));
	*pack = basepack;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	system("clear");
	printf("\n\n");
	printf("\t\t\t\t好友申请列表\n\n");
	if(pack->status == 0)
	{
		printf("\t\t\t\t  暂无记录\n\n");
	}
	else
	{
		num = pack->num;
		printf("\t\t\t  ID   Name\n");
		for(i = 0; i < num; i++)
		{
			Recv_func(pack);
			printf("\t\t\t%-3d  %-20s  \n", pack->fnode.id, pack->fnode.name);
		}
		Process_friendrq(basepack);
	}
	free(pack);
}

void Process_friendrq(Pack basepack)
{
	Pack *pack = (Pack *)malloc(sizeof(Pack));
	*pack = basepack;
	pack->choice = PROCESS_frq;
	while(1)
	{
		printf("\n");
		printf("\t\t\t请输入待处理的账号或输入[0]返回:");
		scanf("%d", &pack->data.cid);
		if(pack->data.cid == 0)
			break;
		printf("\t\t\t[1]同意    [2]不同意    [3]返回\n");
		printf("\t\t\t请输入你的选择: ");
		scanf("%d", &pack->status);
		if(pack->status < 1  && pack->status > 2)
			break;
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		printf("处理成功\n");

		printf("[1]继续处理    [2]返回\n");
		scanf("%d", &pack->status);
		if(pack->status != 1)
			break;
	}
	free(pack);
}

void Opt5(Pack basepack)
{
	Pack *pack = (Pack *)malloc(sizeof(Pack));
	*pack = basepack;
	printf("\t\t\t请输入好友账号:");
	scanf("%d", &pack->data.cid);

	switch (Is_friend(pack))
	{
		case -1:
			printf("该账号不存在\n");
			return ;
		case -2:
			printf("该用户不是你的好友\n");
			return;
		case 1:
			break;
		default:
			return ;
	}
	
	while(1)
	{
		system("clear");
		printf("\n\n\n\n\n\n");
		printf("\t\t\t[1]查看好友信息\n");
		printf("\t\t\t[2]聊天\n");
		printf("\t\t\t[3]传文件\n");
		printf("\t\t\t[4]收文件\n");
		printf("\t\t\t[5]屏蔽好友\n");
		printf("\t\t\t[6]删除好友\n");
		printf("\t\t\t[7]返回\n");
		//printf("----------------------------------------\n");
		printf("\n");
		printf("\t\t\t请输入你的选择:");

		scanf("%d", &pack->choice);
		pack->choice += 39;
		switch (pack->choice)
		{
			case VIEW_friendinfo:
				View_friendinfo(pack);
				break;

			case FRIEND_chat:
				Friend_chat(pack);
				break;

			case FILE_send:
				Send_file(pack);
				break;

			case FILE_recv:
				Recv_file(pack);

				break;
			case SHLELD_friend:
				Shield_friend(pack);
				break;

			case DELETE_friend:
				Delete_friend(pack);
				break;

			case EXIT5:
				return ;

			default:
				printf("请输入正确选择\n");
				break;
		}
		printf("输入任意字符返回...\n");
		setbuf(stdin, NULL);
		getchar();
	}
}

void View_friendinfo(Pack *pack)
{
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	system("clear");
	printf("\n\n\n\n\n\n");
	printf("\t\t\t\t好友信息\n\n");
	printf("\t\t\t账号:%d\n", pack->fnode.id);
	printf("\t\t\t名字:%s\n", pack->fnode.name);
	printf("\t\t\t状态:");
	switch(pack->fnode.status)
	{
		case 1:
			printf("在线\n");
			break;
		case 0:
			printf("离线\n");
			break;
		default:
			printf("其他\n");
	}
}

void Friend_msg(Pack *pack)
{
	pack->choice = GET_friendmsg;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	system("clear");
	printf("\t\t\t\t%s\n\n", pack->fnode.name);
	if(pack->num == 0)
	{
		printf("\t\t\t\t暂无记录\n");
	}
	else
	{
		for(int i = 0; i < pack->num; i++)
		{
			//printf("**%d\n", i);
			//	    printf("%d**\n", pack->num);
			Recv_func(pack);
			//			    printf("**\n");
			if(strcmp(pack->fmnode.name1, pack->info.name))
			{
				printf("%s %02d-%02d %02d:%02d:%02d\n", pack->fmnode.name1, pack->fmnode.date.month, pack->fmnode.date.day,
																	pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
				printf("%s\n", pack->fmnode.msgbuf);
			}
			else
			{
				printf("%65s %02d-%02d %02d:%02d:%02d\n", pack->fmnode.name1, pack->fmnode.date.month, pack->fmnode.date.day,
																	pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
				printf("%80s\n", pack->fmnode.msgbuf);
			}
		}
	}
	//printf("***\n");
}

void Friend_chat(Pack *pack)
{
	/*printf("请输入好友账号: ");
	scanf("%d", &pack->data.cid);*/
	
	switch (Is_friend(pack))
	{
		case -1:
			printf("该账号不存在\n");
			return ;
		case -2:
			printf("该用户不是你的好友\n");
			return;
		case 1:
			break;
		default:
			printf("未知错误\n");
			return ;
	}

	
	ccid = pack->data.cid;
	Friend_msg(pack);

	if(Is_shield(pack) == 1)
	{
		printf("已屏蔽该好友\n");
		return ;
	}
	while(1)
	{
		//printf("Enter your msg or enter 'y' to exit: ");
		memset(pack->data.sendbuf, 0, sizeof(pack->data.sendbuf));
		setbuf(stdin, 0);
		fgets(pack->data.sendbuf, sizeof(pack->data.sendbuf), stdin);
		pack->data.sendbuf[strlen(pack->data.sendbuf)-1] = '\0';
		//scanf("%s", pack->data.sendbuf);

		switch (Is_friend(pack))
		{
			case -1:
				printf("该账号不存在\n");
				return ;
			case -2:
				printf("该用户不是你的好友\n");
				return;
			case 1:
				break;
			default:
				printf("未知错误\n");
				return ;
		}

		if(Shield(pack) == 1)
			pack->status = -99;
		else 
			pack->status == 0;

		if(!(strcmp(pack->data.sendbuf, "exit")))
		{
			ccid = 0;
			return ;
		}
		pack->fmnode.date = DateNow();
		pack->fmnode.time = TimeNow();
		pack->choice  = FRIEND_chat;
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		//printf("                                                    [%s %d-%d %d:%d:%d]\n", pack->info.name, pack->fmnode.date.month, pack->fmnode.date.day,
		//															pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
		//printf("%-s\n",pack->data.sendbuf);
		Friend_msg(pack);
	}
}
int Is_friend(Pack *pack)
{
	pack->choice = IS_friend;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	return pack->status;
}

void Send_file(Pack *pack)
{	
	char file_path[128] = {0};//文件路径
	char file_info[2048] = {0};//文件信息
	char buf[1024] = {0};

	switch (Is_friend(pack))
	{
		case -1:
			printf("该账号不存在\n");
			return ;
		case -2:
			printf("该用户不是你的好友\n");
			return;
		case 1:
			break;
		default:
			printf("未知错误\n");
			return ;
	}

	if(Is_shield(pack) == 1)
	{
		printf("已屏蔽该好友\n");
		return ;
	}
	
	//获取用户输入的文件路径
	printf("\t\t\t请输入文件的绝对路径/0返回:");
	scanf("%s", file_path);

	strncpy(pack->finode.file_name, basename(file_path), sizeof(file_path));
	
	//打开文件
	int fd = open(file_path, O_RDWR);
	if (fd == -1)
	{
		printf("打开文件%s失败\n", file_path);
		return ;
	}	

	//计算文件大小
	pack->finode.file_size = lseek(fd, 0, SEEK_END);
 
	lseek(fd, 0, SEEK_SET);
	
	if(Shield(pack) == 1)
		pack->status = -99;
	else 
		pack->status == 0;

	pack->choice = FILE_info;
	pack->fmnode.date = DateNow();
	pack->fmnode.time = TimeNow();
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	int send_len = 0;//记录发送了多少字节
	
	while (1)
	{	
		bzero(buf, 1024);
		//读取数据
		int ret = read(fd, buf, sizeof(buf));
		printf("发送中...\n");
		//printf("%d\n", ret);
		if (ret <= 0)
		{
			printf("发送文件成功\n");
			break;
		}
			
		//发送数据
		//pack->choice = FILE_send;
		//printf("%d %d\n", pack->choice, pack->info.id);
		send(pack->data.sfd, buf, ret, 0);
		
		send_len += ret;//统计发送了多少字节
		
		//上传文件的百分比 
		//printf("*%d\n", send_len);
	}
	//pack->choice = 0;
	//printf("--choice = %d, id = %d, name = %s, serfd = %d, sfd = %d\n", pack->choice, pack->info.id, pack->info.name, pack->data.serfd, pack->data.sfd);
	//send(pack->data.sfd, pack, sizeof(Pack), 0);
	//printf("%d\n", send_len);
	// 关闭文件 
	close(fd);
}

void Recv_file(Pack *pack)
{
	View_filelist(pack);

	printf("\n\t\t请输入文件名/0返回:");
	scanf("%s", pack->finode.file_name);
		/*if(atoi(pack->finode.buf) == 0)
			return ;*/
	printf("\t\t[1]接收   [2]返回\n");
	printf("\t\t请输入你的选择:");
	scanf("%d", &pack->choice);
	switch (pack->choice)
	{
			case 2:
			return ;
			case 1:
				pack->choice = FILE_recv;
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				/*if(pack->status == -1)
				{
					printf("文件名错误\n");
					break;
				}*/

				printf("接收中...\n");
				while(1)
				{
					if(pack->status == FILE_send)
						break;
				}
				break;
	}
}

void View_filelist(Pack *pack)
{
	int i;
	pack->choice = VIEW_filelist;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	system("clear");
	printf("\n\n");
	printf("\t\t\t\t文件列表\n\n");
	if(pack->num == 0)
	{
		printf("\t\t\t\t暂无记录\n\n");
	}
	else
	{
		printf("\t\t  大小          Name                  状态\n");
		printf("\t\t-------------------------------------------\n");
		for(i = 0; i < pack->num; i++)
		{
			Recv_func(pack);
			printf("\t\t  %-12d  %-20s  ", pack->finode.file_size, pack->finode.file_name);
			switch (pack->finode.num)
			{
				case 1:
					printf("未接收\n");
					break;
				case 2:
					printf("已接收\n");
					break;
				default:
					printf("其他\n");
			}
		}
	}
}

void Shield_friend(Pack *pack)
{
	switch (Is_friend(pack))
	{
		case -1:
			printf("该账号不存在\n");
			return ;
		case -2:
			printf("该用户不是你的好友\n");
			return;
		case 1:
			break;
		default:
			printf("未知错误\n");
			return ;
	}

	switch (Is_shield(pack))
	{
		case 0:
			while(1)
			{
				printf("\t\t\t是否屏蔽该好友(1/0):");
				scanf("%d", &pack->choice);
				switch (pack->choice)
				{
					case 1:
						pack->choice = SHLELD_friend;
						send(pack->data.sfd, pack, sizeof(Pack), 0);
						printf("设置成功\n");
						return ;
					case 0:	
						return ;
					default:
						printf("请输入正确选择\n");
				}
			}
			break;
		case 1:
			while(1)
			{
				printf("\t\t\t已屏蔽该好友\n");
				printf("\t\t\t是否解除屏蔽(1/0):");
				scanf("%d", &pack->choice);
				switch (pack->choice)
				{
					case 1:
						pack->choice = CANCEL_shield;
						send(pack->data.sfd, pack, sizeof(Pack), 0);
						printf("设置成功\n");
					case 0:	
						return ;
					default:
						printf("请输入正确选择\n");
				}
			}
			break;
		default:
			printf("未知错误\n");
	}
}

int Is_shield(Pack *pack)
{
	pack->choice = IS_shield;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	return pack->status;
}

int Shield(Pack *pack)
{
	pack->choice = SHIELD;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	return pack->status;
}

void Delete_friend(Pack *pack)
{
	printf("\t\t\t请输入待删除的好友账号:");
	scanf("%d", &pack->data.cid);

	switch (Is_friend(pack))
	{
		case -1:
			printf("该账号不存在\n");
			return ;
		case -2:
			printf("该用户不是你的好友\n");
			return;
		case 1:
			break;
		default:
			printf("未知错误\n");
			return ;
	}

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	printf("删除成功\n");
}

void Opt2(Pack *pack)
{
	while(1)
	{
		View_grouplist(pack);

		printf("\t\t\t[1]建群\n");
		printf("\t\t\t[2]查看群通知\n");
		printf("\t\t\t[3]添加群聊\n");
		printf("\t\t\t[4]进入群聊\n");
		printf("\t\t\t[5]返回\n");
		printf("\n");
		printf("\t\t\t请输入你的选择:");

		scanf("%d", &pack->choice);
		pack->choice += 54;
		//printf("%d***\n", pack->choice);
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

			case OPT3:
				Opt3(pack);
				break;

			case EXIT6:
				return;

			default:
				printf("请输入正确选择\n");
				break;
		}
		printf("输入任意字符返回...\n");
		setbuf(stdin, NULL);
		getchar();
	}
}

void View_grouplist(Pack *pack)
{
	int i;

	pack->choice = VIEW_grouplist;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	//   printf("%d*\n", pack->num);
	system("clear");
	printf("\n\n");
	printf("\t\t\t\t群聊列表\n\n");
	printf("\t\t  id   name                  status\n");
	printf("\t\t  -----------------------------------\n");
	if(pack->num == 0)
	{
		printf("\t\t\t暂无记录\n");
	}
	else
	{
		for(i = 0; i < pack->num; i++)
		{
			Recv_func(pack);
			printf("\t\t  %-3d  %-20s  ", pack->fnode.id, pack->fnode.name);
			switch (pack->fnode.status)
			{
				case 1:
					printf("成员\n");
					break;
				case 2:
					printf("管理员\n");
					break;
				case 9:
					printf("群主\n");
					break;
				default:
					printf("其他\n");
					break;
			}
		}
	}
	printf("\n");
}

void Set_group(Pack *pack)
{
	printf("\t\t\t请输入群聊名称:");
	scanf("%s", pack->fnode.name);
	pack->choice = SET_group;
	pack->fmnode.date = DateNow();
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	printf("创建成功\n");
}

void Add_group(Pack *pack)
{
    printf("\t\t\t输入待加入群id:");
	scanf("%d", &pack->data.cid);
	pack->choice = ADD_group;
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	Recv_func(pack);

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
		
		default:
			printf("未知错误\n");
			break;
    }
}

void View_grouprq(Pack *pack)
{
	int i;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	system("clear");
	printf("\n\n");
	printf("                                    群通知\n\n");
	printf("     Group                      User \n");
	printf("ID   Name                  ID   Name                  状态               类型\n");
	printf("--------------------------------------------------------------------------------\n");
	if(pack->num == 0)
	{
		printf("\t\t\t\tNo record.\n");
	}
	else
	{
		for(i = 0; i < pack->num; i++)
		{
			
			Recv_func(pack);
			//	printf("%d*%d\n", pack->num, kk);
			printf("%-3d  %-20s  %-3d  %-20s  ", pack->fmnode.id, pack->fmnode.name1, pack->fmnode.time.hour, pack->fmnode.name2);
			switch (pack->fmnode.time.minute)
			{
				case 9:
					printf("已成为群主         ");
					break;
				case 2:
					printf("已成为管理员       ");
					break;
				case 0:
					printf("待处理            ");
					break;
				case 1:
					printf("已同意            ");
					break;
				case 11:
					printf("其他管理员已同意  ");
					break;
				case 3:
					printf("已拒绝            ");
					break;
				case 12:
					printf("其他管理员已拒绝  ");
					break;
				case 4:
					printf("被移出群聊        ");
					break;
				case -1:
					printf("群已被解散        ");
					break;
				default:
					printf("其他             ");
					break;
			}
			switch (pack->fmnode.time.second)
			{

				case 1:
					printf("群通知\n");
					break;
				case 2:
					printf("群申请\n");
					break;
				default:
					printf("其他\n");
					break;
			}
		}
		Process_grouprq(pack);
	}
}

void Process_grouprq(Pack *pack)
{
	pack->choice = PROCESS_grq;
	while(1)
	{

		printf("\n\t\t请输入待处理的群聊账号/[0]返回:");
		scanf("%d", &pack->data.cid);
		if(pack->data.cid == 0)
			return ;
		printf("\t\t请输入待处理的用户账号/[0]返回:");
		scanf("%d", &pack->fnode.id);
		if(pack->data.cid == 0)
			return ;
		printf("\t\t[1]同意   [2]不同意  [3]返回\n");
		printf("\t\t请输入你的选择: ");
		scanf("%d", &pack->status);
		if(pack->status == 3)
			return ;
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		printf("处理成功\n");

		printf("[1]继续处理    [2]返回\n");
		scanf("%d", &pack->status);
		if(pack->status != 1)
			break;
	}
}

void Opt3(Pack *pack)
{
	printf("\t\t\t输入要进入的群聊账号:");
	scanf("%d", &pack->data.cid);
	
	Get_status(pack);
	//printf("%d*\n", pack->status);
	system("clear");
	printf("\n\n\n\n\n\n");
	switch (pack->status)
	{
		case 1:
		while(1)
			{
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t[1]聊天\n");
				printf("\t\t\t[2]查看群\n");
				printf("\t\t\t[3]退群\n");
				printf("\t\t\t[4]返回\n\n");
				printf("\t\t\t请输入你的选择:\n");

				scanf("%d", &pack->choice);
				pack->choice += 64;
				switch (pack->choice)
				{
					case EXIT_group:
						Exit_group(pack);
						ggid = 0;
						return ;
					case EXIT7:
						ggid = 0;
						return ;
					case VIEW_groupinfo:
						View_groupinfo(pack);
						break;
					case GROUP_chat:
						ggid = pack->data.cid;
						Group_chat(pack);
						break;
					default:
						printf("请输入正确选择\n");
						break;

				}
				printf("输入任意字符返回...\n");
				setbuf(stdin, NULL);
				getchar();
			}
			break;

		case 2:
			while(1)
			{
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t[1]聊天\n");
				printf("\t\t\t[2]查看群\n");
				printf("\t\t\t[3]退群\n");
				printf("\t\t\t[4]返回\n");
				printf("\t\t\t[5]踢人\n");
				printf("\n\t\t\t请输入你的选择:");

				scanf("%d", &pack->choice);
				pack->choice += 64;
				switch (pack->choice)
				{
					case EXIT_group:
						Exit_group(pack);
						ggid = 0;
						return ;
					case EXIT7:
						ggid = 0;
						return ;
					case REMOVE_member:
						Remove_member(pack);
						break;
					case VIEW_groupinfo:
						View_groupinfo(pack);
						break;
					case GROUP_chat:
						Group_chat(pack);
						break;
					default:
						printf("请输入正确选择\n");
						break;
				}
				printf("输入任意字符返回...\n");
				setbuf(stdin, NULL);
				getchar();
			}
				break;
		
		case 9:
			while(1)
			{
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t[1]聊天\n");
				printf("\t\t\t[2]查看群\n");
				printf("\t\t\t[3]退群\n");
				printf("\t\t\t[4]返回\n");
				printf("\t\t\t[5]踢人\n");
				printf("\t\t\t[6]设置管理员\n");
				printf("\t\t\t[7]转让群\n");
				printf("\t\t\t[8]解散群\n");
				printf("\t\t\t[9]撤销管理员\n\n");
				printf("\t\t\t请输入你的选择:");

				scanf("%d", &pack->choice);
				pack->choice += 64;
				switch (pack->choice)
				{
					case DISSOLVE_group:
						Dissolve_group(pack);
						return ;
					case TRANSFER_group:
						Transfer_group(pack);
						return ;
					case SET_admini:
						Set_admini(pack);
						break;
					case EXIT_group:
						printf("群主不能直接退群，可以转让群再退群或解散群\n");
						break;
					case EXIT7:
						return ;
					case REMOVE_member:
						Remove_member(pack);
						break;
					case VIEW_groupinfo:
						View_groupinfo(pack);
						break;
					case GROUP_chat:
						Group_chat(pack);
						break;
					case CANCEL_admini:
						Cancel_admini(pack);
					default:
						printf("请输入正确选择\n");
						break;
				}
				printf("输入任意字符返回...\n");
				setbuf(stdin, NULL);
				getchar();
			}
			break;
			default:
				break;
	}
}

void Group_msg(Pack *pack)
{
	pack->choice = GET_groupmsg;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	//printf("**%d\n", pack->num);
	system("clear");
	printf("\t\t\t\t%s\n", pack->fnode.name);
	if(pack->num == 0)
	{
		printf("\t\t\t暂无记录\n");
	}
	else
	{
		for(int i = 0; i < pack->num; i++)
		{
			//printf("**%d\n", i);
			//	    printf("%d**\n", pack->num);
			Recv_func(pack);
			//			    printf("**\n");
			printf("[%s(%d) %d-%d %d:%d:%d]:", pack->fmnode.name1, pack->fmnode.id, pack->fmnode.date.month, pack->fmnode.date.day,
																pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
			printf("%s\n", pack->fmnode.msgbuf);

		}
	}
}

void Group_chat(Pack *pack)
{
	ggid = pack->data.cid;
	Group_msg(pack);

	while(1)
	{
		memset(pack->data.sendbuf, 0, sizeof(pack->data.sendbuf));
		setbuf(stdin, 0);
		fgets(pack->data.sendbuf, sizeof(pack->data.sendbuf), stdin);
		pack->data.sendbuf[strlen(pack->data.sendbuf)-1] = '\0';
		if(!(strcmp(pack->data.sendbuf, "exit")))
		{
			ggid = 0;
			return ;
		}
		pack->fmnode.date = DateNow();
		pack->fmnode.time = TimeNow();
		pack->choice  = GROUP_chat;
		send(pack->data.sfd, pack, sizeof(Pack), 0);

		Group_msg(pack);
	}
}

void View_groupinfo(Pack *pack)
{
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	//printf("---------%s---------\n", pack->fmnode.name1);
	system("clear");
	printf("\n\t\t\t\t%s\n", pack->fmnode.name1);
	printf("\t\t群id:%d\n", pack->data.cid);
	printf("\t\t群名:%s\n", pack->fmnode.name1);
	printf("\t\t群主id:%d\n", pack->fmnode.id);
	printf("\t\t群主:%s\n", pack->fmnode.name2);
	printf("\t\t群人数:%d\n", pack->num);
	printf("\t\t创建日期:%d-%d-%d\n", pack->fmnode.date.year, pack->fmnode.date.month, pack->fmnode.date.day);

	Recv_func(pack);

	printf("\t\t\t\t群成员\n");
	printf("\t\tID   Name                  类型\n");
	printf("\t\t----------------------------------\n");
	if(pack->num == 0)
	{
		printf("\t\t\tNo record.\n");
	}
	else
	{
		for(int i = 0; i < pack->num; i++)
		{
			Recv_func(pack);
			printf("\t\t%-3d  %-20s  ", pack->fnode.id, pack->fnode.name);
			switch (pack->fnode.status)
			{

				case 1:
					printf("成员\n");
					break;
				case 2:
					printf("管理员\n");
					break;
				case 9:
					printf("群主\n");
					break;
				default:
					printf("未知错误\n");
					break;
			}
		}
	}
}

void Remove_member(Pack *pack)
{
	printf("\t\t\t请输入你要踢出该群聊的账号:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	switch (pack->status)
	{
		case -2:
			printf("不是该群成员\n");
			break;
		case -3:
			printf("无权限\n");
			break;
		case 0:
			printf("操作成功\n");
			break;
		default:
			printf("未知错误\n");
			break;
	}
}

void Exit_group(Pack *pack)
{
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	printf("退群成功\n");
}

void Set_admini(Pack *pack)
{
	printf("\t\t\t输入需要设置的成员账号:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
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
		printf("\t\t\t确认解散群（y/n):");
		fflush(stdin);
		ch = getchar();
		switch(ch)
		{
			case 'n':
				return;
			case 'y':
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				printf("解散成功\n");
				printf("输入任意字符返回...\n");
				setbuf(stdin, NULL);
				getchar();
				return;
			default:
				break;
		}
	}

}

void Transfer_group(Pack *pack)
{
	printf("\t\t\t输入需要转让的成员账号:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	switch (pack->status)
	{
		case -1 :
			printf("该用户不存在\n");
			break;
		case -2:
			printf("该用户不是群成员\n");
			break;
		case -3:
			printf("你已经是群主\n");
			break;
		case 0:
			printf("转让成功\n");
			break;
	}
}

void Get_status(Pack *pack)
{
	pack->choice = GET_status;
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	Recv_func(pack);
}

void Cancel_admini(Pack *pack)
{
	printf("\t\t\t输入需要设置的管理员账号:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	switch (pack->status)
	{
		case -4 :
			printf("该id不存在\n");
			break;
		case -1:
			printf("不是该群成员\n");
			break;
		case -2:
			printf("该成员不是管理\n");
			break;
		case 0:
			printf("设置成功\n");
			break;
		default :
			printf("未知错误\n");
			break;
	}
}

int Is_group(Pack *pack)
{
	pack->choice = IS_group;
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

	return pack->status;	
}