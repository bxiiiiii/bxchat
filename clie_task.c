#include "clie_task.h"

void* Recv_pthr(void *arg)
{
   // pthread_detach(tid);
	//int ttid;
	int ret, sum, fd;
	char buf[1024];
	Pack *pack = (Pack *)arg;
	int i = 0;

	Pack tem;
	pack_list_t newNode;

    while(1)
    {
		plist->data.num = 0;
		newNode = (pack_list_t)malloc(sizeof(pack_node_t));
		sleep(0.5);
        int n = recv(pack->data.sfd, &tem, sizeof(Pack), 0);
		if(n == 0)
			exit(0);
		newNode->data = tem;

		switch (tem.status)
		{
			case fri:
				printf("\n[There's a new friend request.]\n");
				//fflush(stdin);
				break;
			
			case sb:
				printf("%d* %d\n", tem.info.id,ccid);
				
				if(tem.info.id == ccid)
				{
					printf("[%s %d-%d %d:%d:%d]\n", tem.fmnode.name1, tem.fmnode.date.month, tem.fmnode.date.day,
																	tem.fmnode.time.hour, tem.fmnode.time.minute, tem.fmnode.time.second);
					printf("%s\n", tem.data.sendbuf);
					//Friend_msg(pack);
				}
				else
					printf("\n[There's a new friend message of %s.]\n", tem.info.name);
				//fflush(stdin);
				//printf("**\n");
				break;

			case mem:
				//printf("%d* %d %d\n",pack->choice, ttid,ccid);
				if(tem.data.cid == ggid)
				{
					printf("[%s(%d) %d-%d %d:%d:%d]:", tem.info.name, tem.fmnode.id, tem.fmnode.date.month, tem.fmnode.date.day,
																	tem.fmnode.time.hour, tem.fmnode.time.minute, tem.fmnode.time.second);
					printf("%s\n", tem.data.sendbuf);
				}
				else
					printf("\n[There's a new group message of %s.]\n", tem.fmnode.name1);
				//fflush(stdin);
				break;			

			case gro:
				printf("\n[There's a new group notice.]\n");
				//fflush(stdin);
				break;

			case FILE_recv:
				//printf("**\n");
				fd = open(tem.finode.file_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
				//printf("%s, %d", tem.finode.file_name, tem.finode.file_size);
				sum = 0;
				while(1)
				{
					
					if(sum >= tem.finode.file_size)
					{
						printf("ζ₯ζΆζε\n");
						plist->data.status = FILE_send;
						break;
					}
					bzero(buf, 1024);
					//printf("**\n");
					ret = recv(pack->data.sfd, buf, sizeof(buf), 0);
					write(fd, buf, ret);
					sum+=ret;
					printf("%d/%d\n", sum, tem.finode.file_size);
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
		printf("\t\t\t[1]η»ι\n");
		printf("\t\t\t[2]ζ³¨ε\n");
		printf("\t\t\t[3]ζΎεε―η \n");
		printf("\t\t\t[4]ιεΊ\n");
		//printf("-------------------\n");
		printf("\n");
		printf("\t\t\tθΎε₯δ½ ηιζ©: ");
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
				printf("\t\t\tθ΄¦ε·οΌ");
				scanf("%d", &basepack->info.id);
				printf("\t\t\tε―η οΌ");
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
				printf("\t\t\tθ―·θΎε₯εε­:");
				scanf("%s", basepack->info.name);
				printf("\t\t\tθ―·θΎε₯ε―η :");
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
				printf("\t\t\tθ―·εζ¬‘θΎε₯ε―η :");
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
				printf("\t\t\tθ?Ύη½?ε―δΏ:");
				scanf("%s", basepack->info.question);	
				printf("\t\t\tθ―·θΎε₯η­ζ‘:");
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
				printf("\t\t\t\tζΎεε―η \n\n\n");
				printf("\t\t\tθ―·θΎε₯εΎζΎεηθ΄¦ε·:");
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
				printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
				break;
			case -2:
				printf("ε―η ιθ――\n");
				break;
			case -3:
				printf("δΈ€ζ¬‘ε―η δΈδΈθ΄\n");
				break;
			case 2:
				printf("η»ιζε\n");
				printf("θΎε₯δ»»ζε­η¬¦η»§η»­...\n");
				setbuf(stdin, NULL);
				getchar();
				return ;

			case 0:
				printf("ζ³¨εζε, δ½ ηθ΄¦ε·δΈΊ%d\n", basepack->info.id);
				break;
			case -5:
				printf("η­ζ‘ιθ――\n");
				break;
			case 1:
				do
				{
					printf("\t\t\tθ―·θΎε₯ζ°ηε―η :");
					scanf("%s", password_buf1);
					printf("\t\t\tθ―·εζ¬‘θΎε₯ε―η :");
					scanf("%s", password_buf2);
					if(strcmp(password_buf2, password_buf1) != 0)
					{
						printf("δΈ€ζ¬‘ε―η δΈδΈθ΄\n");
					}
					else
					{
						strcpy(basepack->info.password, password_buf1);
						basepack->choice = GET_NEW_PW;
                        send(basepack->data.sfd, basepack, sizeof(Pack), 0);

						Recv_func(basepack);
					}
				}while(basepack->status != 7);
				printf("ζΎεε―η ζε\n");
				break;
			case -4:
				printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
				break;
		}
		printf("θΎε₯δ»»ζε­η¬¦η»§η»­...\n");
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
		printf("θΎε₯δ»»ζε­η¬¦η»§η»­...\n");
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
		printf("\t\t\t[1]δΈͺδΊΊθ?Ύη½?\n");
		printf("\t\t\t[2]ε₯½ε\n");
		printf("\t\t\t[3]ηΎ€θ\n");
		printf("\t\t\t[4]ιεΊη»ι\n");
		printf("\n");
		printf("\t\t\tθ―·θΎε₯δ½ ηιζ©:");

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
				send(basepack->data.sfd, basepack, sizeof(Pack), 0);
				return ;
			
			default:
				printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
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
		printf("\t\t\t[1]ζ₯ηδΈͺδΊΊδΏ‘ζ―\n");
		printf("\t\t\t[2]ζ΄ζΉεε­\n");
		printf("\t\t\t[3]ζ΄ζΉε―η \n");
		printf("\t\t\t[4]ζ΄ζΉε―δΏ\n");
		printf("\t\t\t[5]ζ΄ζΉη­ζ‘\n");
		printf("\t\t\t[6]θΏε\n");
		printf("\n");
		printf("\t\t\tθ―·θΎε₯δ½ ηιζ©:");

		scanf("%d", &pack->choice);
		pack->choice +=19;
		switch (pack->choice)
		{
			case VIEW_perinfo:
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				Recv_func(pack);
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\tδΈͺδΊΊδΏ‘ζ―\n\n");
				printf("\t\t\tεε­:%s\n", pack->info.name);
				printf("\t\t\tε―δΏ:%s\n", pack->info.question);
				pack->status = 10;
				break;

			case CHANGE_name:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\tδΏ?ζΉεε­\n\n");
				printf("\t\t\tθ―·θΎε₯ζ°εε­:");
				scanf("%s", pack->info.name);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				Recv_func(pack);
				break;

			case CHANGE_password:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\tδΏ?ζΉε―η \n\n");
				printf("\t\t\tθ―·θΎε₯ζ°ε―η :");
				scanf("%s", password_buf1);
				printf("\t\t\tθ―·εζ¬‘θΎε₯ζ°ε―η οΌ");
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
				printf("\t\t\t\tδΏ?ζΉε―δΏ\n\n");
				printf("\t\t\tθ―·θΎε₯ζ°ηε―δΏ:");
				scanf("%s", pack->info.question);
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				Recv_func(pack);
				break;

			case CHANGE_answer:
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t\tδΏ?ζΉε―δΏη­ζ‘\n\n");
				printf("\t\t\tθ―·θΎε₯ζ°ηε―δΏη­ζ‘:");
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
				printf("ζ΄ζΉε―η ζε\n");
				break;
			case 0:
				printf("ζ΄ζΉεε­ζε\n");
				break;
			case -1:
				printf("δΈ€ζ¬‘θΎε₯ηε―η δΈδΈθ΄\n");
				break;
			case 1:
				printf("ζ΄ζΉε―δΏζε\n");
				break;
			case 2:
				printf("ζ΄ζΉε―δΏη­ζ‘ζε\n");
				break;
			case -4:
				printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
				break;
			default:
				break;
		}
		printf("θΎε₯δ»»ζε­η¬¦θΏε...\n");
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
		printf("\t\t\t[1]ζ·»ε ε₯½ε\n");
		printf("\t\t\t[2]ζ₯ηε₯½εθ―·ζ±\n");
		printf("\t\t\t[3]ιζ©ε₯½ε\n");
		printf("\t\t\t[4]θΏε\n");
		printf("\n");
		printf("\t\t\tθ―·θΎε₯δ½ ηιζ©:");

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
				printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
				break;
		}
		printf("θΎε₯δ»»ζε­η¬¦θΏε...\n");
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
	printf("\t\t\t\tε₯½εεθ‘¨\n\n");
	printf("%d*\n", pack->num);
	if(pack->num == 0)
	{
		printf("\t\t\t\tζζ θ?°ε½\n\n");
	}
	else
	{
		//printf("--------------ε₯½εεθ‘¨------------\n");
		printf("\t\tid   name                  status\n");
		printf("\t\t----------------------------------------\n");
		for(i = 0; i < pack->num; i++)
		{
			Recv_func(pack);
			printf("\t\t%-3d  %-20s  ", pack->fnode.id, pack->fnode.name);
			if(pack->fnode.status == 1)
				printf("ε¨ηΊΏ\n");
			else
				printf("η¦»ηΊΏ\n");
		}
	}
}

void Add_friend(Pack basepack)
{
	Pack *pack = (Pack *)malloc(sizeof(Pack));
	*pack = basepack;
    printf("\t\t\tθ―·θΎε₯δ½ θ¦ζ·»ε ηη¨ζ·θ΄¦ε·:");
	scanf("%d", &pack->data.cid);
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);

    switch (pack->status)
    {
        case -1:
            printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
            break;
        case -3:
            printf("θ―₯η¨ζ·ε·²ζ―δ½ ηε₯½ε\n");
            break;
        case -2:
            printf("ε₯½εη³θ―·ε·²εεΊοΌθ―·εΏιε€η³θ―·\n");
            break;
        case 0:
            printf("ε₯½εη³θ―·ε·²εεΊ\n");
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
	printf("\t\t\t\tε₯½εη³θ―·εθ‘¨\n\n");
	if(pack->status == 0)
	{
		printf("\t\t\t\t  ζζ θ?°ε½\n\n");
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
		printf("\t\t\tθ―·θΎε₯εΎε€ηηθ΄¦ε·ζθΎε₯[0]θΏε:");
		scanf("%d", &pack->data.cid);
		if(pack->data.cid == 0)
			break;
		printf("\t\t\t[1]εζ    [2]δΈεζ    [3]θΏε\n");
		printf("\t\t\tθ―·θΎε₯δ½ ηιζ©: ");
		scanf("%d", &pack->status);
		if(pack->status < 1  && pack->status > 2)
			break;
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		printf("ε€ηζε\n");

		printf("[1]η»§η»­ε€η    [2]θΏε\n");
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
	printf("\t\t\tθ―·θΎε₯ε₯½εθ΄¦ε·:");
	scanf("%d", &pack->data.cid);

	switch (Is_friend(pack))
	{
		case -1:
			printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
			return ;
		case -2:
			printf("θ―₯η¨ζ·δΈζ―δ½ ηε₯½ε\n");
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
		printf("\t\t\t[1]ζ₯ηε₯½εδΏ‘ζ―\n");
		printf("\t\t\t[2]θε€©\n");
		printf("\t\t\t[3]δΌ ζδ»Ά\n");
		printf("\t\t\t[4]ζΆζδ»Ά\n");
		printf("\t\t\t[5]ε±θ½ε₯½ε\n");
		printf("\t\t\t[6]ε ι€ε₯½ε\n");
		printf("\t\t\t[7]θΏε\n");
		//printf("----------------------------------------\n");
		printf("\n");
		printf("\t\t\tθ―·θΎε₯δ½ ηιζ©:");

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
				printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
				break;
		}
		printf("θΎε₯δ»»ζε­η¬¦θΏε...\n");
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
	printf("\t\t\t\tε₯½εδΏ‘ζ―\n\n");
	printf("\t\t\tθ΄¦ε·:%d\n", pack->fnode.id);
	printf("\t\t\tεε­:%s\n", pack->fnode.name);
	printf("\t\t\tηΆζ:");
	switch(pack->fnode.status)
	{
		case 1:
			printf("ε¨ηΊΏ\n");
			break;
		case 0:
			printf("η¦»ηΊΏ\n");
			break;
		default:
			printf("εΆδ»\n");
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
		printf("\t\t\t\tζζ θ?°ε½\n");
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
	/*printf("θ―·θΎε₯ε₯½εθ΄¦ε·: ");
	scanf("%d", &pack->data.cid);*/
	
	switch (Is_friend(pack))
	{
		case -1:
			printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
			return ;
		case -2:
			printf("θ―₯η¨ζ·δΈζ―δ½ ηε₯½ε\n");
			return;
		case 1:
			break;
		default:
			printf("ζͺη₯ιθ――\n");
			return ;
	}

	
	ccid = pack->data.cid;
	Friend_msg(pack);

	if(Is_shield(pack) == 1)
	{
		printf("ε·²ε±θ½θ―₯ε₯½ε\n");
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
				printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
				return ;
			case -2:
				printf("θ―₯η¨ζ·δΈζ―δ½ ηε₯½ε\n");
				return;
			case 1:
				break;
			default:
				printf("ζͺη₯ιθ――\n");
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
	char file_path[128] = {0};//ζδ»Άθ·―εΎ
	char file_info[2048] = {0};//ζδ»ΆδΏ‘ζ―
	char buf[1024] = {0};

	switch (Is_friend(pack))
	{
		case -1:
			printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
			return ;
		case -2:
			printf("θ―₯η¨ζ·δΈζ―δ½ ηε₯½ε\n");
			return;
		case 1:
			break;
		default:
			printf("ζͺη₯ιθ――\n");
			return ;
	}

	if(Is_shield(pack) == 1)
	{
		printf("ε·²ε±θ½θ―₯ε₯½ε\n");
		return ;
	}
	
	//θ·εη¨ζ·θΎε₯ηζδ»Άθ·―εΎ
	printf("\t\t\tθ―·θΎε₯ζδ»Άηη»ε―Ήθ·―εΎ/0θΏε:");
	scanf("%s", file_path);

	strncpy(pack->finode.file_name, basename(file_path), sizeof(file_path));
	
	//ζεΌζδ»Ά
	int fd = open(file_path, O_RDWR);
	if (fd == -1)
	{
		printf("ζεΌζδ»Ά%sε€±θ΄₯\n", file_path);
		return ;
	}	

	//θ?‘η?ζδ»Άε€§ε°
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

	int send_len = 0;//θ?°ε½ειδΊε€ε°ε­θ
	
	while (1)
	{	
		bzero(buf, 1024);
		//θ―»εζ°ζ?
		int ret = read(fd, buf, sizeof(buf));
		printf("ειδΈ­...\n");
		//printf("%d\n", ret);
		if (ret <= 0)
		{
			printf("ειζδ»Άζε\n");
			break;
		}
			
		//ειζ°ζ?
		//pack->choice = FILE_send;
		//printf("%d %d\n", pack->choice, pack->info.id);
		send(pack->data.sfd, buf, ret, 0);
		
		send_len += ret;//η»θ?‘ειδΊε€ε°ε­θ
		
		//δΈδΌ ζδ»ΆηηΎεζ― 
		//printf("*%d\n", send_len);
	}
	//pack->choice = 0;
	//printf("--choice = %d, id = %d, name = %s, serfd = %d, sfd = %d\n", pack->choice, pack->info.id, pack->info.name, pack->data.serfd, pack->data.sfd);
	//send(pack->data.sfd, pack, sizeof(Pack), 0);
	//printf("%d\n", send_len);
	// ε³ι­ζδ»Ά 
	close(fd);
}

void Recv_file(Pack *pack)
{
	View_filelist(pack);

	printf("\n\t\tθ―·θΎε₯ζδ»Άε/0θΏε:");
	scanf("%s", pack->finode.file_name);
		/*if(atoi(pack->finode.buf) == 0)
			return ;*/
	printf("\t\t[1]ζ₯ζΆ   [2]θΏε\n");
	printf("\t\tθ―·θΎε₯δ½ ηιζ©:");
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
					printf("ζδ»Άειθ――\n");
					break;
				}*/

				printf("ζ₯ζΆδΈ­...\n");
				while(1)
				{
					if(plist->data.status == FILE_send)
					{
						printf("11\n");
						plist->data.status = 0;
						break;
					}
						
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
	printf("\t\t\t\tζδ»Άεθ‘¨\n\n");
	if(pack->num == 0)
	{
		printf("\t\t\t\tζζ θ?°ε½\n\n");
	}
	else
	{
		printf("\t\t  ε€§ε°          Name                  ηΆζ\n");
		printf("\t\t-------------------------------------------\n");
		for(i = 0; i < pack->num; i++)
		{
			Recv_func(pack);
			printf("\t\t  %-12d  %-20s  ", pack->finode.file_size, pack->finode.file_name);
			switch (pack->finode.num)
			{
				case 1:
					printf("ζͺζ₯ζΆ\n");
					break;
				case 2:
					printf("ε·²ζ₯ζΆ\n");
					break;
				default:
					printf("εΆδ»\n");
			}
		}
	}
}

void Shield_friend(Pack *pack)
{
	switch (Is_friend(pack))
	{
		case -1:
			printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
			return ;
		case -2:
			printf("θ―₯η¨ζ·δΈζ―δ½ ηε₯½ε\n");
			return;
		case 1:
			break;
		default:
			printf("ζͺη₯ιθ――\n");
			return ;
	}

	switch (Is_shield(pack))
	{
		case 0:
			while(1)
			{
				printf("\t\t\tζ―ε¦ε±θ½θ―₯ε₯½ε(1/0):");
				scanf("%d", &pack->choice);
				switch (pack->choice)
				{
					case 1:
						pack->choice = SHLELD_friend;
						send(pack->data.sfd, pack, sizeof(Pack), 0);
						printf("θ?Ύη½?ζε\n");
						return ;
					case 0:	
						return ;
					default:
						printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
				}
			}
			break;
		case 1:
			while(1)
			{
				printf("\t\t\tε·²ε±θ½θ―₯ε₯½ε\n");
				printf("\t\t\tζ―ε¦θ§£ι€ε±θ½(1/0):");
				scanf("%d", &pack->choice);
				switch (pack->choice)
				{
					case 1:
						pack->choice = CANCEL_shield;
						send(pack->data.sfd, pack, sizeof(Pack), 0);
						printf("θ?Ύη½?ζε\n");
					case 0:	
						return ;
					default:
						printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
				}
			}
			break;
		default:
			printf("ζͺη₯ιθ――\n");
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
	printf("\t\t\tθ―·θΎε₯εΎε ι€ηε₯½εθ΄¦ε·:");
	scanf("%d", &pack->data.cid);

	switch (Is_friend(pack))
	{
		case -1:
			printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
			return ;
		case -2:
			printf("θ―₯η¨ζ·δΈζ―δ½ ηε₯½ε\n");
			return;
		case 1:
			break;
		default:
			printf("ζͺη₯ιθ――\n");
			return ;
	}

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	printf("ε ι€ζε\n");
}

void Opt2(Pack *pack)
{
	while(1)
	{
		View_grouplist(pack);

		printf("\t\t\t[1]ε»ΊηΎ€\n");
		printf("\t\t\t[2]ζ₯ηηΎ€ιη₯\n");
		printf("\t\t\t[3]ζ·»ε ηΎ€θ\n");
		printf("\t\t\t[4]θΏε₯ηΎ€θ\n");
		printf("\t\t\t[5]θΏε\n");
		printf("\n");
		printf("\t\t\tθ―·θΎε₯δ½ ηιζ©:");

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
				printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
				break;
		}
		printf("θΎε₯δ»»ζε­η¬¦θΏε...\n");
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
	printf("\t\t\t\tηΎ€θεθ‘¨\n\n");
	printf("\t\t  id   name                  status\n");
	printf("\t\t  -----------------------------------\n");
	if(pack->num == 0)
	{
		printf("\t\t\tζζ θ?°ε½\n");
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
					printf("ζε\n");
					break;
				case 2:
					printf("η?‘ηε\n");
					break;
				case 9:
					printf("ηΎ€δΈ»\n");
					break;
				default:
					printf("εΆδ»\n");
					break;
			}
		}
	}
	printf("\n");
}

void Set_group(Pack *pack)
{
	printf("\t\t\tθ―·θΎε₯ηΎ€θεη§°:");
	scanf("%s", pack->fnode.name);
	pack->choice = SET_group;
	pack->fmnode.date = DateNow();
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	printf("εε»Ίζε\n");
}

void Add_group(Pack *pack)
{
    printf("\t\t\tθΎε₯εΎε ε₯ηΎ€id:");
	scanf("%d", &pack->data.cid);
	pack->choice = ADD_group;
	send(pack->data.sfd, pack, sizeof(Pack), 0);

	Recv_func(pack);

    switch (pack->status)
    {
        case -1:
            printf("θ―₯ηΎ€IDδΈε­ε¨\n");
            break;
        case -3:
            printf("δ½ ε·²η»ζ―θ―₯ηΎ€ζε\n");
            break;
        case -2:
            printf("ε ηΎ€η³θ―·ε·²εεΊοΌθ―·εΏιε€ει\n");
            break;
        case 0:
            printf("ειζε\n");
            break;
		
		default:
			printf("ζͺη₯ιθ――\n");
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
	printf("                                    ηΎ€ιη₯\n\n");
	printf("     Group                      User \n");
	printf("ID   Name                  ID   Name                  ηΆζ               η±»ε\n");
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
					printf("ε·²ζδΈΊηΎ€δΈ»         ");
					break;
				case 2:
					printf("ε·²ζδΈΊη?‘ηε       ");
					break;
				case 0:
					printf("εΎε€η            ");
					break;
				case 1:
					printf("ε·²εζ            ");
					break;
				case 11:
					printf("εΆδ»η?‘ηεε·²εζ  ");
					break;
				case 3:
					printf("ε·²ζη»            ");
					break;
				case 12:
					printf("εΆδ»η?‘ηεε·²ζη»  ");
					break;
				case 4:
					printf("θ’«η§»εΊηΎ€θ        ");
					break;
				case 5:
					printf("ε·²ιηΎ€          ");
				case -1:
					printf("ηΎ€ε·²θ’«θ§£ζ£        ");
					break;
				default:
					printf("εΆδ»             ");
					break;
			}
			switch (pack->fmnode.time.second)
			{

				case 1:
					printf("ηΎ€ιη₯\n");
					break;
				case 2:
					printf("ηΎ€η³θ―·\n");
					break;
				default:
					printf("εΆδ»\n");
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

		printf("\n\t\tθ―·θΎε₯εΎε€ηηηΎ€θθ΄¦ε·/[0]θΏε:");
		scanf("%d", &pack->data.cid);
		if(pack->data.cid == 0)
			return ;
		printf("\t\tθ―·θΎε₯εΎε€ηηη¨ζ·θ΄¦ε·/[0]θΏε:");
		scanf("%d", &pack->fnode.id);
		if(pack->data.cid == 0)
			return ;
		printf("\t\t[1]εζ   [2]δΈεζ  [3]θΏε\n");
		printf("\t\tθ―·θΎε₯δ½ ηιζ©: ");
		scanf("%d", &pack->status);
		if(pack->status == 3)
			return ;
		send(pack->data.sfd, pack, sizeof(Pack), 0);
		printf("ε€ηζε\n");

		printf("[1]η»§η»­ε€η    [2]θΏε\n");
		scanf("%d", &pack->status);
		if(pack->status != 1)
			break;
	}
}

void Opt3(Pack *pack)
{
	printf("\t\t\tθΎε₯θ¦θΏε₯ηηΎ€θθ΄¦ε·:");
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
				printf("\t\t\t[1]θε€©\n");
				printf("\t\t\t[2]ζ₯ηηΎ€\n");
				printf("\t\t\t[3]ιηΎ€\n");
				printf("\t\t\t[4]θΏε\n\n");
				printf("\t\t\tθ―·θΎε₯δ½ ηιζ©:\n");

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
						printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
						break;

				}
				printf("θΎε₯δ»»ζε­η¬¦θΏε...\n");
				setbuf(stdin, NULL);
				getchar();
			}
			break;

		case 2:
			while(1)
			{
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t[1]θε€©\n");
				printf("\t\t\t[2]ζ₯ηηΎ€\n");
				printf("\t\t\t[3]ιηΎ€\n");
				printf("\t\t\t[4]θΏε\n");
				printf("\t\t\t[5]θΈ’δΊΊ\n");
				printf("\n\t\t\tθ―·θΎε₯δ½ ηιζ©:");

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
						printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
						break;
				}
				printf("θΎε₯δ»»ζε­η¬¦θΏε...\n");
				setbuf(stdin, NULL);
				getchar();
			}
				break;
		
		case 9:
			while(1)
			{
				system("clear");
				printf("\n\n\n\n\n\n");
				printf("\t\t\t[1]θε€©\n");
				printf("\t\t\t[2]ζ₯ηηΎ€\n");
				printf("\t\t\t[3]ιηΎ€\n");
				printf("\t\t\t[4]θΏε\n");
				printf("\t\t\t[5]θΈ’δΊΊ\n");
				printf("\t\t\t[6]θ?Ύη½?η?‘ηε\n");
				printf("\t\t\t[7]θ½¬θ?©ηΎ€\n");
				printf("\t\t\t[8]θ§£ζ£ηΎ€\n");
				printf("\t\t\t[9]ζ€ιη?‘ηε\n\n");
				printf("\t\t\tθ―·θΎε₯δ½ ηιζ©:");

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
						printf("ηΎ€δΈ»δΈθ½η΄ζ₯ιηΎ€οΌε―δ»₯θ½¬θ?©ηΎ€ειηΎ€ζθ§£ζ£ηΎ€\n");
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
						printf("θ―·θΎε₯ζ­£η‘?ιζ©\n");
						break;
				}
				printf("θΎε₯δ»»ζε­η¬¦θΏε...\n");
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
		printf("\t\t\tζζ θ?°ε½\n");
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
	switch (Is_group(pack))
	{
		case -1:
			printf("θ―₯θ΄¦ε·δΈε­ε¨\n");
			return ;
		case -2:
			printf("δΈζ―θ―₯ηΎ€ζε\n");
			return;
		case 1:
			break;
		default:
			printf("ζͺη₯ιθ――\n");
			return ;
	}
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
	printf("\t\tηΎ€id:%d\n", pack->data.cid);
	printf("\t\tηΎ€ε:%s\n", pack->fmnode.name1);
	printf("\t\tηΎ€δΈ»id:%d\n", pack->fmnode.id);
	printf("\t\tηΎ€δΈ»:%s\n", pack->fmnode.name2);
	printf("\t\tηΎ€δΊΊζ°:%d\n", pack->num);
	printf("\t\tεε»Ίζ₯ζ:%d-%d-%d\n", pack->fmnode.date.year, pack->fmnode.date.month, pack->fmnode.date.day);

	Recv_func(pack);

	printf("\t\t\t\tηΎ€ζε\n");
	printf("\t\tID   Name                  η±»ε\n");
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
					printf("ζε\n");
					break;
				case 2:
					printf("η?‘ηε\n");
					break;
				case 9:
					printf("ηΎ€δΈ»\n");
					break;
				default:
					printf("ζͺη₯ιθ――\n");
					break;
			}
		}
	}
}

