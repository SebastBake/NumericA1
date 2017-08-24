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

	resultsFilter_t searchFilter[] = {
		{MVD_THRESH, MAXFLOAT},
		{-MAXFLOAT, MAXFLOAT},
		{-MAXFLOAT, MAXFLOAT},
		{-MAXFLOAT, MAXFLOAT}
	};

	results_t* minU = res_search(bst, searchFilter, mvdMinU);
	results_t* maxU = res_search(bst, searchFilter, mvdMaxU);
	results_t* minV = res_search(bst, searchFilter, mvdMinV);
	results_t* maxV = res_search(bst, searchFilter, mvdMaxV);

	bst_printKey(bst, fp);
	bst_printData(bst, minU->arr[0], fp);
	bst_printData(bst, maxU->arr[0], fp);
	bst_printData(bst, minV->arr[0], fp);
	bst_printData(bst, maxV->arr[0], fp);

	res_free(minU);
	res_free(maxU);
	res_free(minV);
	res_free(maxV);

	fflush(fp);
	fclose(fp);
}

void coarsegrid(bst_t* bst, int resolution) {

	assert(bst!=NULL);

	int n = resolution*resolution;
	resultsFilter_t** bound = initBound(n, bst->dim);
	cell_t* cells[n];
	float xRes = (GRID_X_MAX - GRID_X_MIN) / resolution;
	float yRes = (GRID_Y_MAX - GRID_Y_MIN) / resolution;

	int xCell=0, yCell=0, cellsIndex=0;
	for (xCell=0; xCell<resolution; xCell++) {
		for ( yCell=0; yCell<resolution; yCell++) {

			cellsIndex = yCell + resolution*xCell;

			bound[cellsIndex][BST_X].lo = GRID_X_MIN + xCell*xRes;
			bound[cellsIndex][BST_X].hi = GRID_X_MIN + (xCell+1)*xRes;
			bound[cellsIndex][BST_Y].lo = GRID_Y_MIN + yCell*yRes;
			bound[cellsIndex][BST_Y].hi = GRID_Y_MIN + (yCell+1)*yRes;
			
			cells[cellsIndex] = generateCell(bst, bound[cellsIndex]);
		}
	}
	sortCells(cells, n);
	printTask2(cells, n);
	destroyCells(cells, n);
	free(bound);
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
			{-MAXFLOAT, MAXFLOAT},
			{-MAXFLOAT, MAXFLOAT},
			{-MAXFLOAT, thresh},
			{-MAXFLOAT, MAXFLOAT}
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
	float* yheight = getYs_t4(bst);
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

int mvdMaxU(float* d, results_t* res) {
	
	assert(res->numEl <= 1 && res->numEl >= 0);
	
	if (res->numEl == 0) {
		return 1; // insert item
	} else if (res->numEl == 1) {
		int biggerU = d[BST_U] > res->arr[0][BST_U];
		int sameU = d[BST_U] ==res->arr[0][BST_U];
		int smallerY = d[BST_Y] < res->arr[0][BST_Y];
		if ( biggerU ) { res->arr[0] = d; }
		if ( sameU && smallerY) { res->arr[0] = d; }
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




/* * * * * * * * * * * * * * * * * * * * * * * * * *  TASK 2 HELPER FUNCTIONS */

cell_t* generateCell(bst_t* bst, resultsFilter_t* bounds){

	assert(bst!=NULL);
	assert(bounds!=NULL);
	
	cell_t* cell = (cell_t*)malloc(sizeof(cell_t));
	assert(cell != NULL);

	// Get points by searching the tree
	cell->points = res_search(bst, bounds, noCheck);

	// Calculate average
	float* sum = (float*)calloc(bst->dim, sizeof(float));
	assert(sum!=NULL);
	cell->av = sum;

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
}

void destroyCells(cell_t* cell[], int n) {

	assert(cell!=NULL);

	int i=0;
	for (i=0;i<n;i++) { 
		res_free(cell[i]->points);
		free(cell[i]->av);
		free(cell[i]);
	}
}

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

int noCheck(float* a, results_t* b) {
	return 1;
}

resultsFilter_t** initBound(int n, int dim) {
	resultsFilter_t** bound = 
		(resultsFilter_t**)malloc(n * sizeof(resultsFilter_t*));
	assert(bound!=NULL);

	int i=0;
	for (i=0; i<n; i++){
		bound[i] = (resultsFilter_t*)malloc(dim*sizeof(resultsFilter_t));
		assert(bound[i] != NULL);
		int j=0;
		for (j=0; j<dim; j++) {
			bound[i][j].lo = -MAXFLOAT;
			bound[i][j].hi = MAXFLOAT;
		}
	}

	return bound;
}

// Recursive selection sort applied to an array of cells
void sortCells(cell_t* cell[], int n) {
	
	assert(cell!=NULL);

	int i, max_index;
	float max = -MAXFLOAT;

	if(n==1){
		return;
	}

	for(i=0; i<n-1; i++){
		if(cell[i]->score > max) {

			max = cell[i]->score;
			max_index = i;
		}
	}

	// Swap cells
	cell_t* temp;
	temp = cell[i];
	cell[i] = cell[max_index];
	cell[max_index] = temp;

	sortCells(cell, n-1);
}




/* * * * * * * * * * * * * * * * * * * * * * * * * *  TASK 4 HELPER FUNCTIONS */

float* getYs_t4(bst_t* bst) {

	FILE* fp = fopen(T4_1_CSV, FILE_REWRITE);
	assert(fp!=NULL);
	fprintf(fp, T4_HEADER);

	resultsFilter_t** bound = initBound(T4_NUM_YS, bst->dim);
	float* ys = (float*)calloc(T4_NUM_YS,sizeof(float));
	assert(ys!=NULL);

	int i=0;
	float xVal = 0;
	for (i=0; i<T4_NUM_YS; i++) {

		xVal = i*T4_XS_INTERVAL + T4_INIT_XS;
		bound[i][BST_X].lo = xVal - T4_XS_TOLERANCE;
		bound[i][BST_X].hi = xVal + T4_XS_TOLERANCE;;
		
		results_t* res = res_search(bst, bound[i], mvdMaxU);
		assert(res->numEl == 1);
		ys[i] = SPACING( (res->arr[0])[BST_Y] );
		fprintf(fp, "%.0f,%.6f\n",xVal, (res->arr[0])[BST_Y]);
		res_free(res);
	}

	free(bound);
	fflush(fp);
	fclose(fp);
	return ys;
}