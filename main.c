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

// Buffer 1 variables
char buffer_1[MAX_INPUT+1];
//int buffer_1_idx = 0;
int line_count = 0;

//Buffer 2 variables
char buffer_2[MAX_INPUT+1];

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

void plusSign() {

    int bufferLength = strlen(buffer_2);
    char updatedString[bufferLength];

    for(int i=0; i <bufferLength; i++) {

        if((buffer_2[i] == '+') && (buffer_2[i] == '+')) {

            buffer_2[i] = '^';
            for(int j=i+1; j <bufferLength; j++) {
                buffer_2[j] = buffer_2[j+1];
            }
        }
    }

    printf("%s", buffer_2);

}

void put_buff_2(char input[]) {

    // Probably will put the mutex lock here!!
    strcat(buffer_2, input);


}

void lineSeparator() {

    int bufferLength = strlen(buffer_1);

    for(int i = 0; i <bufferLength; i++) {

        if(buffer_1[i] == '\n') {
            buffer_1[i] = ' ';
        }
    }

    //printf("%s", buffer_1);
    put_buff_2(buffer_1);

}


void put_buff_1(char input[]) {

    // Probably will put the mutex lock here!!
    strcat(buffer_1, input);
    line_count++;

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



}



int main(int argc, char *argv[]) {

    // Each of these will be a thread

    getInput();
    lineSeparator();
    plusSign();


    return 0;
}