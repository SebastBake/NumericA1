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

#define MVD_THRESH 20.0
#define MVD_

void maxveldiff(bst_t* bst);
void coarsegrid(bst_t* bst, int resolution);
void velstat(bst_t* bst);
void wakevis(bst_t* bst);

#endif
