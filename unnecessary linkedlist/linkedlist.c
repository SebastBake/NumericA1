/***************************************************************************
 *
 *   File        : linkedlist.c
 *   Student Id  : 757931
 *   Name        : Sebastian Baker
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linkedlist.h"

/* process all elements in the list */
void list_process_all(listNode_t* p, void (*process)(void*)) {
    while (p) {
        process(p->data);
        p = p->next;
    }
}

/* remove node at the front of the list */
void* list_pop(list_t* list) {
    assert(list != NULL);
    assert(list->head != NULL);
    assert(list->num_elements > 0);
    
    listNode_t* old = list->head;
    list->head = list->head->next;
    void* d = old->data;
    free(old);

    list->num_elements--;
    if (list->num_elements == 0) {
        list->head = NULL;
    }

    return d;
}

/* add node add the front of the list */
void list_push(list_t* list, void* d) {
    assert(list != NULL);
    listNode_t* new = (listNode_t*)malloc(sizeof(listNode_t));
    assert(new);
    new->data = d;
    new->next = list->head;
    list->head = new;
    list->num_elements++;
}

/* create a new linked list structure */
list_t* list_new(void (*delfunc)(void*)) {
    list_t* list;
    list = (list_t*)malloc(sizeof(list_t));
    assert(list != NULL);
    list->head = NULL;
    list->num_elements = 0;
    list->del = delfunc;
    return list;
}

/* free all memory associated with a list */
void list_free(list_t* list) {
    assert(list != NULL);
    while (list->num_elements) {
        void* elem = list_pop(list);
        list->del(elem);
    }
    free(list);
}
