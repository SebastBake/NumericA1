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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * TASK FUNCTIONS */

void maxveldiff(bst_t* bst) {
	assert(bst!=NULL);

	FILE* fp = fopen(T1_CSV, FILE_REWRITE);
	assert(fp!=NULL);

	// search for max/min
	resultsFilter_t searchFilter[] = {
		{MVD_THRESH, FLT_MAX},
		{-FLT_MAX, FLT_MAX},
		{-FLT_MAX, FLT_MAX},
		{-FLT_MAX, FLT_MAX}
	};

	results_t* maxU = res_search(bst, searchFilter, mvdMaxU);
	results_t* minU = res_search(bst, searchFilter, mvdMinU);
	results_t* maxV = res_search(bst, searchFilter, mvdMaxV);
	results_t* minV = res_search(bst, searchFilter, mvdMinV);

	// print max/min
	bst_printKey(bst, fp);
	bst_printData(bst->dim, maxU->arr[0], fp);
	bst_printData(bst->dim, minU->arr[0], fp);
	bst_printData(bst->dim, maxV->arr[0], fp);
	bst_printData(bst->dim, minV->arr[0], fp);

	// free results structures
	res_free(minU);
	res_free(maxU);
	res_free(minV);
	res_free(maxV);

	fflush(fp);
	fclose(fp);
}

void coarsegrid(bst_t* bst, int resolution) {

	assert(bst!=NULL);

	cell_t* cells[resolution*resolution];
	cell_t* tmpCell = NULL;

	resultsFilter_t bound[] = {
		{-FLT_MAX, FLT_MAX},
		{-FLT_MAX, FLT_MAX},
		{-FLT_MAX, FLT_MAX},
		{-FLT_MAX, FLT_MAX}
	};

	float delta_x = (GRID_X_MAX - GRID_X_MIN) / resolution;
	float delta_y = (GRID_Y_MAX - GRID_Y_MIN) / resolution;

	int x_i=0, y_i=0, cell_i=0;
	for ( x_i=0 ; x_i<resolution ; x_i++ ) {
		for ( y_i=0 ; y_i<resolution ; y_i++ ) {

			// generate bounds for search
			bound[BST_X].lo = GRID_X_MIN + x_i*delta_x;
			bound[BST_X].hi = bound[BST_X].lo + delta_x;
			bound[BST_Y].lo = GRID_Y_MIN + y_i*delta_y;
			bound[BST_Y].hi = bound[BST_Y].lo+ delta_y;

			// search and generate cells
			tmpCell = generateCell(bst, bound);
			if (tmpCell != NULL) {
				cells[cell_i] = tmpCell;
				cell_i++;
			}
		}
	}

	// sort, print, free memory
	qsort(cells, cell_i, sizeof(cell_t*), cellCmp);
	printTask2(cells, cell_i);
	destroyCells(cells, cell_i);
}

void velstat(bst_t* bst) {

	assert(bst!=NULL);

	FILE* fp = fopen(T3_CSV, FILE_REWRITE);
	assert(fp!=NULL);
	fprintf(fp, T3_HEADER);

	float thresh = T3_INIT_THRESH;
	float percent = 0;
	int totalPoints = bst->numNodes;
	int numPointsFound = 0;
	
	while(percent<T3_PERCENT_END) {

		resultsFilter_t searchFilter[] = {
			{-FLT_MAX, FLT_MAX},
			{-FLT_MAX, FLT_MAX},
			{-FLT_MAX, thresh},
			{-FLT_MAX, FLT_MAX}
		};

		results_t* res = res_search(bst, searchFilter, noCheck);
		numPointsFound = res->numEl;
		res_free(res);
		percent = PERCENT(numPointsFound, totalPoints);
		fprintf(fp,"%.6f,%d,%.6f\n", thresh, numPointsFound, percent);
		thresh = thresh + T3_THRESH_INTERVAL;
	}

	fflush(fp);
	fclose(fp);
}

void wakevis(bst_t* bst) {

	// get spacing
	float* yheight = getYs_t4(bst);

	// Print wake profile using given skeleton
	int i,j;
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
}




