#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "request.h" // Include itself for more security
#include "messages.h"


void print_SHA3(const unsigned char *SHA) {
    if (SHA != NULL) {
        for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            fprintf(stderr, "%02x", SHA[i]);
        }
        //putchar('\n');
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

// getRequest now takes a pointer and return a pointer
Request *getRequest(int *p_connfd) {
    //unsigned char buff[PACKET_REQUEST_SIZE];
    unsigned char* buff = calloc(PACKET_REQUEST_SIZE, sizeof(char));
    if (buff != NULL){
        // read the message from client and copy it in buffer
        size_t length = read(*p_connfd, buff, PACKET_REQUEST_SIZE);
        if (length != PACKET_REQUEST_SIZE) {
            fprintf(stderr, "ERROR: Unable to read %d elements, read only %zu elements: ", PACKET_REQUEST_SIZE,
                    length);
            perror(NULL);
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