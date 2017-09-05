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
#include <float.h>
#include <string.h>
#include <assert.h>
#include "data_handler.h"

/* * * * * * * * * * * * * * * * * * * * * * *  PRIVATE FUNCTION DECLARATIONS */
// Private functions don't need to be visible to code outside this file

// Funcions dealing with nodes rather than data
static node_t* bst_newNode(int dim, float* data);
static void bst_freeNode(node_t* node);

// BST balancing functions
static void bst_balance( bst_t* bst, node_t* node_a, int dataIndex);
static node_t* bst_leftOuterRot(bst_t* bst, node_t* node_a, int dataIndex);
static node_t* bst_rightOuterRot(bst_t* bst, node_t* node_a, int dataIndex);
static node_t* bst_leftInnerRot(bst_t* bst, node_t* node_a, int dataIndex);
static node_t* bst_rightInnerRot(bst_t* bst, node_t* node_a, int dataIndex);
static int node_maxDepth(node_t* node, int dataIndex);
static void fixParentDepth(bst_t* bst, node_t* node_a, int dataIndex);

// Recursive helper functions
static int bst_printTree_Rec( node_t* n, int dataIndex, int dim, FILE* stream);
static int bst_freeTree_Rec(bst_t* bst, node_t* root, int dataIndex);
static void bst_insertNode_Rec(bst_t* bst, node_t* root, node_t* newNode, int dataIndex);

// Helper search functions
static void bst_search_Rec(node_t* node, results_t *f, int t);
static void res_insert(results_t* res, float* d);




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

	bst->root = (node_t**)calloc( dim , sizeof(node_t*) );
	assert(bst->root != NULL);

	bst->numNodes = 0;
	bst->dim = dim;
	bst->key = key;

	return bst;
}

// Frees memory associated with a tree, including the data
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
		if (bst->root[i] == NULL) {
			bst->root[i] = newNode;
		} else {
			bst_insertNode_Rec(bst, bst->root[i],newNode,i);
		}
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
int bst_printTree(bst_t* bst, int dataIndex, FILE* stream) {

	assert(bst!=NULL);
	assert(dataIndex < bst->dim);

	bst_printKey(bst, stream);	
	return bst_printTree_Rec(bst->root[dataIndex], dataIndex, bst->dim, stream);
}

// Prints a data entry
void bst_printData(int dim, float* data, FILE* stream) {

	assert(data!=NULL);

	int i = 0;
	for (i=0; i < dim; i++) {
		fprintf(stream, "%.6f", data[i] );
		if (i < dim-1) { fprintf(stream, ",");}
	}
	fprintf(stream, "\n");
}

// Searches a bst for data entries beween lo and hi
// The check function must return 1 or 0 to insert or not insert an item
// The check function can also modify the results since it has access to them
results_t* res_search(
	bst_t* bst, resultsFilter_t* filter, int (*check)(float*, results_t*) ) {

	assert(check!=NULL);
	assert(filter!=NULL);
	assert(bst != NULL);

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

	// Determine best filter to apply (the one with the most narrow bounds)
	float minDiff=FLT_MAX;
	int i=0, t=0;
	for (i=0; i<bst->dim; i++) {
		if ((res->filter[i].hi - res->filter[i].lo) < minDiff ) {
			minDiff = res->filter[i].hi - res->filter[i].lo;
			t = i;
		}
	}
	bst_search_Rec(bst->root[t],res,t);

	return res;
}

// Free a results structure
void res_free(results_t* res) {
	free(res->arr);
	free(res);
}




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *  PRIVATE FUNCTIONS */

// Generates a new node
static node_t* bst_newNode(int dim, float* data) {
	
	assert(data != NULL);
	assert(dim>0);

	node_t* newNode = (node_t*)malloc(sizeof(node_t));
	newNode->d = data;
	newNode->depth_R = (int*)calloc(dim, sizeof(int*));
	newNode->depth_L = (int*)calloc(dim, sizeof(int*));
	newNode->left = (node_t**)calloc(dim, sizeof(node_t*));
	newNode->right = (node_t**)calloc(dim, sizeof(node_t*));
	newNode->parent = (node_t**)calloc(dim, sizeof(node_t*));

	assert(newNode != NULL);
	assert(newNode->depth_R != NULL);
	assert(newNode->depth_L != NULL);
	assert(newNode->left != NULL);
	assert(newNode->right != NULL);
	assert(newNode->parent != NULL);

	return newNode;
}

// Frees a node
static void bst_freeNode(node_t* node) {

	assert(node != NULL);
	assert(node->left != NULL);
	assert(node->right != NULL);
	assert(node->d != NULL);

	free(node->d);
	free(node->depth_R);
	free(node->depth_L);
	free(node->right);
	free(node->left);
	free(node->parent);
	free(node);
}

