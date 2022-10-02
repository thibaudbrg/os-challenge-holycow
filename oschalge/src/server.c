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
#include "hashtable.h"
#include "request.h"

#define REQUEST_PACKET_SIZE 49
#define RESPONSE_PACKET_SIZE 8

#define SA struct sockaddr


void print_SHA(const unsigned char *SHA) {
    if (SHA != NULL) {
        putchar('\n');
        for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            printf("%02x", SHA[i]);
        }
        putchar('\n');
    }
}


/*u_int64_t compare(Hash_table *table, Request *request) {
    size_t n = -1;
    size_t i = 0;
    while (i < table->length && n == -1) {
        size_t j = 0;
        while (j < SIZE_HASH && table->table[i][j] == request->hash[j]) {
            if (j == SIZE_HASH - 1) {
                n = i;
            }
            ++j;
        }
        ++i;
    }
    uint64_t answer = (uint64_t) n + request->start;
    return answer;
}*/

int compare2(const uint8_t *to_compare, Request *request) {
    int n = 0;
    size_t j = 0;
    while (j < SIZE_HASH && to_compare[j] == request->hash[j]) {
        if (j == SIZE_HASH - 1) {
            n = 1;
        }
        ++j;
    }
    return n;
}

uint8_t *hash(uint64_t *to_hash) {
    unsigned char *hashed = SHA256((unsigned char *) to_hash, 8, NULL);

    uint8_t *result = calloc(SIZE_HASH, sizeof(uint8_t));
    for (size_t i = 0; i < SIZE_HASH; ++i) {
        result[i] = hashed[i];
    }
    return result;
}


int func(int connfd) {
    unsigned char buff[REQUEST_PACKET_SIZE];
    bzero(buff, REQUEST_PACKET_SIZE);

    // read the message from client and copy it in buffer
    printf("Size of buffer : %ld\n", sizeof(buff));
    size_t length = read(connfd, buff, sizeof(buff));
    printf("The size of the packet received is: %ld\n", length);
    if (length != REQUEST_PACKET_SIZE) {
        fprintf(stderr, "ERROR: Unable to read %d elements, read only %zu elements.\n", REQUEST_PACKET_SIZE, length);
        return -2;
    }

    // print buffer which contains the client contents
    Request *request = getRequest(buff, REQUEST_PACKET_SIZE);
    //Hash_table *hash_table = create_hash_table(request);

    // Hash all possibilities and write them into the hash table

    //size_t n = 0;
    //int result = 0;
    u_int64_t i = request->start;
    uint64_t answer;


    while (compare2(hash(&i), request) != 1 && i < request->end ){
        ++i;
    }
    answer = htobe64(i);
    /*for (u_int64_t i = request->start; i < request->end; ++i) {
        hash_table->table[n] = hash(&i);
        result = compare2(hash(&i), request);
        if (result == 1) {
            // Turn the answer to a big endian encoding
            answer = htobe64(i);
            break;
        }
        ++n;
    }*/

    // copy server message in the buffer and send that buffer to client
    bzero(buff, REQUEST_PACKET_SIZE);
    size_t err = write(connfd, &answer, RESPONSE_PACKET_SIZE);
    if (err == -1) {
        fprintf(stderr, "error writing");
        exit(0);
    }

    // if msg contains "Exit" then server exit and chat ended.
    if (strncmp("exit", buff, 4) == 0) {
        printf("Server Exit...\n");
    }

    // We free the request
    destroy_request(request);
    request = NULL;
    free(request);

    // We free the hash_table
    //destroy_hash_table(hash_table);
    //hash_table = NULL;
    //free(hash_table);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Not enough argument, port number must be specified.\n");
        exit(0);
    } else if (argc > 2) {
        fprintf(stderr, "Too many arguments, only port number must be specified.\n");
        exit(0);
    }

    // Getting the port number
    char *err_port;
    long tmp = strtol(argv[1], &err_port, 10);
    if (tmp > INT_MAX || tmp < INT_MIN || *err_port) {// Overflow or Underflow or Extra data not null
        fprintf(stderr, "ERROR: Port number overflow or underflow or too long.\n");
        exit(0);
    }


    // socket create and verification

    int port = tmp;
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    while (1) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            fprintf(stderr, "socket creation failed...\n");
            exit(0);
        } else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);

        // Binding newly created socket to given IP and verification
        int one = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        if ((bind(sockfd, (SA *) &servaddr, sizeof(servaddr))) != 0) {
            fprintf(stderr, "socket bind failed...\n");
            exit(0);
        } else
            printf("Socket successfully binded..\n");

        // Now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0) {
            fprintf(stderr, "Listen failed...\n");
            exit(0);
        } else
            printf("Server listening..\n");
        len = sizeof(cli);

        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA *) &cli, (socklen_t *) &len);
        if (connfd < 0) {
            fprintf(stderr, "server accept failed...\n");
            exit(0);
        } else
            printf("server accept the client...\n");

        // Function for chatting between client and server
        int err_chat = func(connfd);
        if (err_chat != 0) {
            fprintf(stderr, "Program interrupted by an error of number: %d\n", err_chat);
            exit(0);
        }
        close(connfd);
        close(sockfd);
    }

    return 0;

}