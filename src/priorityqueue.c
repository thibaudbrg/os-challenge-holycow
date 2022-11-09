#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include "priorityqueue.h" // Include itself for more security
#include "request.h"

Queue *createQueue(void) {
    Queue *queue = malloc(sizeof(Queue));
    queue->head = NULL;
    queue->size = 0;
    return queue;
}

node_t *createNode(int * p_connfd) {
    node_t *newnode = malloc(sizeof(node_t));
    newnode->request = getRequest(p_connfd);
    newnode->connfd = p_connfd;
    newnode->next = NULL;
    return newnode;
}

void enqueue(int *p_connfd, Queue *queue) {
    node_t *newnode = createNode(p_connfd);

    // Special case: head has less priority than newNode
    // so place it in front and change the head
    if (queue->head == NULL || newnode->request->p > queue->head->request->p) {
        newnode->next = queue->head;
        queue->head = newnode;
    } else {
        // Traverse the list and find a position to insert the newNode
        node_t *start = queue->head;
        while (start->next != NULL && start->next->request->p > newnode->request->p) {
            start = start->next;
        }
        // Either at the end or at the required position
        newnode->next = start->next;
        start->next = newnode;
    }
    ++queue->size;

    //print_queue(queue);
}

// Returns NULL is the queue is empty
// Returns the pointer to the connfd with the highest priority, if there is one to get
node_t *dequeue(Queue *queue) {
    if (queue->size != 0) {
        node_t *result = queue->head;
        queue->head = queue->head->next;
        --queue->size;
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

void print_queue(Queue const * const queue) {
    if (queue->size == 0) {
        printf("The queue is empty\n");
    } else {
        int pos = 0; // head == 0;
        node_t *start = queue->head;
        printf("                                       //=========HEAD=========\\\\\n");
        while (start->next != NULL) {
            printf("%3d --> prior: %2" PRIu8 " connfd: %3d  ||||  hash: ", pos, start->request->p, *start->connfd);
            print_SHA(start->request->hash);
            start = start->next;
            ++pos;
        }
        printf("                                       //=========TAIL=========\\\\\n");
        putchar('\n');
    }
}