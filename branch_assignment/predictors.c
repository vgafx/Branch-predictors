
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#include "predictors.h"
#include "framework.h"


//Definitions for the prediction state
#define STRONG_TAKEN 4
#define WEAK_TAKEN 3
#define WEAK_NOT_TAKEN 2
#define STRONG_NOT_TAKEN 1

//mask definitions for gshare predictor (4)
#define MASK_ONE 1
#define MASK_TWO 3
#define MASK_THREE 7
#define MASK_FOUR 15
#define MASK_FIVE 31
#define MASK_SIX 63
#define MASK_SEVEN 127
#define MASK_EIGHT 255
#define MASK_NINE 511
#define MASK_TEN 1023
#define MASK_ELEVEN 2047
#define MASK_TWELVE 4095
#define MASK_THIRTEEN 8191
#define MASK_FOURTEEN 16383
#define MASK_FIFTEEN 32767
#define MASK_SIXTEEN 65535



/* Random prediction */
void random_predictor() {
    /* Variable to store the prediction you predict for this branch. */
    bool prediction = false;

    /* Variable to store the the address of the branch. */
    uint32_t addr = 0;

    /*
     * Variable to store the actual branch result
     * (obtained from the predictor library)
     */
    bool actual = false;

    /*
     * Prediction loop, until tracefile is empty.
     */
    while (predictor_getState() != DONE) {

        /* Get the next branch address from the state machine. */
        if (predictor_getNextBranch(&addr) != 0) {
            fprintf(stderr, "ERROR: \"predictor_getNextBranch()\" called in "\
                   "a state it shouldn't be called!\n");
        }

        /* Do a random prediction */
        prediction = rand() & 1;

        /*
         * Feed the prediction to the state machine, and get the actual
         * result back.
         */
        if (predictor_predict(prediction, &actual) != 0) {
            fprintf(stderr, "ERROR: \"predictor_predict()\" called in "\
                    " a state it shouldn't be called\n");
        }
    }
}


/* Predict always true (taken) or false */
void always_x(bool p) {
    /* Variable to store the prediction you predict for this branch. */
    bool prediction = p;

    /* Variable to store the the address of the branch. */
    uint32_t addr = 0;

    /*
     * Variable to store the actual branch result
     * (obtained from the predictor library)
     */
    bool actual = false;

    /*
     * Prediction loop, until tracefile is empty.
     */
    while (predictor_getState() != DONE) {

        /* Get the next branch address from the state machine. */
        if (predictor_getNextBranch(&addr) != 0) {
            fprintf(stderr, "ERROR: \"predictor_getNextBranch()\" called in "\
                   "a state it shouldn't be called!\n");
        }

        /* Do a prediction */
        prediction = p;

        /*
         * Feed the prediction to the state machine, and get the actual
         * result back.
         */
        if (predictor_predict(prediction, &actual) != 0) {
            fprintf(stderr, "ERROR: \"predictor_predict()\" called in "\
                    " a state it shouldn't be called\n");
        }
    }
}


/* Implement assignment 1 here */
void assignment_1_simple() {
    /* Variable to store the prediction the model predicts for this branch. */
    bool prediction = false;

    /* Variable to store the the address of the branch. */
    uint32_t addr = 0;

    /*Variable to represent the 4 prediction states 
     */
 	uint32_t prediction_state = WEAK_TAKEN;

    /*
     * Variable to store the actual branch result
     * (obtained from the predictor library)
     */
    bool actual = false;

    /*
     * Prediction loop, until tracefile is empty.
     */
    while (predictor_getState() != DONE) {

        /* Get the next branch address from the state machine. */
        if (predictor_getNextBranch(&addr) != 0) {
            fprintf(stderr, "ERROR: \"predictor_getNextBranch()\" called in "\
                   "a state it shouldn't be called!\n");
        }

        /* Setting the prediction according to the prediction state */

        if(prediction_state==STRONG_TAKEN || prediction_state==WEAK_TAKEN){
        	prediction=true;
		}
        else if(prediction_state==WEAK_NOT_TAKEN || prediction_state==STRONG_NOT_TAKEN){
        	prediction=false;
        }

        /*
         * Feed the prediction to the state machine, and get the actual
         * result back.
         */
        if (predictor_predict(prediction, &actual) != 0) {
            fprintf(stderr, "ERROR: \"predictor_predict()\" called in "\
                    " a state it shouldn't be called\n");
        }

        /*
         *After obtaining the actual result, adjust the prediction state.
         */
         //;
         switch (prediction_state){

				case STRONG_TAKEN:
				prediction_state = (actual) ? STRONG_TAKEN : WEAK_TAKEN;
				break;

				case WEAK_TAKEN:
				prediction_state = (actual) ? STRONG_TAKEN : STRONG_NOT_TAKEN;
				break;

				case WEAK_NOT_TAKEN:
				prediction_state = (actual) ? STRONG_TAKEN : STRONG_NOT_TAKEN;
				break;

				case STRONG_NOT_TAKEN:
				prediction_state = (actual) ? WEAK_NOT_TAKEN : STRONG_NOT_TAKEN;

				break;
         }

    }


}



