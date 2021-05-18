/*********************************************************************
 *      Author: Ben Prince
 *      Assignment 4: Multi-threaded Producer Consumer Pipeline
 *      Date: 05/172021
 *  Sources: The basic producer/consumer buffer and thread design came from the
 *           example code (6_5_prod_cons_pipeline.c) shared on the assignment.
 * ******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_INPUT 1000
#define NUM_ITEMS 50

// Buffer 1 and mutex variables
char buffer_1[NUM_ITEMS+1][MAX_INPUT];
int count_1 = 0;
int prod_idx_1 = 0;
int con_idx_1 = 0;
int stop_flag_1 = 0;
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

// Buffer 2 and mutex variables
char buffer_2[NUM_ITEMS+1][MAX_INPUT];
int count_2 = 0;
int prod_idx_2 = 0;
int con_idx_2 = 0;
int stop_flag_2 = 0;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3 and mutex variables
char buffer_3[NUM_ITEMS+1][MAX_INPUT];
int count_3 = 0;
int prod_idx_3 = 0;
int con_idx_3 = 0;
int stop_flag_3 = 0;
char tempString[MAX_INPUT];
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

int stopCheck(char buffer[]) {
    /***********************************************
    *   Params: char []
    *   Returns: 0 for false, 1 for true
    *   Desc: gets from buffer 3 for the output_t
    *         thread
    ***********************************************/
    // check if the first 5 chars are STOP and a new line char
    if(strncmp(buffer, "STOP\n", 5) == 0) {
        return 1;
    }
    else {
        return 0;
    }

}

void get_buff_3(char* buff) {
    /***********************************************
    *   Params: char*
    *   Returns: NA
    *   Desc: gets from buffer 3 for the output_t
    *         thread
    ***********************************************/
    // lock mutex
    pthread_mutex_lock(&mutex_3);
    // if count_3 is 0 wait until for full_3 signal
    while (count_3 == 0) {

        pthread_cond_wait(&full_3, &mutex_3);

    }

    // copy buffer_3 into buff
    strcpy(buff, buffer_3[con_idx_3]);

    // Increment consumer index and decrement count_3
    con_idx_3 = con_idx_3 + 1;
    count_3--;
    
    // Unlock mutex
    pthread_mutex_unlock(&mutex_3);

}

void *writeOutput(void *args) {
    /***********************************************
    *   Params: NA
    *   Returns: NA
    *   Desc: function for the output_t thread, 
    *         print lines of 80 chars
    ***********************************************/

    char buff[MAX_INPUT];
    // Loop for max number of items possible
    for (int i = 0; i < NUM_ITEMS; i++) {

        // get string from buffer_3
        get_buff_3(buff);

        // Concat buff onto end of tempString and get tempString len
        strcat(tempString, buff);
        int tempLength = strlen(tempString);

        // Initialize tempBuffer and copy tempString
        char tempBuffer[MAX_INPUT];
        strcpy(tempBuffer, tempString);

        // While we have 80 chars we want to print the line
        while (tempLength > 80) {
            
            // Loop for each char and print
            for (int i=0; i < 80; i++) {

                // Print out each character until 80 chars
                printf("%c", tempString[i]);
                fflush(stdout);
                
                // Replace these chars with endline markers
                tempString[i] = '\0';

            }
            // Finish with a new line char
            printf("\n");

            // Shift chars down to the beginning of tempString
            int tempIndex = 0;
            for (int i=80; i < tempLength; i++) {

                tempBuffer[tempIndex] = tempString[i];
                tempIndex = tempIndex + 1;

            }
            // Place a null char at the end of the remaining text in tempBuffer
            // Get the new tempLength
            tempBuffer[tempIndex] = '\0';
            tempLength = strlen(tempBuffer);

            // Copy tempBuffer into tempString
            strcpy(tempString, tempBuffer);

        }

        // check for stop flag from previous thread
        if((stop_flag_3 != 0) && (count_3 == 0)) {
            break;
        }
    }
}

void put_buff_3(char input[]) {
    /***********************************************
    *   Params: char []
    *   Returns: NA
    *   Desc: Sets buffer 3 with input from plus_t
    *         thread
    ***********************************************/

    // Lock Mutex
    pthread_mutex_lock(&mutex_3);
    // Fill buffer
    strcpy(buffer_3[prod_idx_3], input);
    // Increment variables for buffer 3
    prod_idx_3 = prod_idx_3 + 1;
    count_3++;
    // Signal that buffer is full
    pthread_cond_signal(&full_3);
    // Unlock Mutex
    pthread_mutex_unlock(&mutex_3);

}

void get_buff_2(char* buff) {
    /***********************************************
    *   Params: char*
    *   Returns: NA
    *   Desc: gets from buffer 2 for the plus_t
    *         thread
    ***********************************************/
    // Lock mutex
    pthread_mutex_lock(&mutex_2);
    // if count_2 is 0 wait until for full_2 signal
    while (count_2 == 0) {

        pthread_cond_wait(&full_2, &mutex_2);

    }

    // Copy the buffer string into buff pointer
    strcpy(buff, buffer_2[con_idx_2]);

    // Increment consumer index and decrement count_2
    con_idx_2 = con_idx_2 + 1;
    count_2--;
    
    // Unlock mutex
    pthread_mutex_unlock(&mutex_2);

}

