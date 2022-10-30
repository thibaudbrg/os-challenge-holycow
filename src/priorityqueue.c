#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include "priorityqueue.h" // Include itself for more security
#include "request.h"
#include "messages.h"

node_t *head = NULL;
node_t *tail = NULL;

void print_SHA2(const unsigned char *SHA) {
    if (SHA != NULL) {
        for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            printf("%02x", SHA[i]);
        }
        putchar('\n');
    }
}

void enqueue(int *connfd) {
    node_t *newnode = malloc(sizeof(node_t));
    newnode->connfd = connfd;
    newnode->next = NULL;

    newnode->request = getRequest(*connfd);

    //print_SHA2(newnode->request->hash);
    //print_SHA2(newnode->request->hash);

    // Special case: head has less priority than newNode
    // so place it in front and change the head
    if (head == NULL || newnode->request->p > head->request->p) {
        newnode->next = head;
        head = newnode;
    } else {
        // Traverse the list and find a position to insert the newNode
        node_t *start = head;
        while (start->next != NULL && start->next->request->p > newnode->request->p) {
            start = start->next;
        }
        // Either at the end or at the required position
        newnode->next = start->next;
        start->next = newnode;
    }

    print_queue();
}

// Returns NULL is the queue is empty
// Returns the pointer to the connfd with the highest priority, if there is one to get
node_t *dequeue(void) {
    if (head != NULL) {
        node_t *result = head;
        head = head->next;
        print_queue();
        return result;
    }
    return NULL;
}

void destroy_node(node_t *node) {
    if (node != NULL) {
        close(*node->connfd);
        free(node->connfd);
        node->connfd = NULL;

        destroy_request(node->request);
        free(node->request);
        node->request = NULL;

    }
}

void print_queue(void) {
    if (head == NULL) {
        printf("The queue is empty\n");
    } else {
        int pos = 0; // head == 0;
        printf("i'm here\n");
        node_t *start = head;
        printf("                                       //=========HEAD=========\\\\\n");
        while (start->next != NULL) {
            printf("%3d --> prior: %2" PRIu8 " connfd: %3d  ||||  hash: ", pos, start->request->p, *start->connfd);
            print_SHA2(start->request->hash);
            start = start->next;
            ++pos;
        }
        printf("                                       //=========TAIL=========\\\\\n");
    }
}