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
	
	node_t* newNode = (node_t*)malloc(sizeof(node_t));
	assert(newNode != NULL);
	newNode->d = (float*)malloc(bst->dim*sizeof(float));
	assert(newNode->d != NULL);

	int i = 0, read=0;
	for (i=0; i < bst->dim; i++) {
		read += fscanf(fp, "%f,", &(newNode->d[i]) );
	}

	char endCheck = fgetc(fp); // throw away new line

	if ( read==(bst->dim) && endCheck == NEWLINE) {
		bst_printNode(bst, newNode);
		// bst_insertNode(bst, newNode);
		return !PARSE_FINISHED;
	} else if ( endCheck == EOF) {
		bst_freeNode(newNode);
		return PARSE_FINISHED;
	} else {
		printf("ERROR: File parsing failed, exiting...");
		exit(EXIT_FAILURE);
	}
}

int parseFirstLine(bst_t* bst, FILE* fp) {
	
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

void bst_freeNode(node_t* node) {
	free(node->d);
	free(node);
}

void bst_printNode(bst_t* bst, node_t* node) {
	printf("Node: ");
	int i = 0;
	for (i=0; i < bst->dim; i++) {
		printf("%c=%f, ",bst->key[i], node->d[i] );
	}
	printf("\n");
}

void bst_insertNode(bst_t* bst, node_t* node) {

}


