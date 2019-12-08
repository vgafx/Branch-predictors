#ifndef H_FRAMEWORK
#define H_FRAMEWORK

#include <stdbool.h>
#include <stdint.h>

/*
 * State of the program state machine
 *
 * INIT --> setup --> ADDR
 * ADDR --> getNextBranch --> PRED
 * PRED --> predict --> ADDR
 * PRED --> EOF --> DONE
 *
 * All other state transitions are invalid
 *
 */
typedef enum {INIT, ADDR, PRED, DONE} state;


/*
 * Setup the state machine enviroment.
 *
 * Specify the filename for the tracefile.
 *
 */
int predictor_setup(const char *filename, const char *predictor_name);

/*
 * Cleanup state machine after finishing prediction
 */
void predictor_cleanup();

/*
 * Return the current state of the program.
 */
const state predictor_getState();

/*
 * Get the address of the next branch in the program.
 *
 * Returns 0 or succes.
 * Returns -1 in case of invalid State
 */
const int predictor_getNextBranch(uint32_t *addr);

/*
 * Predict the current branch to be taken or not taken.
 *
 * @param prediction Your prediction (true = taken)
 * @param *result Actual outcome of branch (true = taken)
 *
 * Return 0 on success
 * Returns -1 in case of invalid State
 */
const int predictor_predict(const bool prediction, bool *result);

/*
 * Generate basic statistics
 */
void predictor_printBasicStatistics(int csv);

void predictor_printAdvancedStatistics(int csv);

void predictor_printScore();

#endif
