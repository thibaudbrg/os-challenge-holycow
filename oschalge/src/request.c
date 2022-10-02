#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "request.h" // Include itself for more security
#include "messages.h"

Request *create_empty_request(void) {
    Request *output = NULL;
    output = malloc(sizeof(Request));
    if (output != NULL) {
        output->hash = calloc(SIZE_HASH, sizeof(uint8_t));
        if (output->hash != NULL) {
            output->p = 0;
            output->start = 0;
            output->end = 0;
            printf("Request created successfully!\n");
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

        /*if (output == NULL) {
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

        return output;*/

        output->hash = all_bytes;
        uint64_t *start = all_bytes + PACKET_REQUEST_START_OFFSET;
        output->start = htobe64(*start);
        uint64_t *end = all_bytes + PACKET_REQUEST_END_OFFSET;
        output->end = htobe64(*end);
        output->p = all_bytes + PACKET_REQUEST_PRIO_OFFSET;

        return output;


    } else {
        return NULL;
    }
}