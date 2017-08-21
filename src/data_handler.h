/***************************************************************************
 *
 *   File        : data_handler.c
 *   Student Id  : 757931
 *   Name        : Sebastian Baker
 *
 ***************************************************************************/
#include <stdio.h>

#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#define FILE_READONLY "r"
#define PARSE_FINISHED 1
#define NEWLINE '\n'
#define COMMA ','

/* node type */
typedef struct node node_t;

struct node {
    float* d;
    node_t* left;
    node_t* right;
};

/* bst type */
typedef struct {
    int numNodes;
    int dim;
    char* key;
    node_t* root;
} bst_t;

bst_t* parseFlowFile(char* filename);
int parseFirstLine(bst_t* bst, FILE* fp);
int parseFlowFileLine(bst_t* bst, FILE* fp);

bst_t* bst_newTree();
node_t* bst_newNode();
void bst_insertNode(bst_t* bst, node_t* node);
void bst_deleteNode(bst_t* bst, node_t* node);
node_t* bst_searchRange(bst_t* bst, float loBound, float upBound, int dataCode);

#endif