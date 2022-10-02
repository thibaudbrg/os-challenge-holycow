#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "request.h" // Include itself for more security
#include "messages.h"

Request *create_empty_request(void) {
    Request *output = NULL;
    output = malloc(sizeof(Request));
    if (output != NULL) {
        output->hash = NULL;
        output->p = 0;
        output->start = 0;
        output->end = 0;
        printf("Request created successfully!\n");
        return output;

    }
    return NULL;
}

Request *getRequest(const unsigned char *all_bytes, size_t length) {
    if (all_bytes != NULL) {
        Request *output = create_empty_request();

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