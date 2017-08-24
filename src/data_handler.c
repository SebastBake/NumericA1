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

// Private functions 
// 		- deal directly with nodes should be private and/or
// 		- are used to balance the tree and/or
//		- are recursive

static node_t* bst_newNode(int dim, float* data);
static void bst_freeNode(node_t* node);
static void bst_insertNode_Itr(bst_t* bst, node_t* newNode, int dataIndex);
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
static int bst_printTree_Rec(
	bst_t* bst, node_t* root, int dataIndex, FILE* stream);
static int bst_freeTree_Rec(bst_t* bst, node_t* root, int dataIndex);
static void bst_search_Rec( node_t* node, results_t *f);




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *  PARSING FUNCTIONS */

// Returns bst containing data from the flow file
bst_t* parseFlowFile(char *filename) {

	assert(filename!=NULL);
	
	FILE* fp = fopen(filename, FILE_READ);
	assert(fp != NULL);
	
	bst_t* bst = parseFlowFileFirstLine(fp);
	while (parseFlowFileDataLine(bst, fp) != PARSE_DONE) {
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

	return bst_newTree(i, key);
}

// Parses a data entry from flow file, inserts it into bst
int parseFlowFileDataLine(bst_t* bst, FILE* fp) {

	assert(bst!=NULL);
	assert(fp!=NULL);
	
	// Store the data in an array d
	float* data = (float*)malloc(bst->dim*sizeof(float));
	assert(data != NULL);
	
	// Read the line into array d
	int i = 0, read=0;
	for (i=0; i < bst->dim; i++) {
		read += fscanf(fp, "%f,", &(data[i]) );
	}

	// Decide whether to insert data, finish parsing, or detect an error
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
		bst_insertNode_Itr(bst,newNode,i);
	}

	bst->numNodes++;
}

// Prints the key of the bst 
void bst_printKey(bst_t* bst, FILE* stream) {
	
	assert(bst!=NULL);

	int i = 0;
	for (i=0; i < bst->dim; i++) {
		fprintf(stream, "%c", bst->key[i] );
		if (i < bst->dim-1) { fprintf(stream, ","); }
	}
	fprintf(stream, "\n");
}

// Prints all of the items in order of low to high
void bst_printTree(bst_t* bst, int dataIndex, FILE* stream) {

	assert(bst!=NULL);
	assert(dataIndex < bst->dim);

	bst_printKey(bst, stream);	
	bst_printTree_Rec(bst, bst->root[dataIndex], dataIndex, stream);
}

// Prints a data entry
void bst_printData(bst_t* bst, float* data, FILE* stream) {

	assert(bst!=NULL);
	assert(data!=NULL);

	int i = 0;
	for (i=0; i < bst->dim; i++) {
		fprintf(stream, "%.6f", data[i] );
		if (i < bst->dim-1) { fprintf(stream, ",");}
	}
	fprintf(stream, "\n");
}

// Searches a bst for data entries beween lo and hi
// The check function must return 1 or 0 to insert or not insert an item
// The check function can also modify the results since it has access to them
results_t* res_search(
	bst_t* bst, resultsFilter_t* filter, int (*check)(float*, results_t*)
) {

	assert(check!=NULL);
	assert(filter!=NULL);

	float** arr = (float**)calloc(RESULTS_LEN, sizeof(float*));
	assert(arr!=NULL);

	results_t* res = (results_t*)malloc(sizeof(results_t));
	assert(res!=NULL);

	res->arrLen = RESULTS_LEN;
	res->numEl = 0;
	res->dim = bst->dim;
	res->filter = filter;
	res->check = check;
	res->arr = arr;

	bst_search_Rec(bst->root[BST_INDEX], res);
	
	return res;
}

// Frees a results structure
void res_free(results_t* res) {
	free(res->arr);
	free(res);
}

// Inserts a data entry into a results structure (provided it is within the rules
// defined by the structure)
void res_insert(results_t* res, float* d) {

	// check that item is insertable
	int insert = 1, lessThanLo = 0, moreThanHi = 0;
	int i=0;
	for (i=0;i<res->dim;i++) {
		lessThanLo = d[i] < (res->filter[i]).lo;
		moreThanHi = d[i] > (res->filter[i]).hi;

		if (lessThanLo || moreThanHi ) {
			insert = 0;
		}
	}
	assert(insert == 1);

	if(insert && res->check(d,res)) {

		// Extend array if necessary
		if (res->numEl >= res->arrLen) {
			res->arrLen += RESULTS_LEN;
			res->arr = (float**)realloc(res->arr, res->arrLen * sizeof(float*));
			assert(res->arr != NULL);
		}

		res->arr[res->numEl] = d;
		res->numEl++;
	}
}

// Removes a data entry from a results structure
void res_remove(results_t* res, int index) {
	
	res->arr[index] = NULL;
	int i = index;
	for (i=index; i<res->numEl-1; i++) {
		res->arr[i] = res->arr[i+1];
	}
	res->arr[res->numEl-1] = NULL;
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

// Iteratively insert a node into a bst
static void bst_insertNode_Itr(bst_t* bst, node_t* newNode, int dataIndex) {

	assert(bst != NULL);
	assert(newNode != NULL);

	// Keep track of the parent node for use when balancing
	node_t* parent = NULL;

	// The place where the newNode may be inserted
	node_t* insertHere = bst->root[dataIndex];

	// Iteratively determine where to insert the node
	int insertDir = 0; // Insertion will occur LEFT or RIGHT of the parent node
	while (insertHere != NULL) {

		// Balance the tree
		if (parent!=NULL){
			bst_balance(parent, insertHere, dataIndex, insertDir); 
		}

		// Go left or right
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

	// Insert the node
	if (insertDir == LEFT) {
		parent->left[dataIndex] = newNode;
	} else if (insertDir == RIGHT) {
		parent->right[dataIndex] = newNode;
	} else {
		bst->root[dataIndex] = newNode;
	}
}

// Recursively print tree (in order traverse), returns number of items printed
static int bst_printTree_Rec(
	bst_t* bst, node_t* root, int dataIndex, FILE* stream) {

	assert(bst != NULL);
	assert(dataIndex < bst->dim);

	int numPrinted = 0;
	node_t* left = root->left[dataIndex];
	node_t* right = root->right[dataIndex];

	if (root!=NULL) {
		numPrinted++;
		numPrinted += bst_printTree_Rec(bst, left, dataIndex, stream);
		bst_printData(bst, root->d, stream);
		numPrinted += bst_printTree_Rec(bst, right, dataIndex, stream);
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
static void bst_search_Rec(node_t* node, results_t *f) {

	assert(f!=NULL);

	if (node!=NULL) {
		
		bst_search_Rec(node->left[BST_INDEX], f);

		// check that item is insertable
		int insert = 1, lessThanLo = 0, moreThanHi = 0;
		int i=0;
		for (i=0;i<f->dim;i++) {
			lessThanLo = node->d[i] < (f->filter[i]).lo;
			moreThanHi = node->d[i] > (f->filter[i]).hi;
			if (lessThanLo || moreThanHi ) { insert = 0; }
		}
		if (insert) { res_insert(f, node->d); }

		bst_search_Rec(node->right[BST_INDEX], f);
	}
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
	node_t* parent, node_t* node_a, int dataIndex, int parentDirection){
	

	assert(node_a!=NULL);
	assert(node_a->right[dataIndex]!=NULL);

	bst_leftOuterRot(node_a, node_a->right[dataIndex], dataIndex, RIGHT);
	bst_rightOuterRot(parent, node_a, dataIndex, parentDirection);
}
