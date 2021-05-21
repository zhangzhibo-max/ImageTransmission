#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#include<thread>

#define MAX_LEN 4096


typedef struct ConnectInfo
{
	int fd;
	bool bconnect;
	ConnectInfo()
	{
		memset(this,0,sizeof(ConnectInfo));
	}
}ConInfo;

#define MAX_CONNECT_NUM 5


ConInfo global_coninfo[MAX_CONNECT_NUM];
int clinum=0;
bool global_runing = false;

void send_image_to_all_client()
{
	std::string  strcmd;
	int n;
	char buff[MAX_LEN];
	FILE* fp=NULL;
	while(true)
	{
		strcmd.clear();
		std::cin>>strcmd;
		if(strstr(strcmd.c_str(),"start")!=NULL)
		{
			for(int i=0;i<MAX_CONNECT_NUM;i++)
			{
				if(!global_coninfo[i].bconnect)
					continue;
				if((fp = fopen("head.jpg","rb")) == NULL)
				{
					printf("open image fail.\n");
				}
				bzero(buff,sizeof(buff));
				while(!feof(fp))
				{
					n = fread(buff,1,sizeof(buff),fp);
					if(n != write(global_coninfo[i].fd , buff ,n))
					{
						printf("write.\n");
						break;
					}
				}
				printf("client: %d accept image over,quit.\n",global_coninfo[i].fd);
				close(global_coninfo[i].fd);
				global_coninfo[i].bconnect = false;
				fclose(fp);
				if(clinum>0)
					clinum--;
				printf("current client num : %d\n",clinum);
			}
		}
		if(strcmp(strcmd.c_str(),"quit")==0)
		{
			printf("bye bye...");
			exit(0);
		}
	}

}



int main()
{
	int listenfd,connfd;
	struct sockaddr_in servaddr;
	FILE *fp;
	int n;

	if((listenfd  = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		printf("create socket error:%s(errno :%d)\n",strerror(errno),errno);
		return 0;
	}

	printf(">init socket.\n");

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6666);

	int contain;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&contain,sizeof(int));

	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)
	{
		printf("bind socket error: %s(errno :%d)\n",strerror(errno),errno);
		return 0;
	}

	printf(">bind sucess!\n");

	if(listen(listenfd,10) == -1)
	{
		printf("listen socket error:%s (error: %d)\n",strerror(errno),errno);
		return 0;
	}

	printf("...waiting for clien's request...\n");


	std::thread t1(send_image_to_all_client);
	t1.detach();

	while(true){

		struct sockaddr_in client_addr;
		socklen_t size = sizeof(client_addr);
		if((connfd = accept(listenfd , (struct sockaddr*)&client_addr,&size)) == -1 )
		{
			printf("accept socket error:%s(errno:%d)",strerror(errno),errno);
			continue;
		}
		if(clinum <MAX_CONNECT_NUM)
		{
			global_coninfo[clinum].fd = connfd;
			global_coninfo[clinum].bconnect = true;
			clinum++;
			printf("client: %d connect success.current client num = %d.\n",connfd,clinum);
		}
		else
		{
			printf("There are more than %d clients.",MAX_CONNECT_NUM);
			char a[] ="Please connect later (^.^)!";
			int  sendlen = 0;
			sendlen	= write(connfd,a,strlen(a));
			if(sendlen ==0)
			{
				printf("send error.\n");
			}
			close(connfd);
		}
		/*
		   while(1)
		   {
		   n = read(connfd , buff,MAX_LEN);
		   if(n == 0)
		   break;
		   fwrite(buff,1,n,fp);
		   }
		   buff[n] = '\0';
		   printf("recv msg from client: %s\n",buff);
		   close(connfd);
		   fclose(fp);*/


	}

	close(listenfd);
	return 0;
}


