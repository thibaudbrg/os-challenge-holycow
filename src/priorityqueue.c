#include <stdlib.h>
#include <stdio.h>

#include "priorityqueue.h" // Include itself for more security

node_t *head = NULL;
node_t *tail = NULL;


void enqueue(int *client_socket, int p) {
    node_t *newnode = malloc(sizeof(node_t));
    newnode->connfd = client_socket;
    newnode->next = NULL;
    newnode->priority = p;

    // Special case: head has less priority than newNode
    // so place it in front and change the head
    if (newnode->priority > head->priority) {
        newnode->next = head;
        head = newnode;
    } else {
        // Traverse the list and find a position to insert the newNode
        node_t *start = head;
        while (start->next != NULL && start->next->priority > newnode->priority) {
            start = start->next;
        }
        // Either at the end or at the required position
        newnode->next = start->next;
        start->next = newnode;
    }
}

// Returns NULL is the queue is empty
// Returns the pointer to the connfd with the highest priority, if there is one to get
int *dequeue(void) {
    if (head != NULL) {
        int *result = head->connfd;
        node_t *temp = head;
        head = head->next;

        free(temp);
        temp = NULL;

        return result;
    }
    return NULL;
}