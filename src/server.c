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
#include <pthread.h>

#include "messages.h"
#include "request.h"
#include "queue.h"

#define SOCKET_ERROR (-1)
#define REQUEST_PACKET_SIZE 49
#define RESPONSE_PACKET_SIZE 8
#define SERVER_BACKLOG 1024
#define THREAD_POOL_SIZE 4
#define SA struct sockaddr
#define SA_IN struct sockaddr_in

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER; // Threads wait until something happens and then wake up

void print_SHA(const unsigned char *SHA);

int compare(const uint8_t *to_compare, const Request *request);

uint8_t *hash(uint64_t *to_hash);

uint64_t decode(const Request *request);

int check(int exp, const char *msg);

void *compute(void *p_connfd);

void *thread_function(void *arg);

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
        if (memcmp(to_compare, request->hash, SHA256_DIGEST_LENGTH) == 0) {
            // print_SHA(to_compare);
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
        // printf("Decoded: %" PRIu64 "\n", i);
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

// compute now takes a pointer and return a pointer
void *compute(void *p_connfd) {
    int connfd = *((int *) p_connfd);
    free(p_connfd); // We don't need it anymore
    p_connfd = NULL;

    unsigned char buff[REQUEST_PACKET_SIZE];

    // read the message from client and copy it in buffer
    size_t length = read(connfd, buff, sizeof(buff));
    if (length != REQUEST_PACKET_SIZE) {
        fprintf(stderr, "ERROR: Unable to read %d elements, read only %zu elements: ", REQUEST_PACKET_SIZE, length);
        perror(NULL);
        return NULL;
    }

    Request *request = getRequest(buff, REQUEST_PACKET_SIZE);
    uint64_t answer = htobe64(decode(request));

    // Send answer to the client
    size_t err = send(connfd, &answer, RESPONSE_PACKET_SIZE, 0);
    if (err != RESPONSE_PACKET_SIZE) {
        fprintf(stderr, "ERROR: Failed to send: ");
        perror(NULL);
        return NULL;
    }

    // We free the request
    free(request);
    request = NULL;

    close(connfd);
    //printf("Closing connection.\n");
    return NULL;
}

void *thread_function(void *arg) {
    // Infinite loop because we never want these threads to die
    while (1) {
        int *pclient;
        pthread_mutex_lock(&mutex);
        if ((pclient = dequeue()) == NULL) { // Don't wait if the queue is non-empty
            pthread_cond_wait(&condition_var, &mutex); // Wait until it signals and releases the lock
            // Try again to dequeue in case the queue is not empty anymore
            pclient = dequeue();
        }
        pthread_mutex_unlock(&mutex);
        if (pclient != NULL) {
            // We have a connection
            compute(pclient);
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
    if (tmp > INT_MAX || tmp < INT_MIN || *err_port) {// Overflow or Underflow or Extra data not null
        fprintf(stderr, "ERROR: Port number overflow or underflow or too long.\n");
        exit(EXIT_FAILURE);
    }
    int port = tmp;

    // Socket creation
    int sockfd, connfd;
    SA_IN servaddr;
    int addrlen = sizeof(servaddr);

    // First off, create a bunch of threads to handle future connections
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
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

        // We create a linked list (queue) to put the connection somewhere so that an available thread can find it
        // Make sure only one thread messes with the queue at a time (evict race condition)
        // Thread-safe implementation -- Same as been done during dequeue
        pthread_mutex_lock(&mutex);
        enqueue(p_connfd);
        pthread_cond_signal(&condition_var); // Wake up the thread
        pthread_mutex_unlock(&mutex);
    }

    shutdown(sockfd, SHUT_RDWR);
    return 0;
}



