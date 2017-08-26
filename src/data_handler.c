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
#include "linkedlist.h"

/* * * * * * * * * * * * * * * * * * * * * * *  PRIVATE FUNCTION DECLARATIONS */

// Private functions 
// 		- deal directly with nodes should be private and/or
// 		- are used to balance the tree and/or
//		- are recursive

static node_t* bst_newNode(int dim, float* data);
static void bst_freeNode(node_t* node);
static void bst_insertNode_Itr(bst_t* bst, node_t* newNode, int dataIndex);
static void bst_balance( bst_t* bst, node_t* node_a, int dataIndex);
static node_t* bst_leftOuterRot(bst_t* bst, node_t* node_a, int dataIndex);
static node_t* bst_rightOuterRot(bst_t* bst, node_t* node_a, int dataIndex);
static node_t* bst_leftInnerRot(bst_t* bst, node_t* node_a, int dataIndex);
static node_t* bst_rightInnerRot(bst_t* bst, node_t* node_a, int dataIndex);
static int bst_printTree_Rec( node_t* n, int dataIndex, int dim, FILE* stream);
static int bst_freeTree_Rec(bst_t* bst, node_t* root, int dataIndex);
static void bst_search_Itr(bst_t* bst, results_t *f);
static void bst_search_Rec(node_t* node, results_t *f, int t);
static void bst_insertNode_Rec(bst_t* bst, node_t* root, node_t* newNode, int dataIndex);
static int node_maxDepth(node_t* node, int dataIndex);
static void fixParentDepth(bst_t* bst, node_t* node_a, int dataIndex);




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *  PARSING FUNCTIONS */

