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

#define TASK_1_CSV "task1.csv"
#define TASK_2_CSV "task2.csv"
#define TASK_3_CSV "task3.csv"
#define TASK_4_CSV "task4.csv"

#define FILE_REWRITE "w"

#define BST_X 0
#define BST_Y 1
#define BST_U 2
#define BST_V 3
#define BST_DIM 4

#define MVD_THRESH 20.0
#define CELL_SCORE(x,y,u,v) 100*sqrt( u*u + v*v )/sqrt( x*x + y*y )
#define TASK_2_HEADER "x,y,u,v,S\n"
#define TASK_2_DATA "%.6f,%.6f,%.6f,%.6f\n"
#define GRID_X_MIN 10
#define GRID_X_MAX 70
#define GRID_Y_MIN -20
#define GRID_Y_MAX 20

// Cell data type for task 2
typedef struct {
    results_t* points;
    float* av;
    float score;
} cell_t;

// Filter functions used during searching in maxveldiff(..) - Task 1
int mvdMaxU(float* d, results_t* res);
int mvdMinU(float* d, results_t* res);
int mvdMaxV(float* d, results_t* res);
int mvdMinV(float* d, results_t* res);

// Used in Task 2
cell_t* generateCell(bst_t* bst, resultsFilter_t* bounds);
void destroyCell(cell_t* cell);
void sortCells(cell_t* cells[], int n);
void printTask2(cell_t* cells[], int n);
int noCheck(float* a, results_t* b);
resultsFilter_t** initBound(int n, int dim);

// Task functions
void maxveldiff(bst_t* bst);
void coarsegrid(bst_t* bst, int resolution);
void velstat(bst_t* bst);
void wakevis(bst_t* bst);

#endif
