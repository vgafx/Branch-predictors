#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "framework.h"

//Increment of arrays
#define INC 100

/**
 * Data structure to store branch predictions
 */
typedef struct {
    uint32_t address;   ///< Address of the branch
    size_t size;        ///< Size of the bitvector
    size_t pos;         ///< Current position in the bitvector
    char *pred;         ///< Bitvector of predictions
    char *real;         ///< Bitvector of actual branch results
} data;

/**
 * Data structure to keep an array of branch predictions
 */
typedef struct {
    size_t size;        ///< Size of the array
    size_t pos;         ///< Current latest position in the array
    data *data;         ///< Array of data structs
} elements;

elements e;

///File pointer to input file
FILE *_fp = NULL;
char *_filename = NULL;
char *_predictor_name = NULL;

///Current address in the state machine
uint32_t _addr = 0;
bool _taken = false;

state _state = INIT;

void resize_data(data *d)
{
    d->pred = realloc(d->pred, sizeof(char) * (d->size + INC));
    d->real = realloc(d->real, sizeof(char) * (d->size + INC));

    for (size_t i = d->size; i < d->size + INC; i++) {
        d->pred[i] = 0;
        d->real[i] = 0;
    }
    //Check
    d->size += INC;
}

void resize_elements(elements *e)
{
    e->data = realloc(e->data, sizeof(data) * (e->size + INC));
    for (size_t i = e->size; i < e->size + INC; i++) {
        e->data[i].address = 0;
        e->data[i].size = 1;
        e->data[i].pos = 0;
        e->data[i].pred = calloc(1, sizeof(char));
        e->data[i].real = calloc(1, sizeof(char));
    }
    e->size += INC;
}


void init_elements(elements *e)
{
    e->size = 1;
    e->pos = 0;
    e->data = malloc(sizeof(data));
    e->data[0].address = 0;
    e->data[0].size = 1;
    e->data[0].pos = 0;
    e->data[0].pred = calloc(1, sizeof(char));
    e->data[0].real = calloc(1, sizeof(char));
}

void addPrediction(uint32_t address, bool pred, bool real)
{
    /*
     * Check if we have already seen this branch and if
     * so update the prediction and real bitvector.
     */
    for (size_t i = 0; i < e.pos; i++) {
        if (e.data[i].address == address) {
            data *d = &(e.data[i]);

            if (d->size <= d->pos) {
                resize_data(d);
            }
            d->pred[d->pos] = pred ? 1 : 0;
            d->real[d->pos] = real ? 1 : 0;
            d->pos++;
            return;
        }
    }

    /*
     * We have not seen this branch before. See if there is room in the array
     * And add the first entry in the branch.
     */
    if (e.size <= e.pos) {
        resize_elements(&e);
    }

    data *d = &(e.data[e.pos]);

    d->address = address;
    d->pred[d->pos] = pred ? 1 : 0;
    d->real[d->pos] = real ? 1 : 0;
    d->pos++;
    e.pos++;

}

void parseNextLine();

int predictor_setup(const char *filename, const char *predictor_name)
{
    int str_size = 0;

    //Try to open file
    _fp = fopen(filename, "r");
    if (_fp == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }

    //Init stats data structures
    init_elements(&e);

    _state = ADDR;

    str_size = strlen(filename) + 1;
    _filename = malloc(str_size * sizeof(char));
    strncpy(_filename, filename, str_size);

    str_size = strlen(predictor_name) + 1;
    _predictor_name = malloc(str_size * sizeof(char));
    strncpy(_predictor_name, predictor_name, str_size);

    parseNextLine();

    return 0;
}

void predictor_cleanup()
{
    for (size_t i = 0; i < e.size; i++) {
        free(e.data[i].pred);
        free(e.data[i].real);
    }
    free(e.data);

    free(_filename);
    free(_predictor_name);

    fclose(_fp);
}

/*
 * Parse the next line
 */
void parseNextLine()
{
    //Read a new line and fill the stuff
    char *line = NULL;
    size_t n = 0;
    size_t res = getline(&line, &n, _fp);

    /* Check if we have reached EOF */
    if (res == -1) {
        _state = DONE;
        free(line);
        return;
    }

    /* Parse address of branch */
    char *l = strtok(line, ",");
    _addr = atoi(l);

    /* Parse taken or not taken */
    l = strtok(NULL, ",");
    _taken = false;
    if (atoi(l) == 1) {
        _taken = true;
    }

    free(line);
}

const state predictor_getState()
{
    return _state;
}

const int predictor_getNextBranch(uint32_t *addr)
{
    //Check if we are in a valid state
    if (_state != ADDR) {
        return -1;
    }

    _state = PRED;

    *addr = _addr;
    return 0;
}

const int predictor_predict(const bool prediction, bool *result)
{
    //Check if we are in a valid state
    if (_state != PRED) {
        return -1;
    }

    _state = ADDR;

    *result = _taken;

    //Update predictions
    addPrediction(_addr, prediction, _taken);

    //Parse next line
    parseNextLine();

    return 0;
}

