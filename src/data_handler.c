/***************************************************************************
 *
 *   File        : data_handler.c
 *   Student Id  : 757931
 *   Name        : Sebastian Baker
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "data_handler.h"

/* * * * * * * * * * * * * * * * * * * * * * *  PRIVATE FUNCTION DECLARATIONS */

// Recursive functions and functions which deal directly with nodes should not 
// be public.
static node_t* bst_newNode(int dim, double* data);
static void bst_freeNode(node_t* node);
static void bst_insertNode_Rec(node_t* root, node_t* newNode, int dataIndex);
static int bst_printTree_Rec(bst_t* bst, node_t* root, int dataIndex);
static int bst_freeTree_Rec(node_t* root, int dataIndex);
static int bst_searchRange_Rec(node_t* node, results_t *f);




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *  PARSING FUNCTIONS */

// Returns bst containing data from the flow file
bst_t* parseFlowFile(char *filename) {

	assert(filename!=NULL);
	
	FILE* fp = fopen(filename, FILE_READONLY);
	assert(fp != NULL);
	
	bst_t* bst = parseFlowFileFirstLine(fp);
	
	while (parseFlowFileDataLine(bst, fp) != PARSE_FINISHED) {
	}

	fclose(fp);

	return bst;
}

// Parses first line of flow file to get:
//  1. key (eg: {'x','y','u','v'}) and
//  2. number of dimensions (eg: 4 data entries for each line in the csv)
// Uses the key and number of dimensions to generate an empty bst
bst_t* parseFlowFileFirstLine(FILE* fp) {
	
	assert(fp!=NULL);

	char* key = (char* )malloc( sizeof(char) );
	assert(key != NULL);

	char tmpc;
	int i = 0;
	while (1) {

		tmpc = fgetc(fp);
		if (tmpc == COMMA) { continue; }
		if (tmpc == NEWLINE) { break; }

		key = (char*)realloc( key, (i+2) * sizeof(char) );
		assert(key != NULL);
		key[i] = tmpc;
		i++;
	}

	bst_t* bst = bst_newTree(i, key);

	return bst;
}

// Parses a data entry from flow file, inserts it into bst
int parseFlowFileDataLine(bst_t* bst, FILE* fp) {

	assert(bst!=NULL);
	assert(fp!=NULL);
	
	// Store the data in an array
	double* data = (double*)calloc(bst->dim, sizeof(double));
	assert(data != NULL);
	
	// Read the line
	int i = 0, read=0;
	for (i=0; i < bst->dim; i++) {
		read += fscanf(fp, "%lf,", &(data[i]) );
	}

	// Decide whether to insert data, finish parsing, or detect a file error
	char endCheck = fgetc(fp);
	if ( read==(bst->dim) && endCheck == NEWLINE) {
		bst_insertData(bst, data);
		return !PARSE_FINISHED;
	} else if ( endCheck == EOF) {
		free(data);
		return PARSE_FINISHED;
	} else {
		printf("ERROR: File parsing failed, exiting...");
		exit(EXIT_FAILURE);
	}
}




/* * * * * * * * * * * * * * * * * * * * * * *  PUBLIC BST AND DATA FUNCTIONS */

// Generate a new empty bst
bst_t* bst_newTree(int dim, char* key) {

	assert(key!=NULL);
	
	bst_t* bst = (bst_t*)malloc(sizeof(bst_t));
	assert(bst != NULL);

	bst->numNodes = 0;
	bst->dim = dim;
	bst->key = key;
	bst->root = (node_t**)calloc( bst->dim, sizeof(node_t*) );
	assert(bst->root != NULL);

	return bst;
}

// Frees memory associated with a tree, including the data contained in the tree
int bst_freeTree(bst_t* bst) {

	assert(bst!=NULL);

	int freed = bst_freeTree_Rec(bst->root[BST_ORDER_INDEX], BST_ORDER_INDEX);
	assert(freed == bst->numNodes);
	
	free(bst->root);
	free(bst->key);
	free(bst);

	return freed;
}

// Inserts a data array into the bst as a node
void bst_insertData(bst_t* bst, double* data) {
	
	assert(bst!=NULL);
	assert(data!=NULL);

	node_t* newNode = bst_newNode(bst->dim, data);
	
	int i=0;
	for (i=0; i<bst->dim; i++) {
		if (bst->root[i] == NULL) {
			bst->root[i] = newNode;
		} else {
			bst_insertNode_Rec(bst->root[i], newNode, i);
		}
	}

	bst->numNodes++;
}

