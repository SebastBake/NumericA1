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
#define MAX_KEY_SIZE 99
#define NEWLINE '\n'
#define COMMA ','

/* node type */
typedef struct node node_t;

struct node {
    double* d;
    node_t** left;
    node_t** right;
};

/* bst type */
typedef struct {
    int numNodes;
    int dim;
    char* key;
    node_t** root;
} bst_t;

bst_t* parseFlowFile(char* filename);
bst_t* parseFirstLine(FILE* fp);
int parseFlowFileLine(bst_t* bst, FILE* fp);

bst_t* bst_newTree();
node_t* bst_newNode();

void bst_freeTree(bst_t* bst);

void bst_printData(bst_t* bst, double* data);
void bst_printTree(bst_t* bst, int dataIndex);

void bst_insertData(bst_t* bst, double* data);
float** bst_searchRange(bst_t* bst, double loBound, double upBound, int dataIndex);

#endif