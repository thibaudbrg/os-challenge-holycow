#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "messages.h"
#include "request.h"

#define REQUEST_PACKET_SIZE 49
#define RESPONSE_PACKET_SIZE 8
#define MAX_PENDING 3

#define SA struct sockaddr

uint8_t *hash(uint64_t *to_hash) {
    if (to_hash != NULL) {
        uint8_t *hashed = SHA256((unsigned char *) to_hash, 8, NULL);
        return hashed;
    }
    exit(EXIT_FAILURE);
}


int compare(const uint8_t *to_compare, const Request *request) {
    if (memcmp(to_compare, request->hash, SIZE_HASH) == 0) {
        return 1;
    }
    return 0;
}


uint64_t decode(const Request *request) {
    if (request != NULL) {
        uint64_t i = request->start;
        while (compare(hash(&i), request) != 1 && i < request->end) {
            ++i;
        }
        return i;
    }
    exit(EXIT_FAILURE);
}


int compute(int connfd) {
    unsigned char buff[REQUEST_PACKET_SIZE];
    bzero(buff, REQUEST_PACKET_SIZE);

    // read the message from client and copy it in buffer
    size_t length = read(connfd, buff, REQUEST_PACKET_SIZE);

    Request *request = getRequest(buff, REQUEST_PACKET_SIZE);
    uint64_t answer = htobe64(decode(request));

    // Send answer to the client
    size_t err = send(connfd, &answer, RESPONSE_PACKET_SIZE,0);
    if (err != RESPONSE_PACKET_SIZE) {
        fprintf(stderr, "error writing");
        exit(EXIT_FAILURE);
    }

    // We free the request
    free(request);
    request = NULL;

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

    // socket create and verification
    int sockfd, connfd;
    struct sockaddr_in servaddr;
    int addrlen = sizeof(servaddr);

    //sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket creation failed...\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Socket successfully created..\n");
    }

    //bzero(&servaddr, addrlen);
    memset(&servaddr,0, addrlen);
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    int one = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if ((bind(sockfd, (SA *) &servaddr, addrlen)) < 0) {
        fprintf(stderr, "socket bind failed...\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Socket successfully binded..\n");
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, MAX_PENDING)) < 0) {
        fprintf(stderr, "Listen failed...\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Server listening..\n");
    }

    while (1) {
        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA *) (struct sockaddr *) &servaddr, (socklen_t *) &addrlen);
        if (connfd < 0) {
            fprintf(stderr, "server accept failed...\n");
            exit(EXIT_FAILURE);
        } else {
            printf("server accept the client...\n");
            // Function for chatting between client and server
            int err_chat = compute(connfd);
            if (err_chat != 0) {
                fprintf(stderr, "Program interrupted by an error of number: %d\n", err_chat);
                exit(EXIT_FAILURE);
            }
        }
    }
    close(connfd);
    shutdown(sockfd, SHUT_RDWR);
    return 0;
}