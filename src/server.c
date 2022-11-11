#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
//#include <pthread.h>

#include "messages.h"
#include "request.h"

#define SOCKET_ERROR (-1)
#define REQUEST_PACKET_SIZE 49
#define RESPONSE_PACKET_SIZE 8
#define SERVER_BACKLOG 3

#define SA struct sockaddr

void print_SHA(const unsigned char *SHA) {
    if (SHA != NULL) {
        for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            printf("%02x", SHA[i]);
        }
        putchar('\n');
    }
}

int compare(const uint8_t *to_compare, const Request *request) {
    if (to_compare != NULL && request != NULL) {
        if (memcmp(to_compare, request->hash, SIZE_HASH) == 0) {
            print_SHA(to_compare);
            return 1;
        }
        return 0;
    }
    perror("ERROR: Pointers \"to_compare\" and/or \"request\" is/are NULL: ");
    exit(EXIT_FAILURE);

}

uint8_t *hash(uint64_t *to_hash) {
    if (to_hash != NULL) {
        uint8_t *hashed = SHA256((unsigned char *) to_hash, 8, NULL);
        return hashed;
    }
    perror("ERROR: Pointer \"to_hash\" is NULL: ");
    exit(EXIT_FAILURE);
}

uint64_t decode(const Request *request) {
    if (request != NULL) {
        uint64_t i = request->start;
        while (compare(hash(&i), request) != 1 && i < request->end) {
            ++i;
        }
        printf("Decoded: %" PRIu64 "\n", i);
        return i;
    }
    perror("ERROR: Pointer \"request\" is NULL: ");
    exit(EXIT_FAILURE);
}

int check(int exp, const char *msg) {
    if (exp == SOCKET_ERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}


int compute(int connfd) {
   // int connfd = *((int *) p_connfd);
   // free(p_connfd); // We don't need it anymore

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
    uint64_t answer = htobe64(decode(request));

    // Send answer to the client
    size_t err = send(connfd, &answer, RESPONSE_PACKET_SIZE, 0);
    if (err != RESPONSE_PACKET_SIZE) {
        fprintf(stderr, "ERROR: Failed to send: ");
        perror(NULL);
        return SOCKET_ERROR;
    }

    // We free the request
    free(request);
    request = NULL;

    close(connfd);
    //printf("Closing connection.\n");
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

    check((sockfd = socket(AF_INET, SOCK_STREAM, 0)), "Socket creation failed...");
    printf("Socket successfully created.\n");

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int one = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));

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
        // printf("Connected to client.\n");
        // Create a new pointer foreach thread to not mess with several threads
          int err = compute(connfd);
          if(err != 0){
              fprintf(stderr, "Programwas interrupted by an error number %d",err);
          }
        //int *p_connfd = malloc(sizeof(int));
        //*p_connfd = connfd;
        //pthread_create(&t, NULL, compute, p_connfd);
    }

    shutdown(sockfd, SHUT_RDWR);
    return 0;
}