/* Implement assignment 2 here */
void assignment_2_GAg(int history) {

if (history>28){
	printf("History size too large! Setting history=28\n");
	history=28;
	}

	double queue_size=0;
	queue_size = pow(2.0,history);//get the total size for the array 2^k
	uint64_t hist = (int) queue_size;

	/*Create the array that will hold the history of predictions*/
    int* pattern_history = malloc(hist * sizeof(*pattern_history));
 	/* Variable to store the prediction the model predicts for this branch. */
    bool prediction = false;

    /* Variable to store the the address of the branch. */
    uint32_t addr = 0;

    /*Set prediction state to its initial state*/
 	uint32_t prediction_state = 0;

 	uint32_t temp_BHR=0;
    /*
     * Variable to store the actual branch result
     * (obtained from the predictor library)
     */
    bool actual = false;

    for (int i=0; i<hist; i++){
    	pattern_history[i]=WEAK_TAKEN;
    }

    /*
     * Prediction loop, until tracefile is empty.
     */
    while (predictor_getState() != DONE) {

        /* Get the next branch address from the state machine. */
        if (predictor_getNextBranch(&addr) != 0) {
            fprintf(stderr, "ERROR: \"predictor_getNextBranch()\" called in "\
                   "a state it shouldn't be called!\n");
        }

        /*Decide on the prediction based on the pattern history*/
        if(pattern_history[prediction_state]==STRONG_TAKEN || pattern_history[prediction_state]==WEAK_TAKEN){
        	prediction=true;
		}
        else if(pattern_history[prediction_state]==STRONG_NOT_TAKEN || pattern_history[prediction_state]==WEAK_NOT_TAKEN){
        	prediction=false;
        }

        temp_BHR = prediction_state;
        //Adjust the BHR after the prediction is made
        prediction_state = prediction_state <<1; //shift
        prediction_state |= (prediction) ? 1:0; //append
        prediction_state &=  (hist-1); 
 
        /*
         * Feed the prediction to the state machine, and get the actual
         * result back.
         */
        if (predictor_predict(prediction, &actual) != 0) {
            fprintf(stderr, "ERROR: \"predictor_predict()\" called in "\
                    " a state it shouldn't be called\n");
        }

        /*Adjust the pattern history based on the actual outcome of the branch*/
        switch (pattern_history[temp_BHR]){
     
		case STRONG_TAKEN:
			pattern_history[temp_BHR] = (actual) ? STRONG_TAKEN : WEAK_TAKEN;
			break;

		case WEAK_TAKEN:
			pattern_history[temp_BHR] = (actual) ? STRONG_TAKEN : WEAK_NOT_TAKEN;
			break;

		case WEAK_NOT_TAKEN:
			pattern_history[temp_BHR] = (actual) ? WEAK_TAKEN : STRONG_NOT_TAKEN;
			break;

		case STRONG_NOT_TAKEN:
			pattern_history[temp_BHR] = (actual) ? WEAK_NOT_TAKEN : STRONG_NOT_TAKEN;
			break;

        }

        // adjusting the prediction state based on the actual outcome of the branch
        // if the prediction was not correct the prediction_state (BHR) needs to be corrected
        if (prediction !=actual ){
        	prediction_state ^= (1<<0);
        }
    }
    free(pattern_history);
}




