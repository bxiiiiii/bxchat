#include "clie_task.c"
#define SERV_IP "192.168.30.192"

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
				memset(&basepack, 0, sizeof(Pack));
				basepack.data.sfd = cfd;
				Login_opt(&basepack);
				Check(basepack);
			}
		if(basepack.choice == EXIT3)
			break;
	}while(1);
	

    pthread_join(tid, NULL);
	return 0;
}