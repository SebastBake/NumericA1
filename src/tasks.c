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
#include <sys/time.h>
#include <string.h>
#include "tasks.h"
#include "data_handler.h"

#define BST_X 0
#define BST_Y 1
#define BST_U 2
#define BST_V 3

void maxveldiff(const bst_t* data)
{
    printf("maxveldiff() - IMPLEMENT ME!\n");
    exit(EXIT_FAILURE);
}

void coarsegrid(const bst_t* data, int resolution)
{
    printf("coarsegrid() - IMPLEMENT ME!\n");
    exit(EXIT_FAILURE);
}

void velstat(const bst_t* data)
{
    printf("velstat() - IMPLEMENT ME!\n");
    exit(EXIT_FAILURE);
}

void wakevis(const bst_t* data)
{
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