// Returns bst containing data from the flow file
bst_t* parseFlowFile(char *filename) {

	assert(filename!=NULL);
	
	FILE* fp = fopen(filename, FILE_READ);
	assert(fp != NULL);
	
	bst_t* bst = parseFlowFileFirstLine(fp);
	while (parseFlowFileDataLine(bst, fp) != PARSE_DONE) {
	}
	//printf("fin - building tree\n");

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
		//printf("flag0.1\n");
		bst_insertData(bst, data);
		//printf("flag0.2\n");
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

	// printf("\nInserting node: ");
	// bst_printData(4, data, stdout);
	
	assert(bst!=NULL);
	assert(data!=NULL);

	node_t* newNode = bst_newNode(bst->dim, data);
	
	int i=0;
	for (i=0; i<bst->dim; i++) {
		if (bst->root[i] == NULL) {
			bst->root[i] = newNode;
		} else {
			//bst_insertNode_Itr(bst, newNode, i);
			bst_insertNode_Rec(bst, bst->root[i],newNode,i);
		}
	}
	//bst_printTree(bst,0, stdout);
	//printf("\n");
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

	//printf("Rec search: %d\n",bst_search_Rec(bst->root[BST_INDEX], res) );
	//bst_search_Itr(bst, res);

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
	//assert(insert == 1);

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
// static void bst_insertNode_Itr(bst_t* bst, node_t* newNode, int dataIndex) {

// 	assert(bst != NULL);
// 	assert(newNode != NULL);

// 	// The place where the newNode may be inserted
// 	node_t* next = bst->root[dataIndex];

// 	// Iteratively determine where to insert the node
// 	while (1) {

// 		// Go left or right
// 		if ( newNode->d[dataIndex] < next->d[dataIndex]) {
// 			next->depthDiff[dataIndex]--;
// 			if (next->left[dataIndex] == NULL) {
// 				next->left[dataIndex] = newNode;
// 				newNode->parent[dataIndex] = next;
// 			} else {
// 				next = next->left[dataIndex];
// 			}
// 		} else {
// 			next->depthDiff[dataIndex]++;
// 			if (next->right[dataIndex] == NULL) {
// 				next->right[dataIndex] = newNode;
// 				newNode->parent[dataIndex] = next;
// 			} else {
// 				next = next->right[dataIndex];
// 			}
// 		}
// 	}
// }

// Iteratively insert a node into a bst
static void bst_insertNode_Rec(bst_t* bst, node_t* root, node_t* newNode, int dataIndex) {
	
	assert(newNode != NULL);
	assert(root != NULL);
	assert(bst != NULL);

	if (newNode->d[dataIndex] < root->d[dataIndex]) {
		//printf("L");
		fflush(stdout);

		root->depth_L[dataIndex]++;
		if (root->left[dataIndex] == NULL) {
			root->left[dataIndex] = newNode;
			newNode->parent[dataIndex] = root;
			bst_balance(bst, root, dataIndex);
			//printf("D");
			fflush(stdout);
		} else {
			bst_insertNode_Rec(bst, root->left[dataIndex],newNode,dataIndex);
		}
	} else {

		//printf("R");
		fflush(stdout);
		root->depth_R[dataIndex]++;
		if (root->right[dataIndex] == NULL) {
			root->right[dataIndex] = newNode;
			newNode->parent[dataIndex] = root;
			//printf("D");
			fflush(stdout);
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

static void bst_search_Rec(node_t* node, results_t *f, int t) {
	
	assert(f!=NULL);
	assert(t < f->dim);

	if ( node != NULL ) {

		int aboveUpper=0, belowLower=0, inBetween=0;
		
		aboveUpper = node->d[t] > f->filter[t].hi;
		belowLower = node->d[t] < f->filter[t].lo;
		inBetween = (aboveUpper == 0) && (belowLower == 0);
		//inBetween = 1;

		if ( (aboveUpper || inBetween)) {
			bst_search_Rec(node->left[t],  f, t);
		}

		if ( (belowLower || inBetween) ) {
			bst_search_Rec(node->right[t],  f, t);
		}

		res_insert(f, node->d);
	}
}

// Iterativeley search a tree (pre order traverse), returns number items searched
static void bst_search_Itr(bst_t* bst, results_t *f) {
	
	assert(f!=NULL);
	assert(bst != NULL);
	if ( bst->root[BST_INDEX] == NULL ) { return; }

	// Determine best filter to apply (the one with the most narrow bounds)
	float minDiff=FLT_MAX;
	int i=0, t=0;
	for (i=0; i<bst->dim; i++) {
		if ((f->filter[i].hi - f->filter[i].lo) < minDiff ) {
			minDiff = f->filter[i].hi - f->filter[i].lo;
			t = i;
		}
	}

	// searchQueue
	list_t* searchQ = list_new(free);
	list_push(searchQ, bst->root[t]);
	node_t* node = NULL;

	// Iterate over every node
	int aboveUpper=0, belowLower=0, inBetween=0;
	while ( searchQ->num_elements > 0 ) {

		node = list_pop(searchQ);
			
		aboveUpper = node->d[t] > f->filter[t].hi;
		belowLower = node->d[t] < f->filter[t].lo;
		inBetween = (aboveUpper == 0) && (belowLower == 0);
		//inBetween = 1;

		if ( (aboveUpper || inBetween) && (node->left[t] != NULL)) {
			list_push(searchQ, node->left[t]);
		}
		if ( (belowLower || inBetween) && (node->right[t] != NULL)) {
			list_push(searchQ, node->right[t]);
		}

		res_insert(f, node->d);
	}
	list_free(searchQ);
}






/* * * * * * * * * * * * * * * * * * * * * * *  PRIVATE FUNCTIONS (BALANCING) */

// Correct imbalances in the tree
static void bst_balance( bst_t*bst, node_t* node_a, int dataIndex ) {

	assert(bst != NULL);
	assert(node_a != NULL);

	// printf("flag1\n");
	int a_delDepth = node_a->depth_R[dataIndex] - node_a->depth_L[dataIndex];
	int r_delDepth = 0;
	int l_delDepth = 0;

	//printf("flag2\n");
	if ( a_delDepth >= BST_BALANCE_THRESH ) {
		//printf("flag3a\n");
		assert(node_a->right[dataIndex] != NULL);
		r_delDepth = node_a->right[dataIndex]->depth_R[dataIndex]
												- node_a->depth_L[dataIndex];

		if ( r_delDepth > 0) {
			//printf("r_r(%d,%d) ", a_delDepth, r_delDepth );
			fflush(stdout);
			bst_rightOuterRot(bst, node_a, dataIndex);
		} else {
			//printf("r_l(%d,%d) ", a_delDepth, r_delDepth );
			fflush(stdout);
			bst_rightInnerRot(bst, node_a, dataIndex);
		}

	} else if ( a_delDepth <= -BST_BALANCE_THRESH){
		//printf("flag3b\n");

		assert(node_a->left[dataIndex] != NULL);
		l_delDepth = node_a->left[dataIndex]->depth_R[dataIndex]
												- node_a->depth_L[dataIndex];

		if ( l_delDepth < 0) {
			//printf("l_l(%d,%d) ", a_delDepth, l_delDepth );
			fflush(stdout);
			bst_leftOuterRot(bst, node_a, dataIndex);
		} else {
			//printf("l_r(%d,%d) ", a_delDepth, l_delDepth );
			fflush(stdout);
			bst_leftInnerRot(bst, node_a, dataIndex);
		}
	}
	//printf("balanced(%d,%d,%d)\n", a_delDepth,l_delDepth, r_delDepth );
	//printf("flag4\n");

	assert(node_a->right[dataIndex] != node_a);
	assert(node_a->left[dataIndex] != node_a);
}

// Left outer rotation balancing
static node_t* bst_leftOuterRot(bst_t* bst, node_t* node_a, int dataIndex) {

	assert(bst != NULL);
	assert(node_a!=NULL);
	assert(node_a->left[dataIndex]!=NULL);
	node_t* node_b = node_a->left[dataIndex];
	node_t* parent = node_a->parent[dataIndex];

	// a.left points to b.right
	if (node_b->right[dataIndex] != NULL) {
		node_a->left[dataIndex] = node_b->right[dataIndex];
		node_a->left[dataIndex]->parent[dataIndex] = node_a;
		node_a->depth_L[dataIndex] = 1 + node_maxDepth(node_a->left[dataIndex], dataIndex);
	} else {
		node_a->left[dataIndex] = NULL;
		node_a->depth_L[dataIndex] = 0;
	}
	
	// b.right points to a
	node_b->right[dataIndex] = node_a;
	node_b->right[dataIndex]->parent[dataIndex] = node_b;
	node_b->depth_R[dataIndex] = 1 + node_maxDepth(node_b->right[dataIndex], dataIndex);
	
	// parent now points to b
	if (parent != NULL) {
		if (parent->left[dataIndex] == node_a) {
			parent->left[dataIndex] = node_b;
			node_b->parent[dataIndex] = parent;
		} else if (parent->right[dataIndex] == node_a) {
			parent->right[dataIndex] = node_b;
			node_b->parent[dataIndex] = parent;
		} else {
			exit(EXIT_FAILURE);
		}
	} else { // must account for when node_a is the bst root and parent==NULL
		bst->root[dataIndex] = node_b;
		node_b->parent[dataIndex] = NULL;
	}
	fixParentDepth(bst, node_b, dataIndex);
	return node_b;
}

// Right outer rotation balancing
static node_t* bst_rightOuterRot(bst_t* bst, node_t* node_a, int dataIndex) {
	
	assert(bst != NULL);
	assert(node_a!=NULL);
	assert(node_a->right[dataIndex]!=NULL);
	node_t* node_b = node_a->right[dataIndex];
	node_t* parent = node_a->parent[dataIndex];

	// a.right points to b.left
	if (node_b->left[dataIndex]!= NULL) {
		node_a->right[dataIndex] = node_b->left[dataIndex];
		(node_a->right[dataIndex])->parent[dataIndex] = node_a;
		node_a->depth_R[dataIndex] = 1 + node_maxDepth(node_a->right[dataIndex], dataIndex);
	} else {
		node_a->right[dataIndex] = NULL;
		node_a->depth_R[dataIndex] = 0;
	}
	
	// b.left points to a
	node_b->left[dataIndex] = node_a;
	(node_b->left[dataIndex])->parent[dataIndex] = node_b;
	node_b->depth_L[dataIndex] = 1 + node_maxDepth(node_b->left[dataIndex], dataIndex);
	
	// parent now points to b
	if (parent != NULL) {
		if (parent->right[dataIndex] == node_a) {
			parent->right[dataIndex] = node_b;
			node_b->parent[dataIndex] = parent;
		} else if (parent->left[dataIndex] == node_a) {
			parent->left[dataIndex] = node_b;
			node_b->parent[dataIndex] = parent;
		} else {
			exit(EXIT_FAILURE);
		}
	} else { // must account for when node_a is the bst root and parent==NULL
		bst->root[dataIndex] = node_b;
		node_b->parent[dataIndex] = NULL;
	}
	fixParentDepth(bst, node_b, dataIndex);
	return node_b;
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

static int node_maxDepth(node_t* node, int dataIndex) {
	return MAX(node->depth_L[dataIndex], node->depth_R[dataIndex]);
}

static void fixParentDepth(bst_t* bst, node_t* node_a, int dataIndex) {
	
	node_t* parent = node_a->parent[dataIndex];
	node_t* next = node_a;

	while (parent!=NULL) {

		if (parent->left[dataIndex] == next) {
			parent->depth_L[dataIndex] = 1 + node_maxDepth(next, dataIndex);
		} else if (parent->right[dataIndex] == next) {
			parent->depth_R[dataIndex] = 1 + node_maxDepth(next, dataIndex);
		} else {
			printf("dead!\n");
			exit(EXIT_FAILURE);
		}
		next = parent;
		parent = parent->parent[dataIndex];
	}
}