void Remove_member(Pack *pack)
{
	printf("\t\t\tθ―·θΎε₯δ½ θ¦θΈ’εΊθ―₯ηΎ€θηθ΄¦ε·:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	switch (pack->status)
	{
		case -2:
			printf("δΈζ―θ―₯ηΎ€ζε\n");
			break;
		case -3:
			printf("ζ ζι\n");
			break;
		case 0:
			printf("ζδ½ζε\n");
			break;
		default:
			printf("ζͺη₯ιθ――\n");
			break;
	}
}

void Exit_group(Pack *pack)
{
	send(pack->data.sfd, pack, sizeof(Pack), 0);
	printf("ιηΎ€ζε\n");
}

void Set_admini(Pack *pack)
{
	printf("\t\t\tθΎε₯ιθ¦θ?Ύη½?ηζεθ΄¦ε·:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	switch (pack->status)
	{
		case -4 :
			printf("θ―₯idδΈε­ε¨\n");
			break;
		case -1:
			printf("δΈζ―θ―₯ηΎ€ζε\n");
			break;
		case -2:
			printf("θ―₯ζεε·²ζ―η?‘ηε,θ―·εΏιε€θ?Ύη½?\n");
			break;
		case -3:
			printf("δ½ ε·²η»ζ―ηΎ€δΈ»\n");
			break;
		case 0:
			printf("θ?Ύη½?ζε\n");
			break;
	}
}

void Dissolve_group(Pack *pack)
{
	char ch;
	
	while(1)
	{
		printf("\t\t\tη‘?θ?€θ§£ζ£ηΎ€οΌy/n)\n:");
		fflush(stdin);
		ch = getchar();
		switch(ch)
		{
			case 'n':
				return;
			case 'y':
				send(pack->data.sfd, pack, sizeof(Pack), 0);
				printf("θ§£ζ£ζε\n");
				printf("θΎε₯δ»»ζε­η¬¦θΏε...\n");
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
	printf("\t\t\tθΎε₯ιθ¦θ½¬θ?©ηζεθ΄¦ε·:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	switch (pack->status)
	{
		case -1 :
			printf("θ―₯η¨ζ·δΈε­ε¨\n");
			break;
		case -2:
			printf("θ―₯η¨ζ·δΈζ―ηΎ€ζε\n");
			break;
		case -3:
			printf("δ½ ε·²η»ζ―ηΎ€δΈ»\n");
			break;
		case 0:
			printf("θ½¬θ?©ζε\n");
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
	printf("\t\t\tθΎε₯ιθ¦θ?Ύη½?ηη?‘ηεθ΄¦ε·:");
	scanf("%d", &pack->fnode.id);

	send(pack->data.sfd, pack, sizeof(Pack), 0);
	Recv_func(pack);
	switch (pack->status)
	{
		case -4 :
			printf("θ―₯idδΈε­ε¨\n");
			break;
		case -1:
			printf("δΈζ―θ―₯ηΎ€ζε\n");
			break;
		case -2:
			printf("θ―₯ζεδΈζ―η?‘η\n");
			break;
		case 0:
			printf("θ?Ύη½?ζε\n");
			break;
		default :
			printf("ζͺη₯ιθ――\n");
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