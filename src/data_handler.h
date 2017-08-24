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

#define FILE_READ "r"
#define PARSE_DONE 1
#define CSV_NEWLINE '\n'
#define CSV_COMMA ','
#define BST_INDEX 0
#define BST_BALANCE_THRESH 2
#define RESULTS_LEN 10
#define LEFT -1
#define RIGHT 1


/* node type */
typedef struct node node_t;

struct node {
    float* d;
    int* depthDiff;
    node_t** left;
    node_t** right;
};

/* bst type */
typedef struct {
    int numNodes;
    int dim;
    node_t** root;
    char* key;
} bst_t;

/* Results */
typedef struct {
	int len, n, i;
	float lo, hi;
	float** arr;
} results_t;

bst_t* parseFlowFile(char* filename); // works
bst_t* parseFlowFileFirstLine(FILE* fp); // works
int parseFlowFileDataLine(bst_t* bst, FILE* fp); // works

bst_t* bst_newTree(int dim, char* key); // works
int bst_freeTree(bst_t* bst);

void bst_printData(bst_t* bst, float* data); // works
void bst_printTree(bst_t* bst, int dataIndex); // works

void bst_insertData(bst_t* bst, float* data); // works
results_t bst_searchRange(bst_t* bst, float lo, float hi, int dataIndex);

#endif