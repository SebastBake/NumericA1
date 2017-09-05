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
		{MVD_THRESH, DBL_MAX},
		{-DBL_MAX, DBL_MAX},
		{-DBL_MAX, DBL_MAX},
		{-DBL_MAX, DBL_MAX}
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
	const float dx = T2_DX(resolution); // x size of a cell
	const float dy = T2_DY(resolution); // y size of a cell

	int x_i=0, y_i=0, cell_i=0;
	for ( x_i=0 ; x_i < resolution ; x_i++ ) {
		for ( y_i=0 ; y_i < resolution ; y_i++ ) {

			resultsFilter_t bound[] = {
				{T2_X_LO(x_i,dx), T2_X_HI(x_i,dx)},
				{T2_Y_LO(y_i,dy), T2_Y_HI(y_i,dy)},
				{-DBL_MAX, DBL_MAX},
				{-DBL_MAX, DBL_MAX}
			};

			// search data and generate cells
			tmpCell = generateCell(bst, bound);
			if (tmpCell != NULL) {
				cells[cell_i++] = tmpCell;
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
	float percent = 0;
	int totalPoints = bst->numNodes;
	int numPointsFound = 0;
	int i = 0;
	
	while(percent<T3_PERCENT_END) {

		resultsFilter_t searchFilter[] = {
			{-DBL_MAX, DBL_MAX},
			{-DBL_MAX, DBL_MAX},
			{-DBL_MAX, T3_THRESHOLD(i)},
			{-DBL_MAX, DBL_MAX}
		};

		results_t* res = res_search(bst, searchFilter, res_filterBoundExclude);
		numPointsFound = res->numEl;
		res_free(res);
		percent = PERCENT(numPointsFound, totalPoints);
		fprintf(fp,"%.6f,%d,%.6lf\n", T3_THRESHOLD(i), numPointsFound, percent);
		i++;
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
	
	if (res_filterBoundExclude(d, res)) {
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
	return 0;
}

int mvdMinU(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);

	if (res_filterBoundExclude(d, res)) {
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
	return 0;
}

int mvdMaxV(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);

	if (res_filterBoundExclude(d, res)) {

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
	return 0;
}

int mvdMinV(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);

	if (res_filterBoundExclude(d, res)) {

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
				( d[BST_Y] < res->arr[0][BST_Y] )		// lower Y
			) {
				res->arr[0] = d; // replace item
			}
	
			return 0;
		}
		exit(EXIT_FAILURE);

	}
	return 0;
}




/* * * * * * * * * * * * * * * * * * * * * * * * * *  TASK 2 HELPER FUNCTIONS */

// Generates a new cell struct for task 2
cell_t* generateCell(bst_t* bst, resultsFilter_t* bounds){

	assert(bst!=NULL);
	assert(bounds!=NULL);
	cell_t* cell = (cell_t*)malloc(sizeof(cell_t));
	assert(cell != NULL);

	// Get points by searching the tree
	cell->points = res_search(bst, bounds, res_filterBoundInclude);
		
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
		cell->score = T2_SCORE( 
			cell->av[BST_X], cell->av[BST_Y], cell->av[BST_U], cell->av[BST_V] );

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
		if (cell[i]!=NULL) {
			destroyCell(cell[i]);
		};
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
	assert(fp!=NULL);
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

	float* ys = (float*)calloc(T4_NUM_Y,sizeof(float));
	assert(ys!=NULL);

	int i=0;
	for (i=0; i<T4_NUM_Y; i++) {

		resultsFilter_t bound[] = {
			{T4_X_LO(i), T4_X_HI(i)},
			{-DBL_MAX, DBL_MAX},
			{-DBL_MAX, DBL_MAX},
			{-DBL_MAX, DBL_MAX}
		};
		
		results_t* res = res_search(bst, bound, t4_checker);
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

// Used in search by task 4 to insert items into the results array
int t4_checker(float* d, results_t* res) {
	
	assert(res->numEl == 1 || res->numEl == 0);
	
	if (res_filterBoundInclude(d, res)) {
		if (res->numEl == 0) {
			return 1; // insert item if it's the first result
		} else if (res->numEl == 1) {

			double midX = AVERAGE(res->filter[BST_X].hi, res->filter[BST_X].lo);
			int closerX = fabs(midX-d[BST_X]) < fabs(midX-res->arr[0][BST_X]);
			int sameX = d[BST_X] == res->arr[0][BST_X];
			int maxU = d[BST_U] > res->arr[0][BST_U];
			int sameU = d[BST_U] == res->arr[0][BST_U];
			int minY = d[BST_Y] < res->arr[0][BST_Y];

			// replace priority: closest x --> maximum u --> minimum y
			if (closerX) {
				res->arr[0] = d; // replace previous result
			} else if (sameX && maxU) {
				res->arr[0] = d; // replace previous result
			} else if (sameX && sameU && minY) {
				res->arr[0] = d; // replace previous result
			}
			return 0;
	
		}
		exit(EXIT_FAILURE);
	}
	return 0;	
}
