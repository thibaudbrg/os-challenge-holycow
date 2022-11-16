#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>

#include "messages.h"
#include "decoder.h"
#include "priorityqueue.h"
#include "hashTable.h"
#include "request.h"

#define SOCKET_ERROR (-1)
#define SERVER_BACKLOG 1024
#define THREAD_POOL_SIZE 6
#define SA struct sockaddr
#define SA_IN struct sockaddr_in

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//The_Hash *theHash;


int check(int exp, char const *msg) {
    if (exp == SOCKET_ERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}

void compute_SHA(node_t const *const work) {
    uint64_t answer = decode(work->request);
    // Send answer to the client
    size_t err = send(*work->connfd, &answer, PACKET_RESPONSE_SIZE, 0);
    if (err != PACKET_RESPONSE_SIZE) {
        fprintf(stderr, "ERROR: Failed to send (err = %zu) instead of %d: ", err, PACKET_RESPONSE_SIZE);
        perror(NULL);
    }
}

_Noreturn void *thread_function(void *arg) {
    Queue *queue = (Queue *) arg;
    // Infinite loop because we never want these threads to die
    while (1) {
        node_t *work = NULL;

        pthread_mutex_lock(&mutex);
        work = dequeue(queue);
        pthread_mutex_unlock(&mutex);

        if (work != NULL) {
            // We have a connection
            compute_SHA(work);

            // We free the connfd and the corresponding request
            destroy_node(work);
            free(work);
            work = NULL;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Number of arguments is not correct.\n ");
        exit(EXIT_FAILURE);
    }

    // Getting the port number
    char *err_port;
    long tmp = strtol(argv[1], &err_port, 10);
    if (tmp > INT_MAX || tmp < INT_MIN || *err_port) {
        fprintf(stderr, "ERROR: Port number overflow or underflow or too long.\n");
        exit(EXIT_FAILURE);
    }
    int port = (int) tmp;

    // Socket creation
    int sockfd, connfd;
    SA_IN servaddr;
    int addrlen = sizeof(servaddr);
    init_table();
    // theHash= mmap(NULL, sizeof(theHash), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
    Queue *queue = createQueue();

    // First off, create a bunch of threads to handle future connections
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_create(&thread_pool[i], NULL, thread_function, queue);
    }

    check((sockfd = socket(AF_INET, SOCK_STREAM, 0)), "Socket creation failed...");
    printf("Socket successfully created.\n");

    int one = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));
    bzero((char*)&servaddr, sizeof(servaddr));

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
        // printf("Connected to client.\n");

        int *p_connfd = malloc(sizeof(int));
        *p_connfd = connfd;

        pthread_mutex_lock(&mutex);
        enqueue(p_connfd, queue);
        pthread_mutex_unlock(&mutex);
    }
}