#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#define MAXLINE 4096

int main(int argc, char** argv){
    int   sockfd, len;
    char  buffer[MAXLINE];
    struct sockaddr_in  servaddr;
    FILE *fq;
/*
    if( argc != 2){
        printf("usage: ./client <ipaddress>\n");
        return 0;
    }
*/
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if( inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n",argv[1]);
        return 0;
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

	printf("connect suceess,fd = %d \n",sockfd);
   	/*
    bzero(buffer,sizeof(buffer));
    while(!feof(fq)){
        len = fread(buffer, 1, sizeof(buffer), fq);
        if(len != write(sockfd, buffer, len)){
            printf("write.\n");
            break;
        }
    }*/

	while(true)
    {
		len = read(sockfd , buffer,MAXLINE);
		printf("recvdata:%s\n",buffer);
        if(strstr(buffer,"later")!=NULL)
		{
			break;
		}
		
		if(len == 0)
            break;
		time_t rawtime;
		struct tm *info;
		char imagename[64]="";
		time( &rawtime );
		info = localtime( &rawtime );
		strftime(imagename, 64,"%Y%m%d%H%M%S.jpg", info);
		printf("image name: %s\n",imagename);
		if( ( fq = fopen(imagename,"ab") ) == NULL ){
       		printf("File open.\n");
        	close(sockfd);
        	exit(1);
    	}
        fwrite(buffer,1,len,fq);
   		printf("download the image success.quit!\n");
    }
    buffer[len] = '\0';

    close(sockfd);
	if(fq)
		fclose(fq);

    return 0;
}
