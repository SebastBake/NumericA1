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

// static void bst_freeNode(node_t* node) {
// 	free(node->d);
// 	free(node);
// }

static void bst_insertNode_Rec(node_t* root, node_t* node, int dataIndex) {

	if ( node->d[dataIndex] < root->d[dataIndex]) {
		if (root->left[dataIndex] != NULL) {
			bst_insertNode_Rec(root->left[dataIndex], node, dataIndex);
		} else {
			root->left[dataIndex] = node;
		}
	} else {
		if (root->right[dataIndex] != NULL) {
			bst_insertNode_Rec(root->right[dataIndex], node, dataIndex);
		} else {
			root->right[dataIndex] = node;
		}
	}
}

static void bst_printTree_Rec(bst_t* bst, node_t* root, int dataIndex) {
	if (root!=NULL) {
		bst_printTree_Rec(bst, root->left[dataIndex], dataIndex);
		bst_printData(bst, root->d);
		bst_printTree_Rec(bst, root->right[dataIndex], dataIndex);
	}
}

void bst_printTree(bst_t* bst, int dataIndex) {
	bst_printTree_Rec(bst, bst->root[dataIndex], dataIndex);
}

bst_t* bst_newTree() {
	
	bst_t* bst = (bst_t*)malloc(sizeof(bst_t*));
	assert(bst != NULL);

	bst->numNodes = 0;
	bst->dim = 0;
	bst->root = NULL;
	bst->key = NULL;

	return bst;
}

node_t* bst_newNode() {
	node_t* newNode = (node_t*)malloc(sizeof(node_t));
	assert(newNode != NULL);

	newNode->d = NULL;
	newNode->left = NULL;
	newNode->right = NULL;

	return newNode;
}

bst_t* parseFlowFile(char *filename) {

	assert(filename!=NULL);
	
	FILE* fp = fopen(filename, FILE_READONLY);
	assert(fp != NULL);
	bst_t* bst = bst_newTree();
	
	parseFirstLine(bst, fp);

	while (parseFlowFileLine(bst, fp) != PARSE_FINISHED) {
	}

	fclose(fp);

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
		//bst_insertData(bst, data);
		return !PARSE_FINISHED;
	} else if ( endCheck == EOF) {
		free(data);
		return PARSE_FINISHED;
	} else {
		printf("ERROR: File parsing failed, exiting...");
		exit(EXIT_FAILURE);
	}
}

int parseFirstLine(bst_t* bst, FILE* fp) {

	assert(bst!=NULL);
	assert(fp!=NULL);
	
	bst->key = (char*)malloc(sizeof(char));

	char tmpc;
	int i = 0;
	while (1) {

		tmpc = fgetc(fp);
		if (tmpc == COMMA) { continue; }
		if (tmpc == NEWLINE) { break; }

		bst->key[i] = tmpc;
		bst->key = realloc(bst->key, (i+2)*(sizeof(char)));
		assert(bst->key != NULL);
		i++;
	}

	bst->dim = i;
	return bst->dim;
}

void bst_printData(bst_t* bst, double* data) {

	assert(bst!=NULL);
	assert(data!=NULL);

	printf("Node: ");
	int i = 0;
	for (i=0; i < bst->dim; i++) {
		printf("%c=%f, ",bst->key[i], data[i] );
	}
	printf("\n");
}

void bst_insertData(bst_t* bst, double* data) {
	
	assert(bst!=NULL);
	assert(data!=NULL);

	node_t* newNode = (node_t*)malloc(sizeof(node_t));
	assert(newNode != NULL);
	newNode->left = (node_t**)malloc(bst->dim*sizeof(node_t*));
	assert(newNode->left!=NULL);
	newNode->right = (node_t**)malloc(bst->dim*sizeof(node_t*));
	assert(newNode->right!=NULL);
	newNode->d = data;
	
	int i=0;
	for (i=0; i<bst->dim; i++) {
		if (bst->root[i] == NULL) {
			bst->root[i] = newNode;
		} else {
			bst_insertNode_Rec(bst->root[i], newNode, i);
		}
	}
}