// Returns data items with a value between lo and hi, from a bst
results_t bst_searchRange(bst_t* bst, double lo, double hi, int dataIndex) {

	assert(bst!=NULL);
	assert(lo<hi);
	assert(dataIndex<bst->dim);

	results_t found;
	found.len = FOUND_LEN;
	found.n = 0;
	found.i = dataIndex;
	found.lo = lo;
	found.hi = hi;
	found.arr = (double**)calloc(found.len, sizeof(double*));
	assert(found.arr!=NULL);
	bst_searchRange_Rec(bst->root[found.i], &found);

	return found;
}

// Prints all of the items in order of low to high
void bst_printTree(bst_t* bst, int dataIndex) {

	assert(bst!=NULL);
	assert(dataIndex < bst->dim);
	bst_printTree_Rec(bst, bst->root[dataIndex], dataIndex);
}

// Prints a data entry
void bst_printData(bst_t* bst, double* data) {

	assert(bst!=NULL);
	assert(data!=NULL);

	printf("Node: ");
	int i = 0;
	for (i=0; i < bst->dim; i++) {
		printf("%c=%f ", bst->key[i], data[i] );
	}
	printf("\n");
}




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *  PRIVATE FUNCTIONS */

// Generates a new node
static node_t* bst_newNode(int dim, double* data) {
	
	node_t* newNode = (node_t*)malloc(sizeof(node_t));
	assert(newNode != NULL);

	newNode->left = (node_t**)calloc(dim, sizeof(node_t*));
	assert(newNode->left != NULL);

	newNode->right = (node_t**)calloc(dim, sizeof(node_t*));
	assert(newNode->right != NULL);

	assert(data != NULL);
	newNode->d = data;

	return newNode;
}

// Frees a node
static void bst_freeNode(node_t* node) {

	assert(node != NULL);
	assert(node->left != NULL);
	assert(node->right != NULL);
	assert(node->d != NULL);

	free(node->d);
	free(node->right);
	free(node->left);
	free(node);
}

// Recursively insert a node
static void bst_insertNode_Rec(node_t* root, node_t* newNode, int dataIndex) {

	assert(root != NULL);
	assert(newNode != NULL);

	if ( newNode->d[dataIndex] < root->d[dataIndex]) {
		if (root->left[dataIndex] != NULL) {
			bst_insertNode_Rec(root->left[dataIndex], newNode, dataIndex);
		} else {
			root->left[dataIndex] = newNode;
		}
	} else {
		if (root->right[dataIndex] != NULL) {
			bst_insertNode_Rec(root->right[dataIndex], newNode, dataIndex);
		} else {
			root->right[dataIndex] = newNode;
		}
	}
}

// Recursively print tree (in order traverse), returns number of items printed
static int bst_printTree_Rec(bst_t* bst, node_t* root, int dataIndex) {

	assert(bst != NULL);
	assert(dataIndex < bst->dim);

	int numPrinted = 0;
	if (root!=NULL) {
		numPrinted++;
		numPrinted += bst_printTree_Rec(bst, root->left[dataIndex], dataIndex);
		bst_printData(bst, root->d);
		numPrinted += bst_printTree_Rec(bst, root->right[dataIndex], dataIndex);
	}
	return numPrinted;
}

// Recursively free a tree (post order traverse), returns number of items freed
static int bst_freeTree_Rec(node_t* root, int dataIndex) {

	int numFreed = 0;
	if (root!=NULL) {
		numFreed++;
		numFreed += bst_freeTree_Rec(root->left[dataIndex], dataIndex);
		numFreed += bst_freeTree_Rec(root->right[dataIndex], dataIndex);
		bst_freeNode(root);
	}
	return numFreed;
}

// Recursively search a tree (in order traverse), returns number items searched
static int bst_searchRange_Rec(node_t* node, results_t *f) {

	assert(f!=NULL);

	int searched = 0;

	if (node!=NULL) {
		searched++;
		
		if ( node->d[f->i] < f->lo ) {
			searched += bst_searchRange_Rec(node->right[f->i], f);
		} else if (node->d[f->i] > f->hi ) {
			searched += bst_searchRange_Rec(node->left[f->i], f);
		} else {
			searched += bst_searchRange_Rec(node->left[f->i], f);
			
			// insert the item into the results structure f
			if (f->n >= f->len) {
				f->len += FOUND_LEN;
				f->arr = (double**)realloc(f->arr, f->len * sizeof(double*));
				assert(f->arr != NULL);
			}
			f->arr[f->n] = node->d;
			f->n++;

			searched += bst_searchRange_Rec(node->right[f->i], f);
		}
	}

	return searched;
}
