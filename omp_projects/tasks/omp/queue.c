#include <stdlib.h>
#include "sudoku_seriell.h"

void intEnqueue(num_queue** head, int pid) {
    num_queue* new_node = (num_queue*)malloc(sizeof(num_queue));
    if (!new_node) return;

    new_node->pid = pid;
    new_node->next = *head;

    *head = new_node;
}

int intDequeue(num_queue** head, int freeFirst) {
    num_queue* current, * prev = NULL;
    int* retval = -1;

    if (*head == NULL) return -1;

    current = *head;

    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retval = current->pid;
    if (freeFirst == 1)
    {
        free(current);

        if (prev)
            prev->next = NULL;
        else
            *head = NULL;
    }
    return retval;
}