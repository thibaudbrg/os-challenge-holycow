#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <signal.h>

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <unistd.h>
#include "messages.h"
#include "decoder.h"
#include "request.h"
#include "hashTable.h"



#define SOCKET_ERROR (-1)
#define REQUEST_PACKET_SIZE 49
#define RESPONSE_PACKET_SIZE 8
#define SERVER_BACKLOG 1000

#define SA struct sockaddr

The_Hash *theHash;


int check(int exp, const char *msg) {
    if (exp == SOCKET_ERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}

void sigHandler(int signal){
insert(theHash->hash,theHash->answer);
}
int compute(int connfd, The_Hash *theHash) {
    unsigned char buff[REQUEST_PACKET_SIZE];
    bzero(buff,REQUEST_PACKET_SIZE);

    // read the message from client and copy it in buffer
    size_t length = read(connfd, buff, sizeof(buff));
    if (length != REQUEST_PACKET_SIZE) {
        fprintf(stderr, "ERROR: Unable to read %d elements, read only %zu elements: ", REQUEST_PACKET_SIZE, length);
        perror(NULL);
        return SOCKET_ERROR;
    }

    Request *request = getRequest(buff, REQUEST_PACKET_SIZE);
    uint64_t answer;
    uint64_t search_answer = search(request->hash);
    uint8_t *hashed = SHA256((unsigned char *) &search_answer, 8, NULL);
    if(search_answer !=0 && memcmp(hashed, request->hash, SIZE_HASH) ==0){
        printf("request repeated");
        answer =htobe64(search_answer);
    } else {
        int child = fork();
        check(child, "Fork Failed...");
        answer = decode(request, theHash);
    }
    int err = send(connfd, &answer,RESPONSE_PACKET_SIZE,0);
    check(err,"Error writing to the client");

    // We free the request
    free(request);
    request = NULL;

    close(connfd);

    return 0;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Number of arguments is not correct.\n ");
        exit(EXIT_FAILURE);
    }

    // Getting the port number
    char *err_port;
    long tmp = strtol(argv[1], &err_port, 10);
    if (tmp > INT_MAX || tmp < INT_MIN || *err_port) {// Overflow or Underflow or Extra data not null
        fprintf(stderr, "ERROR: Port number overflow or underflow or too long.\n");
        exit(EXIT_FAILURE);
    }
    int port = tmp;

    // Socket creation
    int sockfd, connfd;
    struct sockaddr_in servaddr;
    int addrlen = sizeof(servaddr);

    theHash = mmap(NULL,sizeof(theHash),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
    signal(SIGCHLD,sigHandler);
    check((sockfd = socket(AF_INET, SOCK_STREAM, 0)), "Socket creation failed...");
    printf("Socket successfully created.\n");

    int one = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));
    bzero((char*)&servaddr,sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);



    // Binding newly created socket to given IP
    check(bind(sockfd, (SA *) &servaddr, addrlen), "Socket bind failed...");
    printf("Socket binded successfully.\n");

    // Now server is ready to listen
    check(listen(sockfd, SERVER_BACKLOG), "Listening failed...");
    printf("Server listening.\n");

    while (1) {

        // Accept the data packet from client
        check(connfd = accept(sockfd, (SA *) (struct sockaddr *) &servaddr, (socklen_t *) &addrlen),
              "Server accept failed...");
          int err = compute(connfd,theHash);
          if(err != 0){
              fprintf(stderr, "Program was interrupted by an error number %d",err);
          }
    }

    shutdown(sockfd, SHUT_RDWR);
    return 0;
}