void *plusSign(void *args) {
    /***********************************************
    *   Params: NA
    *   Returns: NA
    *   Desc: function for the plus_t thread, 
    *         changes "++" to '^'
    ***********************************************/

    char buff[MAX_INPUT];
    // Loop for max amount of items possible
    for (int i = 0; i < NUM_ITEMS; i++) {

        // fill buff with string from buffer_2 and get len
        get_buff_2(buff);
        int bufferLength = strlen(buff);

        // for len of buff change ++ to ^
        for(int i=0; i <bufferLength; i++) {
            
            // if there is a + and the next char is +
            // change to ^ and back up string to get rid of second +
            if((buff[i] == '+') && (buff[i+1] == '+')) {

                buff[i] = '^';
                for(int j=i+1; j <bufferLength; j++) {
                    buff[j] = buff[j+1];
                }
            }
        }
        
        // add to buffer_3 for next thread
        put_buff_3(buff);

        // check for stop flag from previous thread
        if((stop_flag_2 != 0) && (count_2 == 0)) {
            stop_flag_3 = 1;
            break;
        }
    }
}

void put_buff_2(char input[]) {
    /***********************************************
    *   Params: char []
    *   Returns: NA
    *   Desc: Sets buffer 2 with input from separator_t
    *         thread
    ***********************************************/

    // Lock Mutex
    pthread_mutex_lock(&mutex_2);
    // Fill buffer
    strcpy(buffer_2[prod_idx_2], input);

    // Increment buffer index and count_1
    prod_idx_2 = prod_idx_2 + 1;
    count_2++;

    // Signal that buffer is full
    pthread_cond_signal(&full_2);
    // Unlock Mutex
    pthread_mutex_unlock(&mutex_2);

}

void get_buff_1(char* buff) {
    /***********************************************
    *   Params: char*
    *   Returns: NA
    *   Desc: gets from buffer 1 for the separator_t
    *         thread
    ***********************************************/
    // Lock mutex
    pthread_mutex_lock(&mutex_1);
    // if count_1 is 0 wait until for full_1 signal
    while (count_1 == 0) {

        pthread_cond_wait(&full_1, &mutex_1);

    }

    // Copy the buffer string into buff pointer
    strcpy(buff, buffer_1[con_idx_1]);

    // Increment consumer index and decrement count_1
    con_idx_1 = con_idx_1 + 1;
    count_1--;

    // Unlock mutex
    pthread_mutex_unlock(&mutex_1);
    
}

void *lineSeparator(void *args) {
    /***********************************************
    *   Params: NA
    *   Returns: NA
    *   Desc: function for the separator_t thread, 
    *         changes '\n' to spaces
    ***********************************************/

    char buff[MAX_INPUT];
    // Loop for max amount of items possible
    for (int i = 0; i < NUM_ITEMS; i++) {
        // Fill buff with buffer_1 data and get len
        get_buff_1(buff);
        int bufferLength = strlen(buff);

        // Loop for len of buff, changing end lines to spaces
        for(int i = 0; i <bufferLength; i++) {

            if(buff[i] == '\n') {
                buff[i] = ' ';
            }
        }

        // Put results into buffer_2 for next thread
        put_buff_2(buff);

        // Check for stop flag from first thread
        if((stop_flag_1 != 0) && (count_1 == 0)) {
            stop_flag_2 = 1;
            break;
        }
    }

}


void put_buff_1(char* input) {
    /***********************************************
    *   Params: input
    *   Returns: NA
    *   Desc: Sets buffer 1 with input from input_t
    *         thread
    ***********************************************/

    // Lock Mutex
    pthread_mutex_lock(&mutex_1);
    // Fill buffer
    strcpy(buffer_1[prod_idx_1], input);

    // Increment buffer index and count_1
    prod_idx_1 = prod_idx_1 + 1;
    count_1++;

    // Signal that buffer is full
    pthread_cond_signal(&full_1);
    // Unlock Mutex
    pthread_mutex_unlock(&mutex_1);

}

void *getInput(void *args) {
    /***********************************************
    *   Params: NA
    *   Returns: NA
    *   Desc: Function for the input_t thread, gets
    *         input from stdin
    ***********************************************/

    char buffer[MAX_INPUT];
    // Get text from stdin and place in buffer_1
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    
        // Get size of current buffer for clearing later
        size_t buffSize = strlen(buffer);
        // Check for STOP/set stop_flag_1 to let next thread know of STOP
        stop_flag_1 = stopCheck(buffer);
        if (stop_flag_1 != 0) {
            break;
        }

        // Add to input
        put_buff_1(buffer);

        // Clear the buffer for the next fgets
        for (int i = 0; i < buffSize; i++) {
            buffer[i] = '\0';
        }

    }
}



int main(int argc, char *argv[]) {

    // Initialize threads
    pthread_t input_t, separator_t, plus_t, output_t;
    // Create the threads
    pthread_create(&input_t, NULL, getInput, NULL);
    pthread_create(&separator_t, NULL, lineSeparator, NULL);
    pthread_create(&plus_t, NULL, plusSign, NULL);
    pthread_create(&output_t, NULL, writeOutput, NULL);

    pthread_join(input_t, NULL);
    pthread_join(separator_t, NULL);
    pthread_join(plus_t, NULL);
    pthread_join(output_t, NULL);

    return EXIT_SUCCESS;
}