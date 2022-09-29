#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "messages.h"

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32)
/* Windows headers */
#include <winsock2.h>
#include <windows.h>
#include <io.h>

#else /* or check here for unix / linux & fail if OS unrecognized */
/* POSIX headers */
#include <netdb.h>
#include <poll.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

#endif /* _WIN32 */

/* shared headers */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
//imports de server youtube en haut
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include <inttypes.h>

#define MAX 392
#define REQUEST_PACKET_SIZE 49
#define RESPONSE_PACKET_SIZE 8
#define SIZE_HASH 32
#define SIZE_START 8
#define SIZE_END 8
#define SIZE_OF_BYTE 8

#define SA struct sockaddr

typedef struct {
    uint8_t *hash;
    u_int64_t start;
    u_int64_t end;
    uint8_t p;
} Request;

typedef struct {
    size_t length;
    uint8_t **table;
} Hash_table;


void print_SHA(const unsigned char *SHA) {
    if (SHA != NULL) {
        putchar('\n');
        for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            printf("%02x", SHA[i]);
        }
        putchar('\n');
    }
}


u_int64_t compare(Hash_table *table, Request *request) {
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
}

Hash_table *create_hash_table(const Request *request) {
    Hash_table *hash_table = NULL;
    hash_table = malloc(sizeof(Hash_table));
    if (hash_table != NULL) {
        hash_table->length = request->end - request->start;
        hash_table->table = calloc(hash_table->length, sizeof(uint8_t *));
        if (hash_table->table != NULL) {
            for (size_t i = 0; i < hash_table->length; ++i) {
                hash_table->table[i] = calloc(SIZE_HASH, sizeof(uint8_t));
                if (hash_table->table[i] != NULL) {
                    return hash_table;
                }
            }
        }
    }
    return NULL;
}

void destroy_hash_table(Hash_table *hash_table) {
    for (size_t i = 0; i < hash_table->length; ++i) {
        hash_table->table[i] = NULL;
    }
    hash_table->table = NULL;
    free(hash_table->table);
}

Request *create_empty_request(void) {
    Request *output = NULL;
    output = malloc(sizeof(Request));
    if (output != NULL) {
        output->hash = calloc(SIZE_HASH, sizeof(uint8_t));
        if (output->hash != NULL) {
            output->p = 0;
            output->start = 0;
            output->end = 0;
            printf("Request created sucessfully!\n");
            return output;
        }
    }
    return NULL;
}

void destroy_request(Request *request) {
    free(request->hash);
    request->hash = NULL;
}


Request *getRequest(const unsigned char *all_bytes, size_t length) {
    if (all_bytes != NULL) {
        Request *output = create_empty_request();

        if (output == NULL) {
            fprintf(stderr, "ERROR: Request is NULL.\n");
            return NULL;
        }

        for (size_t i = 0; i < SIZE_HASH; ++i) {
            output->hash[i] = (uint8_t) all_bytes[i];
        }
        for (size_t i = 0; i < SIZE_START; ++i) {
            output->start <<= SIZE_OF_BYTE;
            output->start |= (u_int64_t) all_bytes[SIZE_HASH + i];
            output->end <<= SIZE_OF_BYTE;
            output->end |= (u_int64_t) all_bytes[SIZE_HASH + SIZE_START + i];
        }

        output->p = (u_int8_t) all_bytes[SIZE_HASH + SIZE_START + SIZE_END];

        return output;
    } else {
        return NULL;
    }
}

uint8_t *hash(uint64_t *to_hash) {

    // Copy in the buffer the to_hash value but in a char* type
    char buff[SIZE_OF_BYTE];
    snprintf(buff, SIZE_OF_BYTE, "%"PRIu64, *to_hash);
    unsigned char *hashed = SHA256(to_hash, 8, NULL);

    uint8_t *result = calloc(SIZE_HASH, sizeof(uint8_t));
    for (size_t i = 0; i < SIZE_HASH; ++i) {
        result[i] = hashed[i];
    }
    return result;
}


int func(int connfd) {
    // infinite loop for chat
    unsigned char buff[MAX];
    int inf_loop = 1;
    while (inf_loop) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        printf("Size of buffer : %ld\n", sizeof(buff));
        size_t length = read(connfd, buff, sizeof(buff));
        printf("The size of the packet received is: %ld\n", length);
        if (length != REQUEST_PACKET_SIZE) {
            fprintf(stderr, "ERROR: Unable to read %d elements, read only %zu elements.\n", REQUEST_PACKET_SIZE,
                    length);
            return -2;
        }

        // print buffer which contains the client contents
        Request *request = getRequest(buff, MAX);
        Hash_table *hash_table = create_hash_table(request);

        // Hash all possibilities and write them into the hash table
        size_t n = 0;
        for (u_int64_t i = request->start; i < request->end; ++i) {
            hash_table->table[n] = hash(&i);
            ++n;
        }

        u_int64_t answer = htobe64(compare(hash_table, request));

        // copy server message in the buffer and send that buffer to client
        bzero(buff, MAX);
        write(connfd, &answer, RESPONSE_PACKET_SIZE);

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            inf_loop = 0;
        }

        // We free the request
        destroy_request(request);
        request = NULL;
        free(request);

        // We free the hash_table
        destroy_hash_table(hash_table);
        hash_table = NULL;
        free(hash_table);
    }
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
    // convert from long
    int port = tmp;


    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
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
        fprintf(stderr, "Programm interrupted by an error of number: %d\n", err_chat);
        exit(0);
    }

    // After chatting close the socket
    close(sockfd);
    return 0;
}
