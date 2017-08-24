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

void myTest(char* flowFileName);

int main(int argc, char *argv[]) {

	/* Parse Command Line Arguments */
	if (argc != NUM_ARGS) {
		printf(INPUT_INSTRUCTIONS);
		exit(EXIT_FAILURE);
	}
	char* flowFileName = argv[1];
	int gridResolution = atoi(argv[2]);
	printf("FILENAME: %s\nRESOLUTION: %d\n", flowFileName, gridResolution);

	myTest(flowFileName);

	/* TODO: Add timing for each task and output running time in ms */

	/* Task 0: Parse input file */
	//bst_t* bst = parseFlowFile(flowFileName);
    
	/* Task 1: Find the maximum velocity difference */
	//maxveldiff();
	
	/* Task 2: Coarser Grid */
	//coarsegrid(, gridResolution);
	
	/* Task 3: Statistics */
	//velstat();
	
	/* Task 4: Wake height and visualisation */
	//wakevis();
    
	return EXIT_SUCCESS;
}

void myTest(char* flowFileName) {

	printf("Parsing...\n");
	bst_t* bst = parseFlowFile(flowFileName);
	
	//bst_printTree(bst, 0);
	
	printf("Searching... bst_searchRange(bst, -0.001158, -0.001158, 3) \n");
	results_t r = bst_searchRange(bst, -0.001158 - 0.0000005, -0.001158 + 0.0000005, 3);
	
	int i;
	for (i=0; i<r.n; i++) {
		bst_printData(bst, r.arr[i]);
	}

	printf("Freeing: bst_freeTree(bst)\n");
	printf("Freed: %d\n", bst_freeTree(bst));
	bst = NULL;

}
