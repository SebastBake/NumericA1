/***************************************************************************
 *
 *   File        : data_handler.h
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
#define RESULTS_LEN 128
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* node type */
typedef struct NodeStruct node_t;
struct NodeStruct {
    float* d;
    int* depth_L;
    int* depth_R;
    node_t** parent;
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

/* Results type */
// check function returns 1 when a new data point should be inserted into the
// results.

typedef struct {
    float lo, hi;
} resultsFilter_t;

typedef struct ResultsStruct results_t;
struct ResultsStruct {
    int arrLen, numEl;
    int dim;
    resultsFilter_t* filter;
    int (*check)(float*, results_t*);
	float** arr;
};

// parse flow file
bst_t* parseFlowFile(char* filename);
bst_t* parseFlowFileFirstLine(FILE* fp);
int parseFlowFileDataLine(bst_t* bst, FILE* fp);

// build a tree
bst_t* bst_newTree(int dim, char* key);
void bst_insertData(bst_t* bst, float* data);
int bst_freeTree(bst_t* bst);

// print data
void bst_printKey(bst_t* bst, FILE* stream);
void bst_printData(int dim, float* data, FILE* stream);
int bst_printTree(bst_t* bst, int dataIndex, FILE* stream);

// search the tree
results_t* res_search(
    bst_t* bst, resultsFilter_t* filter, int (*check)(float*, results_t*));

void res_free(results_t* res);

#endif