/* Implement assignment 3 here */
void assignment_3_SAs(int history, int n_sets) {

    /* Sanity checks for input parameters */
	if (history>20){
		printf("History size too large! Setting history=20\n");
		history=20;
	}else if(n_sets<2){//if set number is 1 it is essentially a global and not per set predictor. 
		printf("Input n_sets param too small for simulation! Setting n_sets=2 \n");
		n_sets=2;
	}else if(n_sets>128){
		printf("Input n_sets param too large for simulation! Setting n_sets=128 \n");
		n_sets=128;
	}

	/*The number of sets is rounded UP to a power of 2*/
	n_sets--;
    n_sets |= n_sets >> 1;
    n_sets |= n_sets >> 2;
    n_sets |= n_sets >> 4;
    n_sets |= n_sets >> 8;
    n_sets |= n_sets >> 16;
	n_sets++; 
	printf("n_sets(bitwise) is: %d\n", n_sets);

	double total_size=0;
	total_size = pow(2.0,history);//get the total size for the array 2^k
	uint64_t hist = (int) total_size;

	/* Branch History Table to hold BHRs for each set (1st level) */
	/*Init all BHRs to 0*/
	int branch_history_table[n_sets];
	for (int h=0; h<n_sets; h++){
		branch_history_table[h]=0;
	}

	/*Create the Per Set Pattern History Tables (2nd level)*/
    int** pattern_history_tables = malloc(n_sets * sizeof(pattern_history_tables));

    for (int y=0; y<n_sets; y++){
    	pattern_history_tables[y] = malloc(hist * sizeof(*pattern_history_tables[y]));
    }

 	/* Variable to store the prediction the model predicts for this branch. */
    bool prediction = false;

    /* Variable to store the the address of the branch. */
    uint32_t addr = 0;

 	/*Variable for temporarily storing BHRs*/
 	uint32_t current_BHR=0;

 	/*Variable used to distinguish the set that the address corresponds to*/
 	uint32_t set_index=0;

 	/*Variable used to index the pattern history tables*/
 	uint32_t pattern_table_index=0;

    /*
     * Variable to store the actual branch result
     * (obtained from the predictor library)
     */
    bool actual = false;

    /*Initialize all elements of the pattern history tables*/
    for(int i=0; i<n_sets; i++){
    	for (int j=0; j<hist; j++){
    		pattern_history_tables[i][j]= WEAK_TAKEN;
    	}
    }

    /*
     * Prediction loop, until tracefile is empty.
     */
    while (predictor_getState() != DONE) {

        /* Get the next branch address from the state machine. */
        if (predictor_getNextBranch(&addr) != 0) {
            fprintf(stderr, "ERROR: \"predictor_getNextBranch()\" called in "\
                   "a state it shouldn't be called!\n");
        }

        /*Get the set index by ignoring the low-order bits from the current address*/
        set_index= (addr >> 10) & (n_sets-1); //swift and mask with n_sets-1

        /*Generate the index that wil be used for addressing the Pattern History tables
         *Or it with the set_index 
         *mask with n_sets-1 to get the index
         */
        pattern_table_index = addr | set_index;
        pattern_table_index &=n_sets-1;

        /*Use the history register associated with the current address*/
        current_BHR = branch_history_table[set_index];

        /*Decide on the prediction based on the pattern history table of the current set*/
        if(pattern_history_tables[pattern_table_index][current_BHR]==STRONG_TAKEN || pattern_history_tables[pattern_table_index][current_BHR]==WEAK_TAKEN){
        	prediction=true;
		}
        else if(pattern_history_tables[pattern_table_index][current_BHR]==STRONG_NOT_TAKEN || pattern_history_tables[pattern_table_index][current_BHR]==WEAK_NOT_TAKEN){
        	prediction=false;
        }

        /*
         * Feed the prediction to the state machine, and get the actual
         * result back.
         */
        if (predictor_predict(prediction, &actual) != 0) {
            fprintf(stderr, "ERROR: \"predictor_predict()\" called in "\
                    " a state it shouldn't be called\n");
        }

        /*Adjust the pattern history based on the actual outcome of the branch*/
        switch (pattern_history_tables[pattern_table_index][current_BHR]){
     
		case STRONG_TAKEN:
			pattern_history_tables[pattern_table_index][current_BHR] = (actual) ? STRONG_TAKEN : WEAK_TAKEN;
			break;

		case WEAK_TAKEN:
			pattern_history_tables[pattern_table_index][current_BHR] = (actual) ? STRONG_TAKEN : WEAK_NOT_TAKEN;
			break;

		case WEAK_NOT_TAKEN:
			pattern_history_tables[pattern_table_index][current_BHR] = (actual) ? WEAK_TAKEN : STRONG_NOT_TAKEN;
			break;

		case STRONG_NOT_TAKEN:
			pattern_history_tables[pattern_table_index][current_BHR] = (actual) ? WEAK_NOT_TAKEN : STRONG_NOT_TAKEN;
			break;
        }

        // adjusting the current BHR based on the actual outcome of the branch
        branch_history_table[set_index] = branch_history_table[set_index] <<1;
        branch_history_table[set_index] |= (actual) ? 1:0;
        branch_history_table[set_index] &= (hist-1);

    }
    /*Free up space*/
    for (int r=0; r<n_sets; r++){
    free(pattern_history_tables[r]);
    }
    free(pattern_history_tables);
}




