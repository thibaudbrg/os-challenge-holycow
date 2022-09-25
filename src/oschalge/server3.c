#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc,char *argv[])
{
    int sockfd,newsockfd,portno,clilen,n,i;
    char buffer[256];
    int optval=1;
    struct sockaddr_in serv_addr,cli_addr;

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
        error("ERROR opening socket");
    else printf("creation succesfull\n");
    bzero((char *)&serv_addr,sizeof(serv_addr));

    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
    serv_addr.sin_port=htons(atoi(argv[1]));
    if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
        error("ERROR on binding");
    else printf("binding succesfull\n");
    //while(1)

    n=listen(sockfd,5);
    if(n==-1)
        error("Error :listen");
    else printf("listening\n");
    clilen=sizeof(cli_addr);


    newsockfd=accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
    if(newsockfd<0)
        error("ERROR on accept");
    else printf("accept\n");
    bzero(buffer,256);


    n=read(newsockfd,buffer,255);
    if(n<0)
        error("ERROR reading from socket");
    printf("Here is the message:%s",buffer);
    n=write(newsockfd, "got your message",18);
    if(n<0)
        error("ERROR writing to socket");
    //close(sockfd);

    return 0;
}