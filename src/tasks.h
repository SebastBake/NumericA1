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

#define MVD_THRESH 20

#define T2_HEADER "x,y,u,v,S\n"
#define T2_SCORE(x,y,u,v) 100*(sqrt(pow(u,2)+pow(v,2))/sqrt(pow(x,2)+pow(y,2)))
#define T2_DX(resolution) (70 - 10) / resolution
#define T2_DY(resolution) (20 - (-20)) / resolution
#define T2_X_LO(i, dx) (10 + i*dx) 
#define T2_X_HI(i, dx) (10 + i*dx + dx) 
#define T2_Y_LO(i, dy) (-20 + i*dy) 
#define T2_Y_HI(i, dy) (-20 + i*dy + dy) 


#define T3_HEADER "threshold,points,percentage\n"
#define T3_INIT_THRESH 0.5
#define T3_THRESH_INTERVAL 0.1
#define T3_THRESHOLD(i) (double)(0.5 + (i*0.1))
#define T3_PERCENT_END 100.0
#define PERCENT(numer, denom) 100.0*((float)numer/(float)denom)

#define T4_HEADER "x,y_h\n"
#define T4_NUM_Y 12
#define T4_X_LO(i) (double)(i*5.0 + 10.0 - 0.05)
#define T4_X_HI(i) (double)(i*5.0 + 10.0 + 0.05)
#define AVERAGE(x,y) (double)((x+y)/2)
#define SPACING(y) ceil(10.0 * fabs(y))

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
void destroyCell(cell_t* cell);
void printTask2(cell_t* cells[], int n);
int cellCmp(const void* a, const void* b);

// Filter function for task 3
int t3_filter(float* d, results_t* res);

// Used in Task 4
float* getYs_t4(bst_t* bst);

// Task functions
void maxveldiff(bst_t* bst);
void coarsegrid(bst_t* bst, int resolution);
void velstat(bst_t* bst);
void wakevis(bst_t* bst);

#endif