// Iteratively insert a node into a bst
static void bst_insertNode_Rec(bst_t* bst, node_t* root, node_t* newNode, int dataIndex) {
	
	assert(newNode != NULL);
	assert(root != NULL);
	assert(bst != NULL);

	if (newNode->d[dataIndex] < root->d[dataIndex]) {

		root->depth_L[dataIndex]++;
		if (root->left[dataIndex] == NULL) {
			root->left[dataIndex] = newNode;
			newNode->parent[dataIndex] = root;
			bst_balance(bst, root, dataIndex);
		} else {
			bst_insertNode_Rec(bst, root->left[dataIndex],newNode,dataIndex);
		}
	} else {
		root->depth_R[dataIndex]++;
		if (root->right[dataIndex] == NULL) {
			root->right[dataIndex] = newNode;
			newNode->parent[dataIndex] = root;
		} else {
			bst_insertNode_Rec(bst, root->right[dataIndex],newNode,dataIndex);
		}
	}
	bst_balance(bst, root, dataIndex);
}

// Recursively print tree (in order traverse), returns number of items printed
static int bst_printTree_Rec(node_t* n, int dataIndex, int dim, FILE* stream) {

	assert(dataIndex < dim);

	int numPrinted = 0;

	if (n!=NULL) {

		node_t* left = n->left[dataIndex];
		node_t* right = n->right[dataIndex];
		numPrinted=1;

		numPrinted += bst_printTree_Rec(left, dataIndex, dim, stream);
		bst_printData(dim, n->d, stream);
		numPrinted += bst_printTree_Rec(right, dataIndex, dim, stream);
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

// Recursively search the tree
static void bst_search_Rec(node_t* node, results_t *f, int t) {
	
	assert(f!=NULL);
	assert(t < f->dim);

	if ( node != NULL ) {

		int aboveUpper=0, belowLower=0, inBetween=0;
		
		aboveUpper = node->d[t] > (f->filter[t].hi + EPS);
		belowLower = node->d[t] < (f->filter[t].lo - EPS);
		inBetween = (aboveUpper == 0) && (belowLower == 0);

		if ( aboveUpper ) {
			bst_search_Rec(node->left[t],  f, t);
		}

		if ( belowLower ) {
			bst_search_Rec(node->right[t],  f, t);
		}

		if ( inBetween ) {
			bst_search_Rec(node->left[t],  f, t);
			res_insert(f, node->d);
			bst_search_Rec(node->right[t],  f, t);
		}
	}
}

// Inserts a data entry into a results structure
// (provided it is within the bounds)
static void res_insert(results_t* res, float* d) {

	if(res->check(d,res)) {

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

// Results filter which will include points on the bounds of the filter
int res_filterBoundInclude(float* d, results_t* res) {
	// check that item is insertable
	int insert = 1, lessThanLo = 0, moreThanHi = 0;
	int i=0;
	for (i=0;i<res->dim;i++) {

		lessThanLo = d[i] < res->filter[i].lo;
		moreThanHi = d[i] > res->filter[i].hi;
		if ( lessThanLo || moreThanHi ) {
			insert = 0;
		}
	}
	return insert;
}

// Results filter which will include points inside bounds of the filter
int res_filterBoundExclude(float* d, results_t* res) {
	// check that item is insertable
	int insert = 1, lessThanLo = 0, moreThanHi = 0;
	int i=0;
	for (i=0;i<res->dim;i++) {

		lessThanLo = d[i] <= res->filter[i].lo;
		moreThanHi = d[i] >= res->filter[i].hi;
		if ( lessThanLo || moreThanHi ) {
			insert = 0;
		}
	}
	return insert;
}





/* * * * * * * * * * * * * * * * * * * * * * *  PRIVATE FUNCTIONS (BALANCING) */

/*	Input imbalance visualisation:
 *
 *  Left outer |  Left inner | Right outer | Right inner |
 *		 a     |       a     |     a       |     a       |
 *	    /      |      /      |      \      |      \      |
 *     b       |     b       |       b     |       b     |
 *    /        |      \      |        \    |      /      |
 *   c         |       c     |         c   |     c       |
 *             |             |             |             |
 */


// Correct imbalances in the tree
static void bst_balance( bst_t*bst, node_t* a, int dataIndex ) {

	assert(bst != NULL);
	assert(a != NULL);

	int a_depDiff = a->depth_R[dataIndex] - a->depth_L[dataIndex];
	int r_depDiff = 0;
	int l_depDiff = 0;

	if ( a_depDiff >= BST_BALANCE_A ) {

		r_depDiff = 
			a->right[dataIndex]->depth_R[dataIndex] - a->depth_L[dataIndex];

		if ( r_depDiff >= BST_BALANCE_B) {
			bst_rightOuterRot(bst, a, dataIndex);
		} else {
			bst_rightInnerRot(bst, a, dataIndex);
		}

	} else if ( a_depDiff <= -BST_BALANCE_A){

		l_depDiff =
			a->left[dataIndex]->depth_R[dataIndex] - a->depth_L[dataIndex];

		if ( l_depDiff <= -BST_BALANCE_B) {
			bst_leftOuterRot(bst, a, dataIndex);
		} else {
			bst_leftInnerRot(bst, a, dataIndex);
		}
	}
}

// Left outer rotation balancing
static node_t* bst_leftOuterRot(bst_t* bst, node_t* a, int dataIndex) {

	assert(bst != NULL);
	assert(a!=NULL);
	assert(a->left[dataIndex]!=NULL);
	node_t* b = a->left[dataIndex];
	node_t* parent = a->parent[dataIndex];

	// a.left points to b.right
	if (b->right[dataIndex] != NULL) {
		a->left[dataIndex] = b->right[dataIndex];
		a->left[dataIndex]->parent[dataIndex] = a;
		a->depth_L[dataIndex] = 1 + node_maxDepth(a->left[dataIndex], dataIndex);
	} else {
		a->left[dataIndex] = NULL;
		a->depth_L[dataIndex] = 0;
	}
	
	// b.right points to a
	b->right[dataIndex] = a;
	b->right[dataIndex]->parent[dataIndex] = b;
	b->depth_R[dataIndex] = 1 + node_maxDepth(b->right[dataIndex], dataIndex);
	
	// parent now points to b
	if (parent != NULL) {
		if (parent->left[dataIndex] == a) {
			parent->left[dataIndex] = b;
			b->parent[dataIndex] = parent;
		} else if (parent->right[dataIndex] == a) {
			parent->right[dataIndex] = b;
			b->parent[dataIndex] = parent;
		} else {
			exit(EXIT_FAILURE);
		}
	} else { // must account for when node_a is the bst root and parent==NULL
		bst->root[dataIndex] = b;
		b->parent[dataIndex] = NULL;
	}
	fixParentDepth(bst, b, dataIndex);
	return b;
}

// Right outer rotation balancing
static node_t* bst_rightOuterRot(bst_t* bst, node_t* a, int dataIndex) {
	
	assert(bst != NULL);
	assert(a!=NULL);
	assert(a->right[dataIndex]!=NULL);
	node_t* b = a->right[dataIndex];
	node_t* parent = a->parent[dataIndex];

	// a.right points to b.left
	if (b->left[dataIndex]!= NULL) {
		a->right[dataIndex] = b->left[dataIndex];
		(a->right[dataIndex])->parent[dataIndex] = a;
		a->depth_R[dataIndex] = 1 + node_maxDepth(a->right[dataIndex], dataIndex);
	} else {
		a->right[dataIndex] = NULL;
		a->depth_R[dataIndex] = 0;
	}
	
	// b.left points to a
	b->left[dataIndex] = a;
	(b->left[dataIndex])->parent[dataIndex] = b;
	b->depth_L[dataIndex] = 1 + node_maxDepth(b->left[dataIndex], dataIndex);
	
	// parent now points to b
	if (parent != NULL) {
		if (parent->right[dataIndex] == a) {
			parent->right[dataIndex] = b;
			b->parent[dataIndex] = parent;
		} else if (parent->left[dataIndex] == a) {
			parent->left[dataIndex] = b;
			b->parent[dataIndex] = parent;
		} else {
			exit(EXIT_FAILURE);
		}
	} else { // must account for when node_a is the bst root and parent==NULL
		bst->root[dataIndex] = b;
		b->parent[dataIndex] = NULL;
	}
	fixParentDepth(bst, b, dataIndex);
	return b;
}

// Left inner rotation balancing
static node_t* bst_leftInnerRot( bst_t* bst, node_t* a, int dataIndex){

	assert(a!=NULL);
	assert(bst != NULL);

	node_t* b = a->left[dataIndex];
	assert(b!=NULL);
	node_t* c = b->right[dataIndex];
	assert(c!=NULL);
	node_t* parent = a->parent[dataIndex];
	
	// a.left points to c.right
	if (c->right[dataIndex] != NULL) {
		a->left[dataIndex] = c->right[dataIndex];
		a->left[dataIndex]->parent[dataIndex] = a;
		a->depth_L[dataIndex] = 1 + node_maxDepth(a->left[dataIndex], dataIndex);
	} else {
		a->left[dataIndex] = NULL;
		a->depth_L[dataIndex] = 0;
	}
	
	// b.right points to c.left
	if (c->left[dataIndex] != NULL) {
		b->right[dataIndex] = c->left[dataIndex];
		b->right[dataIndex]->parent[dataIndex] = b;
		b->depth_R[dataIndex] = 1 + node_maxDepth(b->right[dataIndex], dataIndex);
	} else {
		b->right[dataIndex] = NULL;
		b->depth_R[dataIndex] = 0;
	}

	// c.left points to b
	c->left[dataIndex] = b;
	c->left[dataIndex]->parent[dataIndex] = b;
	c->depth_L[dataIndex] = 1 + node_maxDepth(c->left[dataIndex], dataIndex);
	
	// c.right points to a
	c->right[dataIndex] = a;
	c->right[dataIndex]->parent[dataIndex] = a;
	c->depth_R[dataIndex] = 1 + node_maxDepth(c->right[dataIndex], dataIndex);
	
	// parent now points to c
	if (parent != NULL) {
		if (parent->left[dataIndex] == a) {
			parent->left[dataIndex] = c;
			c->parent[dataIndex] = parent;
		} else if (parent->right[dataIndex] == a) {
			parent->right[dataIndex] = c;
			c->parent[dataIndex] = parent;
		} else {
			exit(EXIT_FAILURE);
		}
	} else { // must account for when node_a is the bst root and parent==NULL
		bst->root[dataIndex] = c;
		c->parent[dataIndex] = NULL;
	}
	fixParentDepth(bst, c, dataIndex);
	return c;
}

// Right inner rotation balancing
static node_t* bst_rightInnerRot( bst_t* bst, node_t* a, int dataIndex){

	assert(a!=NULL);
	assert(bst != NULL);

	node_t* b = a->right[dataIndex];
	assert(b!=NULL);
	node_t* c = b->left[dataIndex];
	assert(c!=NULL);
	node_t* parent = a->parent[dataIndex];
	
	// a.right points to c.left
	if (c->left[dataIndex] != NULL) {
		a->right[dataIndex] = c->left[dataIndex];
		a->right[dataIndex]->parent[dataIndex] = a;
		a->depth_R[dataIndex] = 1 + node_maxDepth(a->right[dataIndex], dataIndex);
	} else {
		a->right[dataIndex] = NULL;
		a->depth_R[dataIndex] = 0;
	}
	
	// b.left points to c.right
	if (c->right[dataIndex] != NULL) {
		b->left[dataIndex] = c->right[dataIndex];
		b->left[dataIndex]->parent[dataIndex] = b;
		b->depth_L[dataIndex] = 1 + node_maxDepth(b->left[dataIndex], dataIndex);
	} else {
		b->left[dataIndex] = NULL;
		b->depth_L[dataIndex] = 0;
	}

	// c.left points to a
	c->left[dataIndex] = a;
	c->left[dataIndex]->parent[dataIndex] = c;
	c->depth_L[dataIndex] = 1 + node_maxDepth(c->left[dataIndex], dataIndex);
	
	// c.right points to b
	c->right[dataIndex] = b;
	c->right[dataIndex]->parent[dataIndex] = c;
	c->depth_R[dataIndex] = 1 + node_maxDepth(c->right[dataIndex], dataIndex);
	
	
	// parent now points to c
	if (parent != NULL) {
		if (parent->left[dataIndex] == a) {
			parent->left[dataIndex] = c;
			c->parent[dataIndex] = parent;
		} else if (parent->right[dataIndex] == a) {
			parent->right[dataIndex] = c;
			c->parent[dataIndex] = parent;
		} else {
			exit(EXIT_FAILURE);
		}
		
	} else { // must account for when node_a is the bst root and parent==NULL
		bst->root[dataIndex] = c;
		c->parent[dataIndex] = NULL;
	}
	fixParentDepth(bst, c, dataIndex);
	return c;
}

// Returns the maximum depth of the left and right branches
static int node_maxDepth(node_t* node, int dataIndex) {
	return MAX(node->depth_L[dataIndex], node->depth_R[dataIndex]);
}

// Propogates a change in depth up the tree towards the root
static void fixParentDepth(bst_t* bst, node_t* node_a, int dataIndex) {
	
	node_t* parent = node_a->parent[dataIndex];
	node_t* next = node_a;

	while (parent!=NULL) {

		if (parent->left[dataIndex] == next) {
			parent->depth_L[dataIndex] = 1 + node_maxDepth(next, dataIndex);
		} else if (parent->right[dataIndex] == next) {
			parent->depth_R[dataIndex] = 1 + node_maxDepth(next, dataIndex);
		} else {
			printf("The tree is broken!\n");
			exit(EXIT_FAILURE);
		}
		next = parent;
		parent = parent->parent[dataIndex];
	}
}
