
#ifndef PREDICTORS_H
#define PREDICTORS_H

#include <stdbool.h>

/* Random prediction */
void random_predictor();

/* Predict always true (taken) or false */
void always_x(bool p);

/* Implement assignment 1 here */
void assignment_1_simple();

/* Implement assignment 2 here */
void assignment_2_GAg(int history);

/* Implement assignment 4 here */
void assignment_3_SAs(int history, int n_sets);

/* Assignment 4: Change these parameters to your needs */
void assignment_4_your_own(int history, int bitmask);

/* Bonus: Change these parameters to your needs */
void bonus_1();

/* Bonus: Change these parameters to your needs */
void bonus_2();

#endif
