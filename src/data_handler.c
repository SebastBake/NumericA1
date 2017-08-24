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

// Functions which deal directly with nodes should be private
static node_t* bst_newNode(int dim, float* data);
static void bst_freeNode(node_t* node);
static void bst_insertNode_Itr(bst_t* bst, node_t* newNode, int dataIndex);

// Recursive functions should be private 
//static void bst_insertNode_Rec(node_t* root, node_t* newNode, int dataIndex);
static int bst_printTree_Rec(bst_t* bst, node_t* root, int dataIndex);
static int bst_freeTree_Rec(bst_t* bst, node_t* root, int dataIndex);
static int bst_searchRange_Rec(node_t* node, results_t *f);

// Balancing functions should be private
static void bst_balance(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection);
static void bst_leftOuterRot(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection);
static void bst_rightOuterRot(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection);
static void bst_leftInnerRot(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection);
static void bst_rightInnerRot(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection);




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *  PARSING FUNCTIONS */

// Returns bst containing data from the flow file
bst_t* parseFlowFile(char *filename) {

	assert(filename!=NULL);
	
	FILE* fp = fopen(filename, FILE_READ);
	assert(fp != NULL);
	
	bst_t* bst = parseFlowFileFirstLine(fp);
	
	//int i = 0;
	while (parseFlowFileDataLine(bst, fp) != PARSE_DONE) {
		//i++;
		//if (i%5000 == 0) { printf("Parsed %d lines\n", i); }
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
		if (tmpc == CSV_COMMA) { continue; }
		if (tmpc == CSV_NEWLINE) { break; }

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
	float* data = (float*)malloc(bst->dim*sizeof(float));
	assert(data != NULL);
	
	// Read the line
	int i = 0, read=0;
	for (i=0; i < bst->dim; i++) {
		read += fscanf(fp, "%f,", &(data[i]) );
	}

	// Decide whether to insert data, finish parsing, or detect a file error
	char endCheck = fgetc(fp);
	if ( read==(bst->dim) && endCheck == CSV_NEWLINE) {
		bst_insertData(bst, data);
		return !PARSE_DONE;
	} else if ( endCheck == EOF) {
		free(data);
		return PARSE_DONE;
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

	int freed = bst_freeTree_Rec(bst, bst->root[BST_INDEX], BST_INDEX);
	assert(bst->numNodes == 0);
	free(bst->root);
	free(bst->key);
	free(bst);

	return freed;
}

// Inserts a data array into the bst as a node
void bst_insertData(bst_t* bst, float* data) {
	
	assert(bst!=NULL);
	assert(data!=NULL);

	node_t* newNode = bst_newNode(bst->dim, data);
	
	int i=0;
	for (i=0; i<bst->dim; i++) {
		// if (bst->root[i] == NULL) {
		// 	bst->root[i] = newNode;
		// } else {
		// 	bst_insertNode_Rec(bst->root[i], newNode, i);
		// }
		bst_insertNode_Itr(bst,newNode,i);
	}

	bst->numNodes++;
}

// Returns data items with a value between lo and hi, from a bst
results_t bst_searchRange(bst_t* bst, float lo, float hi, int dataIndex) {

	assert(bst!=NULL);
	assert(lo<hi);
	assert(dataIndex<bst->dim);

	results_t found;
	found.len = RESULTS_LEN;
	found.n = 0;
	found.i = dataIndex;
	found.lo = lo;
	found.hi = hi;
	found.arr = (float**)calloc(found.len, sizeof(float*));
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
void bst_printData(bst_t* bst, float* data) {

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
static node_t* bst_newNode(int dim, float* data) {
	
	assert(data != NULL);
	assert(dim>0);

	node_t* newNode = (node_t*)malloc(sizeof(node_t));
	newNode->d = data;
	newNode->depthDiff = (int*)calloc(dim, sizeof(int*));
	newNode->left = (node_t**)calloc(dim, sizeof(node_t*));
	newNode->right = (node_t**)calloc(dim, sizeof(node_t*));

	assert(newNode != NULL);
	assert(newNode->depthDiff != NULL);
	assert(newNode->left != NULL);
	assert(newNode->right != NULL);

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

/* //Recursively insert a node (Too slow - not used)
static void bst_insertNode_Rec(node_t* root, node_t* newNode, int dataIndex) {

	assert(root != NULL);
	assert(newNode != NULL);

	if ( newNode->d[dataIndex] < root->d[dataIndex]) {
		root->depthDiff[dataIndex]--;
		if (root->left[dataIndex] != NULL) {
			bst_insertNode_Rec(root->left[dataIndex], newNode, dataIndex);
			bst_balance(root, root->left[dataIndex], dataIndex, LEFT);
			
		} else { 
			root->left[dataIndex] = newNode;
		}

	} else {
		root->depthDiff[dataIndex]++;
		if (root->right[dataIndex] != NULL) {
			bst_insertNode_Rec(root->right[dataIndex], newNode, dataIndex);
			bst_balance(root, root->right[dataIndex], dataIndex, RIGHT);
			
		} else { 
			root->right[dataIndex] = newNode;
		}
	}
}*/

// Iteratively insert a node
static void bst_insertNode_Itr(bst_t* bst, node_t* newNode, int dataIndex) {

	assert(bst != NULL);
	assert(newNode != NULL);

	node_t* parent = NULL;
	node_t* insertHere = bst->root[dataIndex];

	if (insertHere == NULL) {
		bst->root[dataIndex] = newNode;
		return;
	}

	int insertDir = 0;
	while (insertHere != NULL) {

		if (parent!=NULL){
			bst_balance(parent,insertHere, dataIndex, insertDir); 
		}

		if ( newNode->d[dataIndex] < insertHere->d[dataIndex]) {

			insertDir = LEFT;
			insertHere->depthDiff[dataIndex]--;
			parent = insertHere;
			insertHere = insertHere->left[dataIndex];
		} else {

			insertDir = RIGHT;
			insertHere->depthDiff[dataIndex]++;
			parent = insertHere;
			insertHere = insertHere->right[dataIndex];
		}
	}

	if (insertDir == LEFT) {
		parent->left[dataIndex] = newNode;
	} else if (insertDir == RIGHT) {
		parent->right[dataIndex] = newNode;
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
static int bst_freeTree_Rec(bst_t* bst, node_t* root, int dataIndex) {

	int numFreed = 0;
	if (root!=NULL) {
		numFreed++;
		numFreed += bst_freeTree_Rec(bst, root->left[dataIndex], dataIndex);
		numFreed += bst_freeTree_Rec(bst, root->right[dataIndex], dataIndex);
		bst_freeNode(root);
		bst->numNodes--;
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
				f->len += RESULTS_LEN;
				f->arr = (float**)realloc(f->arr, f->len * sizeof(float*));
				assert(f->arr != NULL);
			}
			f->arr[f->n] = node->d;
			f->n++;

			searched += bst_searchRange_Rec(node->right[f->i], f);
		}
	}

	return searched;
}

/* * * * * * * * * * * * * * * * * * * * * * *  PRIVATE FUNCTIONS (BALANCING) */

// Correct imbalances in the tree
static void bst_balance(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection) {	

	node_t* node_b = NULL;

	if (node_a->depthDiff[dataIndex] > BST_BALANCE_THRESH) {
		
		node_b = node_a->right[dataIndex];

		if (node_b->depthDiff[dataIndex] > 0) {
			bst_rightOuterRot(parent, node_a, dataIndex, parentDirection);
		} else {
			bst_rightInnerRot(parent, node_a, dataIndex, parentDirection);
		}

	} else if (node_a->depthDiff[dataIndex] < -BST_BALANCE_THRESH){
		
		node_b = node_a->left[dataIndex];

		if (node_b->depthDiff[dataIndex] < 0) {
			bst_leftOuterRot(parent, node_a, dataIndex, parentDirection);
		} else {
			bst_leftInnerRot(parent, node_a, dataIndex, parentDirection);
		}
	}
}

// Left outer rotation balancing
static void bst_leftOuterRot(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection) {

	assert(node_a!=NULL);
	assert(node_a->left[dataIndex]!=NULL);
	assert(parent!=NULL);
	node_t* node_b = node_a->left[dataIndex];

	node_a->depthDiff[dataIndex] = 0;
	node_b->depthDiff[dataIndex] = 0;
	
	// a.left points to b.right
	node_a->left[dataIndex] = node_b->right[dataIndex];
	
	// b.right points to a
	node_b->right[dataIndex] = node_a;
	
	// parent now points to b (previously a.left)
	if (parentDirection == LEFT) {
		parent->left[dataIndex] = node_b;
	} else {
		parent->right[dataIndex] = node_b;
	}
}

// Right outer rotation balancing
static void bst_rightOuterRot(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection) {
	
	assert(parent!=NULL);
	assert(node_a!=NULL);
	assert(node_a->right[dataIndex]!=NULL);
	node_t* node_b = node_a->right[dataIndex];

	node_a->depthDiff[dataIndex] = 0;
	node_b->depthDiff[dataIndex] = 0;

	// a.right points to b.left
	node_a->right[dataIndex] = node_b->left[dataIndex];
	
	// b.left points to a
	node_b->left[dataIndex] = node_a;
	
	// parent now points to b (previously a.right)
	if (parentDirection == LEFT) {
		parent->left[dataIndex] = node_b;
	} else {
		parent->right[dataIndex] = node_b;
	}
}

// Left inner rotation balancing
static void bst_leftInnerRot(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection){

	assert(node_a!=NULL);
	assert(node_a->left[dataIndex]!=NULL);

	bst_rightOuterRot(node_a, node_a->left[dataIndex], dataIndex, LEFT);
	bst_leftOuterRot(parent, node_a, dataIndex, parentDirection);
}

// Right inner rotation balancing
static void bst_rightInnerRot(
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection) {

	assert(node_a!=NULL);
	assert(node_a->right[dataIndex]!=NULL);

	bst_leftOuterRot(node_a, node_a->right[dataIndex], dataIndex, RIGHT);
	bst_rightOuterRot(parent, node_a, dataIndex, parentDirection);
}
