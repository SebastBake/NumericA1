/***************************************************************************
 *
 *   File        : tasks.c
 *   Student Id  : 757931
 *   Name        : Sebastian Baker
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include "tasks.h"
#include "data_handler.h"

// Filter functions used during searching in maxveldiff(..)
int mvdMaxU(float* d, results_t* res);
int mvdMinU(float* d, results_t* res);
int mvdMaxV(float* d, results_t* res);
int mvdMinV(float* d, results_t* res);


void maxveldiff(bst_t* bst) {

	FILE* fp = fopen(TASK_1_CSV, FILE_REWRITE);

	results_t* minU = res_search(bst, MVD_THRESH, MAXFLOAT, BST_X, mvdMinU);
	results_t* maxU = res_search(bst, MVD_THRESH, MAXFLOAT, BST_X, mvdMaxU);
	results_t* minV = res_search(bst, MVD_THRESH, MAXFLOAT, BST_X, mvdMinV);
	results_t* maxV = res_search(bst, MVD_THRESH, MAXFLOAT, BST_X, mvdMaxV);

	bst_printKey(bst, fp);
	bst_printData(bst, minU->arr[0], fp);
	bst_printData(bst, maxU->arr[0], fp);
	bst_printData(bst, minV->arr[0], fp);
	bst_printData(bst, maxV->arr[0], fp);
	res_free(minU);
	res_free(maxU);
	res_free(minV);
	res_free(maxV);

	fclose(fp);
}

void coarsegrid(bst_t* bst, int resolution) {
	printf("coarsegrid() - IMPLEMENT ME!\n");
	exit(EXIT_FAILURE);
}

void velstat(bst_t* bst) {
	printf("velstat() - IMPLEMENT ME!\n");
	exit(EXIT_FAILURE);
}

void wakevis(bst_t* bst) {
	printf("wakevis() Part 1 - IMPLEMENT ME!\n");
	int i,j;
	int n = 12; // Location in x for wake visualization
	float* yheight;
	yheight = (float*) calloc(n,sizeof(float));
	/* Task 4: Part 2, nothing is to be changed here
	   Remember to output the spacing into the array yheight
	   for this to work. You also need to initialize i,j and 
	   yheight so the skeleton as it stands will not compile */
	 
	FILE *ft42;
	ft42 = fopen("task4_2.txt","w");
	for (j = 11; j>=0; j--){
	for (i=0;i<yheight[j]-yheight[0]+4;i++){
 		fprintf(ft42, " ");
	}
		fprintf(ft42, "*\n");
	}
	for (i=0;i<5; i++){
		fprintf(ft42, "III\n");
	}
	for(j = 0; j<12; j++ ){
		for (i=0;i<yheight[j]-yheight[0]+4;i++){
			fprintf(ft42, " ");
		}
		fprintf(ft42, "*\n");
	}
	fclose(ft42);
	
	/* Cleanup */
	free(yheight);

	exit(EXIT_FAILURE);
}





int mvdMaxU(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);
	
	if (res->numEl == 0) {
		return 1; // insert item
	} else if (res->numEl == 1) {
		if (d[BST_U] > res->arr[0][BST_U]) {
			res->arr[0] = d;
		}
		return 0; // Don't insert item
	}
	exit(EXIT_FAILURE);
}

int mvdMinU(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);

	if (res->numEl == 0) {
		return 1; // insert item
	} else if (res->numEl == 1) {
		if (d[BST_U] < res->arr[0][BST_U]) {
			res->arr[0] = d;
		}
		return 0; // Don't insert item
	}
	exit(EXIT_FAILURE);
}

int mvdMaxV(float* d, results_t* res) {
    
    assert(res->numEl <= 1 && res->numEl >= 0);

	if (res->numEl == 0) {
		return 1; // insert item
	} else if (res->numEl == 1) {
		if (d[BST_V] > res->arr[0][BST_V]) {
			res->arr[0] = d;
		}
		return 0; // Don't insert item
	}
	exit(EXIT_FAILURE);
}

int mvdMinV(float* d, results_t* res) {
    
    assert(res->numEl <= 1 && res->numEl >= 0);

	if (res->numEl == 0) {
		return 1; // insert item
	} else if (res->numEl == 1) {
		if (d[BST_V] < res->arr[0][BST_V]) {
			res->arr[0] = d;
		}
		return 0; // Don't insert item
	}
	exit(EXIT_FAILURE);
}
