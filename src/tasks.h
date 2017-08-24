/***************************************************************************
 *
 *   File        : tasks.h
 *   Student Id  : 757931
 *   Name        : Sebastian Baker
 *
 ***************************************************************************/

#include "data_handler.h"

#ifndef TASKS_H
#define TASKS_H

#define T1_CSV "task1.csv"
#define T2_CSV "task2.csv"
#define T3_CSV "task3.csv"
#define T4_1_CSV "task4_1.csv"
#define T4_2_CSV "task4_2.csv"

#define FILE_REWRITE "w"

#define BST_X 0
#define BST_Y 1
#define BST_U 2
#define BST_V 3
#define BST_DIM 4

#define MVD_THRESH 20.0
#define CELL_SCORE(x,y,u,v) 100*sqrt( u*u + v*v )/sqrt( x*x + y*y )
#define T2_HEADER "x,y,u,v,S\n"
#define GRID_X_MIN 10
#define GRID_X_MAX 70
#define GRID_Y_MIN -20
#define GRID_Y_MAX 20
#define T3_HEADER "threshold,points,percentage\n"
#define T3_INIT_THRESH 0.5
#define T3_THRESH_INTERVAL 0.1
#define T3_PERCENT_END 100
#define PERCENT(numer, denom) 100*numer/denom
#define T4_HEADER "x,y_h\n"
#define T4_NUM_YS 12
#define T4_INIT_XS 10.0
#define T4_XS_INTERVAL 5.0
#define T4_XS_TOLERANCE 0.05
#define SPACING(y) ceil(10 * fabs(y))


// Cell data type for task 2
typedef struct {
    results_t* points;
    float* av;
    float score;
} cell_t;

// Filter functions used during searching in Task 1
int mvdMaxU(float* d, results_t* res);
int mvdMinU(float* d, results_t* res);
int mvdMaxV(float* d, results_t* res);
int mvdMinV(float* d, results_t* res);

// Used in Task 2
cell_t* generateCell(bst_t* bst, resultsFilter_t* bounds);
void destroyCells(cell_t* cell[], int n);
void sortCells(cell_t* cells[], int n);
void printTask2(cell_t* cells[], int n);
int noCheck(float* a, results_t* b);
resultsFilter_t** initBound(int n, int dim);

// Used in Task 4
float* getYs_t4(bst_t* bst);

// Task functions
void maxveldiff(bst_t* bst);
void coarsegrid(bst_t* bst, int resolution);
void velstat(bst_t* bst);
void wakevis(bst_t* bst);

#endif
