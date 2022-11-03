#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "messages.h"
#include "decoder.h"
#include "priorityqueue.h"

#define SOCKET_ERROR (-1)
#define SERVER_BACKLOG 1000
#define THREAD_POOL_SIZE 8
#define SA struct sockaddr
#define SA_IN struct sockaddr_in

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER; // Threads wait until something happens and then wake up

int check(int exp, char const *msg) {
    if (exp == SOCKET_ERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}

// compute now takes a pointer and return a pointer
void compute_SHA(node_t const * const work) {
    uint64_t answer = htobe64(decode(work->request));
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
        if (work == NULL) {
            pthread_cond_wait(&condition_var, &mutex);
            work = dequeue(queue);
        }
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

    Queue *queue = createQueue();

    // First off, create a bunch of threads to handle future connections
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_create(&thread_pool[i], NULL, thread_function, queue);
    }

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

        int *p_connfd = malloc(sizeof(int));
        *p_connfd = connfd;

        pthread_mutex_lock(&mutex);
        enqueue(p_connfd, queue);
        pthread_cond_signal(&condition_var); // Wake up the thread
        pthread_mutex_unlock(&mutex);
    }
}