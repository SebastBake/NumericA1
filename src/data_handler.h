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
#define DEFAULT_BST_FREE_ORDER_INDEX 0
#define FOUND_LEN 10

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
    node_t** root;
    char* key;
} bst_t;

/* Results */
typedef struct Results_Struct {
	int len, n, i;
	double lo, hi;
	double** arr;
} results_t;

bst_t* parseFlowFile(char* filename); // works
bst_t* parseFirstLine(FILE* fp); // works
int parseFlowFileLine(bst_t* bst, FILE* fp); // works

bst_t* bst_newTree(int dim, char* key); // works
void bst_freeTree(bst_t* bst);

void bst_printData(bst_t* bst, double* data); // works
void bst_printTree(bst_t* bst, int dataIndex); // works

void bst_insertData(bst_t* bst, double* data); // works
results_t bst_searchRange(bst_t* bst, double loBound, double upBound, int dataIndex);

#endif