/* * * * * * * * * * * * * * * * * * * * * * * * * *  TASK 1 HELPER FUNCTIONS */
// These functions are called by the bst search function to decide when to
// insert items into the search array. They are used in task 1 to replace 
// previous results so that the correct maximum/minimum is found

int mvdMaxU(float* d, results_t* res) {
	
	assert(res->numEl == 1 || res->numEl == 0);
	
	if (res->numEl == 0) {
		return 1; // insert item if it's the first result
	} else if (res->numEl == 1) {

		if (d[BST_U] > res->arr[0][BST_U]) { // found a new maximum U
			res->arr[0] = d; // replace previous result
		} else if (
			(d[BST_U] == res->arr[0][BST_U]) && // same maximum U
			(d[BST_X] < res->arr[0][BST_X])		// earlier in the domain X
		) {
			res->arr[0] = d; // replace previous result
		} else if (
			(d[BST_U] == res->arr[0][BST_U]) && // same maximum U
			(d[BST_X] == res->arr[0][BST_X]) && // same domain X
			(d[BST_Y] < res->arr[0][BST_Y])		// lower Y
		) {
			res->arr[0] = d; // replace previous result
		}
		return 0;

	}
	exit(EXIT_FAILURE);
}

int mvdMinU(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);

	if (res->numEl == 0) {
		return 1; // insert item if it's the first result
	} else if (res->numEl == 1) {

		if ( d[BST_U] < res->arr[0][BST_U] ) { // new min U found
			res->arr[0] = d; // replace item
		} else if ( 
			( d[BST_U] == res->arr[0][BST_U] ) && 	// same U
			( d[BST_X] < res->arr[0][BST_X] ) 		// earlier in domain X
		) {
			res->arr[0] = d; // replace item
		} else if (
			( d[BST_U] == res->arr[0][BST_U] ) && 	// same U
			( d[BST_X] == res->arr[0][BST_X] ) &&	// same domain X
			( d[BST_Y] < res->arr[0][BST_Y] )		// lower domain Y
		) {
			res->arr[0] = d; // replace item
		}
		return 0;
	}
	exit(EXIT_FAILURE);
}

int mvdMaxV(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);

	if (res->numEl == 0) {
		return 1; // insert item if it's the first result
	} else if (res->numEl == 1) {
		
		if ( d[BST_V] > res->arr[0][BST_V] ) { // new max X found
			res->arr[0] = d; // replace item
		} else if ( 
			( d[BST_V] == res->arr[0][BST_V] ) && 	// same V
			( d[BST_X] < res->arr[0][BST_X] ) 		// earlier in domain X
		) {
			res->arr[0] = d; // replace item
		} else if (
			( d[BST_V] == res->arr[0][BST_V] ) && 	// same V
			( d[BST_X] == res->arr[0][BST_X] ) &&	// same domain X
			( d[BST_Y] < res->arr[0][BST_Y] )		// lower domain Y
		) {
			res->arr[0] = d; // replace item
		}

		return 0;
	}
	exit(EXIT_FAILURE);
}

int mvdMinV(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);

	if (res->numEl == 0) {
		return 1; // insert item if it's the first result
	} else if (res->numEl == 1) {
		
		if ( d[BST_V] < res->arr[0][BST_V] ) { // new min V found
			res->arr[0] = d; // replace item
		} else if ( 
			( d[BST_V] == res->arr[0][BST_V] ) && 	// same V
			( d[BST_X] < res->arr[0][BST_X] ) 		// earlier in domain X
		) {
			res->arr[0] = d; // replace item
		} else if (
			( d[BST_V] == res->arr[0][BST_V] ) && 	// same V
			( d[BST_X] == res->arr[0][BST_X] ) &&	// same domain X
			( d[BST_Y] < res->arr[0][BST_Y] )		// lower domain Y
		) {
			res->arr[0] = d; // replace item
		}

		return 0;
	}
	exit(EXIT_FAILURE);
}




/* * * * * * * * * * * * * * * * * * * * * * * * * *  TASK 2 HELPER FUNCTIONS */