/* Assignment 4: gshare predictor */
void assignment_4_your_own(int history, int num_bits) {
	/*Restrict input*/
	if (history>28){
		printf("History size too large! Setting history=28\n");
		history=28;
	}
	/*Variable used as a bitmask for the x low order bits of the address and the history*/
	uint32_t bitmask=0;

	//Restrict the number of bits
	if (num_bits<1 || num_bits>16){
		printf("Number of input bits not within accepted range. Bit length should be within the range 1 to 16 \n");
		num_bits = 8;//set a default
		printf("Setting default num_bits= %u  low order bits to use for this simulation.\n",num_bits);
	}

	/*In some cases where num_bits is larger than history, an index will be generated that is much larger than 2^history
	 *and this will lead to segmentation fault (index out of boundaries). Therefore history size should be adjusted accordingly 
	 *REMOVE the following limitation to test specific scenarios.
	 */
	if (history<num_bits){
	history=num_bits;
	}

	/*Setting the bitmask variable according to user input.*/
	switch (num_bits){
		case 1:
			bitmask=MASK_ONE;
		break;

		case 2:
			bitmask=MASK_TWO;
		break;

		case 3:
			bitmask=MASK_THREE;
		break;

		case 4:
			bitmask=MASK_FOUR;
		break;

		case 5:
			bitmask=MASK_FIVE;
		break;

		case 6:
			bitmask=MASK_SIX;
		break;

		case 7:
			bitmask=MASK_SEVEN;
		break;

		case 8:
			bitmask=MASK_EIGHT;
		break;

		case 9:
			bitmask=MASK_NINE;
		break;

		case 10:
			bitmask=MASK_TEN;
		break;

		case 11:
			bitmask=MASK_ELEVEN;
		break;

		case 12:
			bitmask=MASK_TWELVE;
		break;

		case 13:
			bitmask=MASK_THIRTEEN;
		break;

		case 14:
			bitmask=MASK_FOURTEEN;
		break;

		case 15:
			bitmask=MASK_FIFTEEN;
		break;

		case 16:
			bitmask=MASK_SIXTEEN;
		break;
	}

	double queue_size=0;
	queue_size = pow(2.0,history);//get the total size for the array 2^k
	uint64_t hist = (int) queue_size;

	/*Create the array that will hold the history of predictions*/
    int* pattern_history = malloc(hist * sizeof(*pattern_history));
 	/* Variable to store the prediction the model predicts for this branch. */
    bool prediction = false;

    /* Variable to store the the address of the branch. */
    uint32_t addr = 0;

    /*Set Branch History Register to its initial state*/
 	uint32_t BHR = 0;

 	/*The index tha will be used to lookup the pattern history table*/
 	uint32_t gshare_index=0;

 	/*Variables for temporarily storing the x low bits of addresses and history*/
 	uint32_t x_bit_addr=0;
 	uint32_t x_bit_bhr=0;
    /*
     * Variable to store the actual branch result
     * (obtained from the predictor library)
     */
    bool actual = false;

    for (int i=0; i<hist; i++){
    	pattern_history[i]=WEAK_TAKEN;
    }

    /*
     * Prediction loop, until tracefile is empty.
     */
    while (predictor_getState() != DONE) {

        /* Get the next branch address from the state machine. */
        if (predictor_getNextBranch(&addr) != 0) {
            fprintf(stderr, "ERROR: \"predictor_getNextBranch()\" called in "\
                   "a state it shouldn't be called!\n");
        }

        /*Decide on the prediction based on the pattern history*/
        if(pattern_history[gshare_index]==STRONG_TAKEN || pattern_history[gshare_index]==WEAK_TAKEN){
        	prediction=true;
		}
        else if(pattern_history[gshare_index]==STRONG_NOT_TAKEN || pattern_history[gshare_index]==WEAK_NOT_TAKEN){
        	prediction=false;
        }
 
        /*
         * Feed the prediction to the state machine, and get the actual
         * result back.
         */
        if (predictor_predict(prediction, &actual) != 0) {
            fprintf(stderr, "ERROR: \"predictor_predict()\" called in "\
                    " a state it shouldn't be called\n");
        }

        /*Adjust the pattern history based on the actual outcome of the branch*/
        switch (pattern_history[gshare_index]){
     
		case STRONG_TAKEN:
			pattern_history[gshare_index] = (actual) ? STRONG_TAKEN : WEAK_TAKEN;
			break;

		case WEAK_TAKEN:
			pattern_history[gshare_index] = (actual) ? STRONG_TAKEN : WEAK_NOT_TAKEN;
			break;

		case WEAK_NOT_TAKEN:
			pattern_history[gshare_index] = (actual) ? WEAK_TAKEN : STRONG_NOT_TAKEN;
			break;

		case STRONG_NOT_TAKEN:
			pattern_history[gshare_index] = (actual) ? WEAK_NOT_TAKEN : STRONG_NOT_TAKEN;
			break;

        }


        // adjusting the BHR based on the actual outcome of the branch
        BHR = BHR <<1; //shift
        BHR |= (actual) ? 1:0; //append
        BHR &=  (hist-1);
        //get the last x bits
        x_bit_addr = addr & bitmask;
        x_bit_bhr = BHR & bitmask;
        //XOR them to generate an index
        gshare_index = x_bit_addr ^ x_bit_bhr;

    }
    free(pattern_history);
}





