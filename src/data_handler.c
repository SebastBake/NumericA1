/***************************************************************************
 *
 *   File        : data_handler.h
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

static void bst_printTree_Rec(bst_t* bst, node_t* root, int dataIndex) {

	assert(bst != NULL);

	if (root!=NULL) {
		bst_printTree_Rec(bst, root->left[dataIndex], dataIndex);
		bst_printData(bst, root->d);
		bst_printTree_Rec(bst, root->right[dataIndex], dataIndex);
	}
}

static void bst_freeTree_Rec(node_t* root, int dataIndex) {

	if (root!=NULL) {
		bst_freeTree_Rec(root->left[dataIndex], dataIndex);
		bst_freeTree_Rec(root->right[dataIndex], dataIndex);
		bst_freeNode(root);
	}

}

void bst_searchRange_Rec(node_t* node, results_t *f) {

	assert(f!=NULL);

	if (node!=NULL) {
		if ( node->d[f->i] < f->lo ) {
			//printf("searching recursively: found %f < lo=%f\n", node->d[f->i], f->lo);
			bst_searchRange_Rec(node->right[f->i], f);
		} else if (node->d[f->i] > f->hi ) {
			//printf("searching recursively: found %f > hi=%f\n", node->d[f->i], f->hi);
			bst_searchRange_Rec(node->left[f->i], f);
		} else {
			
			// printf("Found: ");
			// int i = 0;
			// for (i=0; i < 4; i++) {
			// 	printf("%f ", node->d[i] );
			// }
			// printf("\n");
			
			if (f->n >= f->len) {
				f->len += FOUND_LEN;
				f->arr = (double**)realloc(f->arr, f->len * sizeof(double*));
			}
			f->arr[f->n] = node->d;
			f->n++;

			bst_searchRange_Rec(node->left[f->i], f);
			bst_searchRange_Rec(node->right[f->i], f);
		}
	}
}







































bst_t* parseFlowFile(char *filename) {

	assert(filename!=NULL);
	
	FILE* fp = fopen(filename, FILE_READONLY);
	assert(fp != NULL);
	
	bst_t* bst = parseFirstLine(fp);

	//printf("parsing file...\n");
	while (parseFlowFileLine(bst, fp) != PARSE_FINISHED) {
	}

	fclose(fp);

	return bst;
}

bst_t* parseFirstLine(FILE* fp) {
	
	assert(fp!=NULL);

	char* tmpkey = (char* )malloc( sizeof(char) );

	char tmpc;
	int i = 0;
	while (1) {

		tmpc = fgetc(fp);
		if (tmpc == COMMA) { continue; }
		if (tmpc == NEWLINE) { break; }

		tmpkey = (char* )realloc(tmpkey, (i+2)* sizeof(char) );
		tmpkey[i] = tmpc;
		i++;
	}

	bst_t* bst = bst_newTree(i, tmpkey);
	assert(bst!=NULL);

	return bst;
}
	
int parseFlowFileLine(bst_t* bst, FILE* fp) {

	assert(bst!=NULL);
	assert(fp!=NULL);
	
	double* data = (double*)malloc(bst->dim*sizeof(double));
	assert(data != NULL);

	int i = 0, read=0;
	for (i=0; i < bst->dim; i++) {
		read += fscanf(fp, "%lf,", &(data[i]) );
	}

	char endCheck = fgetc(fp); // throw away new line

	if ( read==(bst->dim) && endCheck == NEWLINE) {
		//bst_printData(bst, data);
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


































bst_t* bst_newTree(int dim, char* key) {
	
	bst_t* bst = (bst_t*)malloc(sizeof(bst_t));
	assert(bst != NULL);

	bst->numNodes = 0;
	bst->dim = dim;
	bst->key = key;
	bst->root = (node_t** )calloc( bst->dim, sizeof(node_t*) );

	return bst;
}

void bst_freeTree(bst_t* bst) {

	bst_freeTree_Rec(bst->root[DEFAULT_BST_FREE_ORDER_INDEX], DEFAULT_BST_FREE_ORDER_INDEX);

	free(bst->root);
	free(bst->key);
	free(bst);

}

void bst_insertData(bst_t* bst, double* data) {
	
	assert(bst!=NULL);
	assert(data!=NULL);

	node_t* newNode = bst_newNode(bst->dim, data); //printf("made a node\n");
	
	int i=0;
	for (i=0; i<bst->dim; i++) {
		if (bst->root[i] == NULL) {
			bst->root[i] = newNode; //printf("inserted head\n");
		} else {
			bst_insertNode_Rec(bst->root[i], newNode, i); //printf("inserted\n");
		}
	}

	bst->numNodes++;
}

results_t bst_searchRange(bst_t* bst, double loBound, double upBound, int dataIndex) {

	assert(bst!=NULL);
	assert(loBound<upBound);
	assert(dataIndex<=bst->dim);

	results_t found;
	found.len = FOUND_LEN;
	found.n = 0;
	found.i = dataIndex;
	found.lo = loBound;
	found.hi = upBound;
	found.arr = (double**)calloc(found.len, sizeof(double*));
	assert(found.arr!=NULL);
	//printf("start rec search(%d): lo=%f hi=%f\n", found.i, found.lo, found.hi);
	bst_searchRange_Rec(bst->root[found.i], &found);
	return found;
}

void bst_printTree(bst_t* bst, int dataIndex) {

	assert(bst!=NULL);

	bst_printTree_Rec(bst, bst->root[dataIndex], dataIndex);
}

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


