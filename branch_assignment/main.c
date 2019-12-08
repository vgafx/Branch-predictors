/* 
Vasilis

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "framework.h"
#include "predictors.h"


#define FILENAME_LENGTH 250

/* Print usage text */
void usage(){
    printf("Branch predictor Framework PSA course:\n");
    printf("\n");
    printf(" Global options:\n");
    printf("  -a        Print advanced statistics\n");
    printf("  -b        Print basic statistics\n");
    printf("  -c        Print csv format instead of table\n");
    printf("  -h        Help - Print this text\n");
    printf("  -i <FILE> Tracefile to be read (mandatory)\n");
    printf("  -s        Print score only\n");
    printf("\n");
    printf(" Predictor types:\n");
    printf("  -f        Always false (Predict all branches not taken)\n");
    printf("  -r        Predict random\n");
    printf("  -t        Always true (Predict all branches taken)\n");
    printf("  -1        Assignment 1: Simple\n");
    printf("  -2        Assignment 2: GAg\n");
    printf("  -3        Assignment 3: SAs\n");
    printf("  -4        Assignment 4: Your own\n");
    printf("  -5        Bonus 1: Your own\n");
    printf("  -6        Bonus 2: Your own\n");
    printf("\n");
    printf(" Predictor options:\n");
    printf("  -x        Random seed for random predictor (-r)\n");
    printf("  -k        History length for Assignment 2, 3 (and 4)\n");
    printf("  -n        Number of sets for Assignment 3 (and 4)\n");
    /* FIXME: Add your own options here. */

    printf("\n");

}

int main(int argc, char **argv) {

    /* Options (init). */
    char c;
    char filename[FILENAME_LENGTH];
    char *predictor_name;

    int statistics_advanced = 0;
    int statistics_basic = 0;
    int statistics_score = 0;
    int print_csv = 0;

    int random_seed = 42;
    int n = 1;
    int k = 1;

    char prediction_algorithm = 'r';
    predictor_name = "random";

    /* Handle command line options. */
    while ((c = getopt(argc, argv, "hi:abcstfr123456x:k:n:")) != -1) {
        switch(c) {
            /* Help. */
            case 'h':
                usage();
                return 0;
            /* Input */
            case 'i':
                strncpy(filename, optarg, FILENAME_LENGTH);
                break;

            /* Output - Statistics */
            case 'a':
                statistics_advanced = 1;
                break;
            case 'b':
                statistics_basic = 1;
                break;
            case 'c':
                print_csv = 1;
                break;
            case 's':
                statistics_score = 1;
                break;

            /* Prediction algorithms. */
            case 't':
                prediction_algorithm = 't';
                predictor_name = "always_true";
                break;
            case 'f':
                prediction_algorithm = 'f';
                predictor_name = "always_false";
                break;
            case 'r':
                prediction_algorithm = 'r';
                predictor_name = "random";
                break;
            case '1':
                prediction_algorithm = '1';
                predictor_name = "Assignment_1_Simple";
                break;
            case '2':
                prediction_algorithm = '2';
                predictor_name = "Assignment_2_GAg";
                break;
            case '3':
                prediction_algorithm = '3';
                predictor_name = "Assignment_3_SAa";
                break;
            case '4':
                prediction_algorithm = '4';
                predictor_name = "Assignment_4_Your_Own";
                break;
            case '5':
                prediction_algorithm = '5';
                predictor_name = "Bonus_1";
                break;
            case '6':
                prediction_algorithm = '6';
                predictor_name = "Bonus_2";
                break;

            /* Prediction options. */
            case 'x':
                random_seed = atoi(optarg);
                break;
            case 'k':
                k = atoi(optarg);
                break;
            case 'n':
                n = atoi(optarg);
                break;
            /* FIXME: Add your own options here? */

            /* Unknown values. */
            case '?':
                if(isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n\n", optopt);
                }
                else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n\n",
                            optopt);
                }
            default:
                usage();
                return 1;
        }
    }

    /* Seed 'randomness' */
    srand(random_seed);

    /* Setup state machine. This will search for the -i option. */
    if (predictor_setup(filename, predictor_name) != 0) {
        fprintf(stderr, "Problem initializing predictor. Exiting...\n\n");
        usage();
        return -1;
    }

    /*
     * Now run the prediction algorithm
     */
    switch(prediction_algorithm) {
        case 'r':
            random_predictor();
            break;
        case 't':
        	printf("Next Trace \n");
            always_x(true);
            break;
        case 'f':
            always_x(false);
            break;
        case '1':
            assignment_1_simple();
            break;
        case '2':
            assignment_2_GAg(k);
            break;
        case '3':
            assignment_3_SAs(k, n);
            break;
        case '4':
            assignment_4_your_own(k,n);
            break;
        case '5':
            bonus_1();
            break;
        case '6':
            bonus_2();
            break;
        default:
            usage();
            return -1;
    }

    /* Print basic statistics. */
    if (statistics_basic) {
        predictor_printBasicStatistics(print_csv);
    }

    /* Print advanced statistics. */
    if (statistics_advanced) {
        predictor_printAdvancedStatistics(print_csv);
    }

    /* Print score. */
    if (statistics_score) {
        predictor_printScore();
    }

    /* Cleanup. */
    predictor_cleanup();

    return 0;
}
