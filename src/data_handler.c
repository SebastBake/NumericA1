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
	
	parseFirstLine(bst, fp); //printf("DIM: %d\n", bst->dim);

	while (parseFlowFileLine(bst, fp) != PARSE_FINISHED) {
	}

	fclose(fp);

	return bst;
}

int parseFlowFileLine(bst_t* bst, FILE* fp) {
	
	node_t* newNode = (node_t*)malloc(sizeof(node_t));
	assert(newNode != NULL);

	int dBytes = (bst->dim) * sizeof(float);
	newNode->d = (float*)malloc(dBytes);
	assert(newNode->d != NULL);

	printf("READLINE: ");
	int i = 0;
	while ( fscanf(fp, "%f,", &((newNode->d)[i]) ) ) {
		printf("%f ", newNode->d[i]);
		i++;
	}
	printf("\n");

	assert(fgetc(fp) == NEWLINE); // throw away new line

	if ( i==(bst->dim-1) ) {
		// bst_insertNode(bst, newNode);
		return !PARSE_FINISHED;
	} else if ( i==0 ) {
		return PARSE_FINISHED;
	} else {
		printf("ERROR: partial parse, exiting...");
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
		printf("C(%d): %c\n",i,tmpc);

		bst->key[i] = tmpc;
		bst->key = realloc(bst->key, (i+2)*(sizeof(char)));
		assert(bst->key != NULL);
		i++;
	}

	bst->dim = i;
	return bst->dim;
}