/* This implementation is a variation of the first assignment (simple branch predictor).
 * This predictor differs in that the prediction state, transitions only 1 state after a single prediction.
 * The simple branch predictor implemented in assignment 1 will transition 2 states at a time when it is
 * at a 'weak' state and a miss prediction is made (ex: WEAK_TAKEN-> STRONG_NOT_TAKEN after a missprediction)
 *
 */
void bonus_1() {
  /* Variable to store the prediction the model predicts for this branch. */
    bool prediction = false;

    /* Variable to store the the address of the branch. */
    uint32_t addr = 0;

    /*Variable to represent the 4 prediction states (DEFINED at top)*/
 	uint32_t prediction_state = 0;

    /*
     * Variable to store the actual branch result
     * (obtained from the predictor library)
     */
    bool actual = false;

    /*
     * Prediction loop, until tracefile is empty.
     */
    while (predictor_getState() != DONE) {

        /* Get the next branch address from the state machine. */
        if (predictor_getNextBranch(&addr) != 0) {
            fprintf(stderr, "ERROR: \"predictor_getNextBranch()\" called in "\
                   "a state it shouldn't be called!\n");
        }

        /* Setting the prediction according to the prediction state */

        if(prediction_state==STRONG_TAKEN || prediction_state==WEAK_TAKEN){
        	prediction=true;
		}
        else if(prediction_state==WEAK_NOT_TAKEN || prediction_state==STRONG_NOT_TAKEN){
        	prediction=false;
        }

        /*
         * Feed the prediction to the state machine, and get the actual
         * result back.
         */
        if (predictor_predict(prediction, &actual) != 0) {
            fprintf(stderr, "ERROR: \"predictor_predict()\" called in "\
                    " a state it shouldn't be called\n");
        }

        /*
         *After obtaining the actual result, adjust the prediction state.
         *This model transitions only 1 state at a time.
         */
         //;
         switch (prediction_state){

				case STRONG_TAKEN:
					prediction_state = (actual) ? STRONG_TAKEN : WEAK_TAKEN;
					break;

				case WEAK_TAKEN:
					prediction_state = (actual) ? STRONG_TAKEN : WEAK_NOT_TAKEN;
					break;

				case WEAK_NOT_TAKEN:
					prediction_state = (actual) ? WEAK_TAKEN : STRONG_NOT_TAKEN;
					break;

				case STRONG_NOT_TAKEN:
					prediction_state = (actual) ? WEAK_NOT_TAKEN : STRONG_NOT_TAKEN;
					break;
         }

    }
}

/* Bonus: Change these parameters to your needs */
void bonus_2() {
  //always_x(false);
     return;
}