void __predictor_printBasicStatistics(int score_only, int csv){

    if (_state != DONE) {
        return;
    }

    size_t hits = 0;
    size_t miss = 0;
    size_t taken = 0;
    size_t nottaken = 0;
    size_t pt = 0;
    size_t pnt = 0;
    size_t counter = 0;

    for (size_t i = 0; i < e.pos; i++) {
        data *d = &(e.data[i]);
        for (size_t j = 0; j < d->pos; j++) {
            d->pred[j] == d->real[j] ? hits++ : miss++;
            d->real[j] == 1 ? taken++ : nottaken++;
            d->pred[j] == 1 ? pt++ : pnt++;
            counter++;
        }
    }

    double d_counter = ((double)counter / (double)counter) * 100.0;
    double d_pos = ((double)e.pos / (double)counter) * 100.0;
    double d_taken = ((double)taken / (double)counter) * 100.0;
    double d_pt = ((double)pt / (double)counter) * 100.0;
    double d_nottaken = ((double)nottaken / (double)counter) * 100.0;
    double d_pnt = ((double)pnt / (double)counter) * 100.0;
    double d_hits = ((double)hits / (double)counter) * 100.0;
    double d_miss = ((double)miss / (double)counter) * 100.0;

    if (score_only) {
        printf("%s - %s: %8.4f\n", _filename, _predictor_name, d_hits);
    }
    else if (csv) {
        printf("Branches, Unique Branches, Unique Branches %%, Taken, "
               "Taken %%, Predicted Taken, , Predicted Taken %%, Not Taken, "
               "Not Taken %%, Predicted Not-Taken, Predicted Not-Taken %%, "
               "Predicted right, Predicted right %%, Predicted wrong, "
               "Predicted wrong %%\n");
        printf("%8zu, %8zu, %8.4f, %8zu, %8.4f, %8zu, %8.4f, %8zu, %8.4f, "
               "%8zu, %8.4f, %8zu, %8.4f, %8zu, %8.4f\n",
               counter, e.pos, d_pos, taken, d_taken, pt, d_pt, nottaken,
               d_nottaken, pnt, d_pnt, hits, d_hits, miss, d_miss);
    }
    else {
        printf("Predictor:           %s\n", _predictor_name);
        printf("Tracefile:           %s\n", _filename);
        printf("Branches:            %8zu  %8.4f%%\n", counter, d_counter);
        printf("Unique Branches:     %8zu  %8.4f%%\n", e.pos, d_pos);
        printf("Taken:               %8zu  %8.4f%%\n", taken, d_taken);
        printf("Predicted Taken:     %8zu  %8.4f%%\n", pt, d_pt);
        printf("Not taken:           %8zu  %8.4f%%\n", nottaken, d_nottaken);
        printf("Predicted Non-Taken: %8zu  %8.4f%%\n", pnt, d_pnt);
        printf("Predicted right:     %8zu  %8.4f%%\n", hits, d_hits);
        printf("Predicted wrong:     %8zu  %8.4f%%\n", miss, d_miss);
        printf("\n");
    }

}

void predictor_printBasicStatistics(int csv) {
    __predictor_printBasicStatistics(0, csv);
}

void predictor_printScore(){
    __predictor_printBasicStatistics(1, 0);
}

void predictor_printAdvancedStatistics(int csv) {

    if (_state != DONE) {
        return;
    }

    if (csv) {
        printf("Branch, Counter, Taken, Taken %%, Predicted Taken, "
               "Predicted Taken %%, Not taken, Not taken %%, Predicted right, "
               "Predicted right %%, Predicted wrong, Predicted wrong %%\n");
    }
    else {
        printf(" -------------------------------------------------------------"
               "--------------------------------------------------------------"
               "---------------------- \n");
        printf("|   Branch   |      Counter     |      Taken       |"
          "     P. Taken     |     Not Taken    |   P. Not taken   |"
          "        OK        |      Wrong       |\n");
        printf("|-------------------------------------------------------------"
               "--------------------------------------------------------------"
               "----------------------|\n");
    }

    for (size_t i = 0; i < e.pos; i++) {
        size_t hits = 0;
        size_t miss = 0;
        size_t taken = 0;
        size_t nottaken = 0;
        size_t pt = 0;
        size_t pnt = 0;
        size_t counter = 0;

        data *d = &(e.data[i]);

        for (size_t j = 0; j < d->pos; j++) {
            d->pred[j] == d->real[j] ? hits++ : miss++;
            d->real[j] == 1 ? taken++ : nottaken++;
            d->pred[j] == 1 ? pt++ : pnt++;
            counter++;
        }

        double d_counter = ((double)counter / (double)counter) * 100.0;
        double d_taken = ((double)taken / (double)counter) * 100.0;
        double d_pt = ((double)pt / (double)counter) * 100.0;
        double d_nottaken = ((double)nottaken / (double)counter) * 100.0;
        double d_pnt = ((double)pnt / (double)counter) * 100.0;
        double d_hits = ((double)hits / (double)counter) * 100.0;
        double d_miss = ((double)miss / (double)counter) * 100.0;

        if (csv) {
            printf("%10u, %8zu, %8zu, %8.4f, %8zu, %8.4f, %8zu, %8.4f, "
                   "%8zu, %8.4f, %8zu, %8.4f, %8zu, %8.4f\n",
                   d->address, counter, taken, d_taken, pt, d_pt, nottaken,
                   d_nottaken, pnt, d_pnt, hits, d_hits, miss, d_miss);
        }
        else {
            printf("|");
            printf(" %10u |", d->address);
            printf(" %8zu %6.2f%% |", counter, d_counter);
            printf(" %8zu %6.2f%% |", taken, d_taken);
            printf(" %8zu %6.2f%% |", pt, d_pt);
            printf(" %8zu %6.2f%% |", nottaken, d_nottaken);
            printf(" %8zu %6.2f%% |", pnt, d_pnt);
            printf(" %8zu %6.2f%% |", hits, d_hits);
            printf(" %8zu %6.2f%% |", miss, d_miss);
            printf("\n");
        }
    }

    if (!csv) {
        printf(" -------------------------------------------------------------"
               "--------------------------------------------------------------"
               "---------------------- \n");
    }

}

