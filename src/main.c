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
#define INPUT_INSTRUCTIONS "USAGE EXAMPLE: ./flow flow_data.csv 24"

typedef struct timeval myTime_t;
myTime_t timer_start();
double timer_stop(myTime_t start);

int main(int argc, char *argv[]) {

	// Parse Command Line Arguments
	if (argc != NUM_ARGS) {
		printf(INPUT_INSTRUCTIONS);
		exit(EXIT_FAILURE);
	}
	char* flowFileName = argv[1];
	int gridResolution = atoi(argv[2]);

	// Generate bst
	bst_t* bst = parseFlowFile(flowFileName);
	
	// Task 1: Find the maximum velocity difference
	myTime_t task1_time = timer_start();
	maxveldiff(bst);
	printf("TASK1: %.2lf milliseconds\n", timer_stop(task1_time));

	// Task 2: Coarser Grid
	myTime_t task2_time = timer_start();
	coarsegrid(bst, gridResolution);
	printf("TASK2: %.2lf milliseconds\n", timer_stop(task2_time));

	// Task 3: Statistics
	myTime_t task3_time = timer_start();
	velstat(bst);
	printf("TASK3: %.2lf milliseconds\n", timer_stop(task3_time));

	// Task 4: Wake height and visualisation
	myTime_t task4_time = timer_start();
	wakevis(bst);
	printf("TASK4: %.2lf milliseconds\n", timer_stop(task4_time));

	// free bst
	bst_freeTree(bst);

	return EXIT_SUCCESS;
}

myTime_t timer_start() {
	myTime_t start;
	gettimeofday(&start, NULL);
	return start;
}

double timer_stop(myTime_t start) {
	myTime_t stop;
	gettimeofday(&stop, NULL);
	double elapsed_ms = (stop.tv_sec - start.tv_sec) * 1000.0;
	elapsed_ms += (stop.tv_usec - start.tv_usec) / 1000.0;
	return elapsed_ms;
}
