#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <strings.h>
#include "request.h" // Include itself for more security
#include "messages.h"

void print_SHA(unsigned const char *SHA) {
    if (SHA != NULL) {
        for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            fprintf(stderr, "%02x", SHA[i]);
        }
        putchar('\n');
    }
}


Request *create_empty_request(void) {
    Request *output = NULL;
    output = malloc(sizeof(Request));
    if (output != NULL) {
        output->hash = calloc(SHA256_DIGEST_LENGTH, sizeof(uint8_t));
        if (output->hash != NULL) {
            output->p = 0;
            output->start = 0;
            output->end = 0;
            // printf("Request created successfully!\n");
            return output;
        }
    }
    return NULL;
}

Request *getRequest(int const *p_connfd) {
    unsigned char *buff = calloc(PACKET_REQUEST_SIZE, sizeof(char));
    if (buff != NULL) {
        bzero(buff,PACKET_REQUEST_SIZE);
        size_t length = read(*p_connfd, buff, PACKET_REQUEST_SIZE);
        if (length != PACKET_REQUEST_SIZE) {
            fprintf(stderr, "ERROR getRequest(): Unable to read %d elements, read only %zu elements.\n", PACKET_REQUEST_SIZE,
                    length);
            return NULL;
        }

        Request *request = create_empty_request();

        request->hash = (uint8_t *) (buff + PACKET_REQUEST_HASH_OFFSET);

        uint64_t *start = (uint64_t *) (buff + PACKET_REQUEST_START_OFFSET);
        request->start = htobe64(*start);

        uint64_t *end = (uint64_t *) (buff + PACKET_REQUEST_END_OFFSET);
        request->end = htobe64(*end);

        request->p = ((uint8_t *) (buff + PACKET_REQUEST_PRIO_OFFSET))[0];

        return request;

    }
    return NULL;
}

void destroy_request(Request *request) {
    if (request != NULL) {
        free(request->hash);
        request->hash = NULL;
    }
}