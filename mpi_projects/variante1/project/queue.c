#include <stdlib.h>
#include "sudoku_seriell.h"

void enqueue(grids_queue** head, struct grid val) {
    grids_queue *new_node = (grids_queue*)malloc(sizeof(grids_queue));
    if (!new_node) return;

    new_node->item = val;
    new_node->next = *head;

    *head = new_node;
}

struct grid dequeue(grids_queue** head) {
    grids_queue *current, * prev = NULL;
    struct grid retval = emptyGrid;

    if (*head == NULL) return emptyGrid;

    current = *head;
    
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retval = current->item;
    free(current);

    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    return retval;
}

void intEnqueue(processes_queue** head, int pid) {
    processes_queue* new_node = (processes_queue*)malloc(sizeof(processes_queue));
    if (!new_node) return;

    new_node->pid = pid;
    new_node->next = *head;

    *head = new_node;
}

int intDequeue(processes_queue** head) {
    processes_queue* current, * prev = NULL;
    int* retval = -1;

    if (*head == NULL) return -1;

    current = *head;

    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retval = current->pid;
    free(current);

    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    return retval;
}