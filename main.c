/*********************************************************************
 *      Author: Ben Prince
 *      Assignment 4: Multi-threaded Producer Consumer Pipeline
 *      Date: 05/172021
 *  Sources:
 * ******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_INPUT 1000
#define NUM_ITEMS 49
char* buffer_1[MAX_INPUT+1];
int buffer_1_idx = 0;

int more_input = 1;
int stop_flag = 0;

int stopCheck(char buffer[]) {

    if(strncmp(buffer, "STOP\n", 5) == 0) {
        return 1;
    }
    else {
        return 0;
    }

}

void getUserInput(char* input) {

    char buffer[MAX_INPUT];

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        
        // Get size of current buffer for clearing later
        size_t buffSize = strlen(buffer);
        // Check for STOP
        stop_flag = stopCheck(buffer);
        // Add to input
        strcat(input, buffer);

        if (stop_flag != 0) {
            break;
        }

        // Clear the buffer for the next fgets
        for (int i = 0; i < buffSize; i++) {
            buffer[i] = '\0';
        }

    }

}

void put_buff_1(char input[]) {

    buffer_1[buffer_1_idx] = input;          // Start Here!!
    printf("%s", buffer_1[buffer_1_idx]);
    buffer_1_idx = buffer_1_idx + 1;

}

void getInput() {
    /***********************************************
    *   Params: NA
    *   Returns: NA
    *   Desc: Gets input from stdin
    ***********************************************/

    char buffer[MAX_INPUT];

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        
        // Get size of current buffer for clearing later
        size_t buffSize = strlen(buffer);
        // Check for STOP
        stop_flag = stopCheck(buffer);
        // Add to input
        // strcat(input, buffer);

        put_buff_1(buffer);

        if (stop_flag != 0) {
            break;
        }

        // Clear the buffer for the next fgets
        for (int i = 0; i < buffSize; i++) {
            buffer[i] = '\0';
        }

    }



    // getUserInput(input);
    // printf("%s", input);                    // Here for testing purposes, REMOVE LATER
    // fflush(stdout);                         // Here for testing purposes, REMOVE LATER


}



int main(int argc, char *argv[]) {

    // char *input[MAX_INPUT];

    // while(more_input == 1) {

    //     getInput();

    // }

    getInput();


    return 0;
}