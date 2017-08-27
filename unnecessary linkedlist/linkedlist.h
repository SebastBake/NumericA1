/***************************************************************************
 *
 *   File        : linkedlist.h
 *   Student Id  : 757931
 *   Name        : Sebastian Baker
 *
 ***************************************************************************/

#include <stdio.h>

#ifndef LINKEDLIST_H

/* node type */
typedef struct listNode listNode_t;
struct listNode {
    void* data;
    listNode_t* next;
};

/* linked list type */
typedef struct {
    int num_elements;
    listNode_t* head;
    void (*del)(void*);
} list_t;

void list_free(list_t* list);
list_t* list_new(void (*delfunc)(void*));
void* list_pop(list_t* list);
void list_push(list_t* list, void* d);

#endif