/***************************************************************************
 *
 *   File        : main.c
 *   Student Id  : 757931
 *   Name        : Sebastian Baker
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include "tasks.h"
#include "data_handler.h"

#define NUM_ARGS 3
#define INPUT_INSTRUCTIONS "USAGE EXAMPLE: ./flow flow_data.csv 128"

//void myTest(char* flowFileName);

int main(int argc, char *argv[]) {

	// Parse Command Line Arguments
	if (argc != NUM_ARGS) {
		printf(INPUT_INSTRUCTIONS);
		exit(EXIT_FAILURE);
	}

	char* flowFileName = argv[1];
	int gridResolution = atoi(argv[2]);
	printf("FILE: %s, RES: %d\n", flowFileName, gridResolution);

	//myTest(flowFileName);

	/* TODO: Add timing for each task and output running time in ms */

	bst_t* bst = parseFlowFile(flowFileName); // Generate bst
	
	maxveldiff(bst); // Task 1: Find the maximum velocity difference
	coarsegrid(bst, gridResolution); // Task 2: Coarser Grid
	velstat(bst); // Task 3: Statistics
	wakevis(bst); // Task 4: Wake height and visualisation

	bst_freeTree(bst); // free bst

	return EXIT_SUCCESS;
}

// int dudcheck(float* d, results_t* f) {
// 	return 1;
// }

// void myTest(char* flowFileName) {

// 	printf("Parsing...\n");
// 	bst_t* bst = parseFlowFile(flowFileName);
	
// 	printf("Searching...\n");

// 	resultsFilter_t searchFilter[] = {
// 		{-MAXFLOAT, MAXFLOAT},
// 		{-MAXFLOAT, MAXFLOAT},
// 		{-MAXFLOAT, MAXFLOAT},
// 		{-MAXFLOAT, MAXFLOAT}
// 	};
	
// 	results_t* r = res_search(bst,, &dudcheck);
	
// 	int i;
// 	for (i=0; i<r->numEl; i++) {
// 		bst_printData(bst, r->arr[i], stdout);
// 	}

// 	res_free(r);

// 	printf("Freeing: bst_freeTree(bst)\n");
// 	printf("Freed: %d\n", bst_freeTree(bst));
// 	bst = NULL;
// }
