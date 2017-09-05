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
#define BST_BALANCE_A 2
#define BST_BALANCE_B 1
#define RESULTS_LEN 128
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define EPS 0.00001

/* node type */
// Note: Each node is part of any number of binary trees, a tree is formed for
// each of the data entries in the csv, so that in the case of this assignment, 
// there would be trees for x,y,u and v
typedef struct NodeStruct node_t;
struct NodeStruct {
	float* d; // data
	int* depth_L; // depth of the left subtree
	int* depth_R; // depth of the right subtree
	node_t** parent; // Pointers to parents
	node_t** left; // Pointers to left subtrees
	node_t** right; // Pointers to right subtrees
};

/* bst type */
typedef struct {
	int numNodes;
	int dim; // dimension of the tree, eg: 4 for {'x','y','u','v'}
	node_t** root;
	char* key; // In this case the key is: {'x','y','u','v'}
} bst_t;

/* Results type */
// Note: Check function returns 1 when a new data point should be inserted into
// the results. I also use the check function to do other stuff, like keep track
// of the maximum value in the results.

typedef struct { // Used to define a search
	double lo, hi;
} resultsFilter_t;

typedef struct ResultsStruct results_t;
struct ResultsStruct {
	int arrLen, numEl;
	int dim;
	resultsFilter_t* filter;
	int (*check)(float*, results_t*);
	float** arr; // results of the search appear in this array
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
int res_filterBoundExclude(float* d, results_t* res);
int res_filterBoundInclude(float* d, results_t* res);

#endif