// Generates a new cell struct for task 2
cell_t* generateCell(bst_t* bst, resultsFilter_t* bounds){

	assert(bst!=NULL);
	assert(bounds!=NULL);
	cell_t* cell = (cell_t*)malloc(sizeof(cell_t));
	assert(cell != NULL);

	// Get points by searching the tree
	cell->points = res_search(bst, bounds, noCheck);
		
	// Calculate average and score if there were results found, else return null
	float* sum = (float*)calloc(bst->dim, sizeof(float));
	assert(sum!=NULL);
	cell->av = sum;

	if ((cell->points)->numEl != 0) {

		// Calculate average
		int i=0, dataIndex = 0;
		for ( dataIndex=0; dataIndex<bst->dim; dataIndex++ ) {
			for ( i=0; i<(cell->points)->numEl; i++ ) {
				sum[dataIndex] += (cell->points)->arr[i][dataIndex];
			}
			cell->av[dataIndex] = sum[dataIndex]/(cell->points)->numEl;
		}

		// Calculate score
		cell->score = CELL_SCORE( 
			cell->av[BST_X], cell->av[BST_Y], cell->av[BST_U], cell->av[BST_V] 
		);

		return cell;
	} else {
		destroyCell(cell);
		return NULL;
	}
}

// frees a list of cells
void destroyCells(cell_t* cell[], int n) {

	assert(cell!=NULL);

	int i=0;
	for (i=0;i<n;i++) { 
		if (cell[i]!=NULL) { destroyCell(cell[i]); };
	}
}

// frees memory associated with a cell
void destroyCell(cell_t* cell) {
	
	assert(cell!=NULL);
	res_free(cell->points);
	free(cell->av);
	free(cell);
}

// Prints the output of task 2
void printTask2(cell_t* cells[], int n) {
	assert(cells != NULL);

	FILE* fp = fopen(T2_CSV,FILE_REWRITE);

	fprintf(fp, T2_HEADER);

	int i=0;
	for (i=0;i<n;i++) {
		fprintf(fp, "%.6f,%.6f,%.6f,%.6f,%.6f\n",
			cells[i]->av[BST_X],
			cells[i]->av[BST_Y],
			cells[i]->av[BST_U],
			cells[i]->av[BST_V],
			cells[i]->score
			);
	}
	
	fflush(fp);
	fclose(fp);
}

// Check function used to determine if an item should be inserted, returns 1
// since we only care about whether the item is within the bounds, which is
// checked during search in data_handler.c - res_insert(..)
int noCheck(float* a, results_t* b) {
	return 1;
}

// Comparison function used to qsort cells by their score
int cellCmp(const void* a, const void* b) {

	float a_s = (*(cell_t**)a)->score;
	float b_s = (*(cell_t**)b)->score;

	return (a_s < b_s) - (a_s > b_s);
}




/* * * * * * * * * * * * * * * * * * * * * * * * * *  TASK 4 HELPER FUNCTIONS */

float* getYs_t4(bst_t* bst) {

	assert(bst!=NULL);

	FILE* fp = fopen(T4_1_CSV, FILE_REWRITE);
	assert(fp!=NULL);
	fprintf(fp, T4_HEADER);

	float* ys = (float*)calloc(T4_NUM_YS,sizeof(float));
	assert(ys!=NULL);

	resultsFilter_t bound[] = {
		{-FLT_MAX, FLT_MAX},
		{-FLT_MAX, FLT_MAX},
		{-FLT_MAX, FLT_MAX},
		{-FLT_MAX, FLT_MAX}
	};

	int i=0;
	float xVal = 0;
	for (i=0; i<T4_NUM_YS; i++) {

		xVal = i*T4_XS_INTERVAL + T4_INIT_XS;
		bound[BST_X].lo = xVal - T4_XS_TOLERANCE;
		bound[BST_X].hi = xVal + T4_XS_TOLERANCE;
		
		results_t* res = res_search(bst, bound, mvdMaxU);
		if (res->numEl == 1) {
			ys[i] = SPACING( (res->arr[0])[BST_Y] );
			fprintf(fp, "%.6f,%.6f\n",
				(res->arr[0])[BST_X],
				(res->arr[0])[BST_Y]);
			res_free(res);
		}
	}

	fflush(fp);
	fclose(fp);
	return ys;
}