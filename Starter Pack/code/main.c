/***************************************************************************
 *
 *   File        : main.c
 *   Student Id  : <INSERT STUDENT ID HERE>
 *   Name        : <INSERT STUDENT NAME HERE>
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include "tasks.h"

int main(int argc, char *argv[]) {
	
	/* TODO: Parse Command Line Arguments */
	char* flow_file = NULL;
	int resolution = 0;

	/* TODO: Add timing for each task and output running time in ms */
    
	/* Task 1: Find the maximum velocity difference */
	maxveldiff(flow_file);
	
	/* Task 2: Coarser Grid */
	coarsegrid(flow_file, resolution);
	
	/* Task 3: Statistics */
	velstat(flow_file);
	
	/* Task 4: Wake height and visualisation */
	wakevis(flow_file);
    
	return (EXIT_SUCCESS);
}
