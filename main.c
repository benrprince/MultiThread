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

// Buffer 1 and mutex variables
char buffer_1[MAX_INPUT+1];
int count_1 = 0;
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

// Buffer 2 and mutex variables
char buffer_2[MAX_INPUT+1];
int count_2 = 0;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3 and mutex variables
char buffer_3[MAX_INPUT+1];
int count_3 = 0;
int buff3_index = 0;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

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

void get_buff_3(char* buff) {

    pthread_mutex_lock(&mutex_3);
    while (count_3 == 0) {

        pthread_cond_wait(&full_3, &mutex_3);

    }

    strcpy(buff, buffer_3);

    count_3--;

    pthread_mutex_unlock(&mutex_3);

}

void *writeOutput(void *args) {

    char buff[MAX_INPUT];
    get_buff_3(buff);
    int bufferLength = strlen(buff);
    char tempString[81];
    int tempIndex = 0;

    for (int i=buff3_index; i <bufferLength; i++) {
        
        tempString[tempIndex] = buff[i];
        tempIndex++;
        buff3_index++;

        if (tempIndex == 80) {

            printf("%s\n", tempString);
            fflush(stdout);

            tempIndex = 0;
            buff3_index++;

        }
    }
}

void put_buff_3(char input[]) {

    // Lock Mutex
    pthread_mutex_lock(&mutex_3);
    // Fill buffer
    strcat(buffer_3, input);
    count_3++;
    // Signal that buffer is full
    pthread_cond_signal(&full_3);
    // Unlock Mutex
    pthread_mutex_unlock(&mutex_3);

}

void get_buff_2(char* buff) {

    pthread_mutex_lock(&mutex_2);
    while (count_2 == 0) {

        pthread_cond_wait(&full_2, &mutex_2);

    }

    strcpy(buff, buffer_2);

    count_2--;

    pthread_mutex_unlock(&mutex_2);

}

void *plusSign(void *args) {

    char buff[MAX_INPUT];
    get_buff_2(buff);
    int bufferLength = strlen(buff);

    for(int i=0; i <bufferLength; i++) {

        if((buff[i] == '+') && (buff[i] == '+')) {

            buff[i] = '^';
            for(int j=i+1; j <bufferLength; j++) {
                buff[j] = buff[j+1];
            }
        }
    }
    //printf("**plusSign function**%s", buff);     // This is for testing. Remove!!
    put_buff_3(buff);

}

void put_buff_2(char input[]) {

    // Lock Mutex
    pthread_mutex_lock(&mutex_2);
    // Fill buffer
    strcat(buffer_2, input);
    count_2++;
    // Signal that buffer is full
    pthread_cond_signal(&full_2);
    // Unlock Mutex
    pthread_mutex_unlock(&mutex_2);

}

void get_buff_1(char* buff) {

    pthread_mutex_lock(&mutex_1);
    while (count_1 == 0) {

        pthread_cond_wait(&full_1, &mutex_1);

    }

    strcpy(buff, buffer_1);

    count_1--;

    pthread_mutex_unlock(&mutex_1);
    
}

void *lineSeparator(void *args) {

    char buff[MAX_INPUT];
    get_buff_1(buff);
    int bufferLength = strlen(buff);

    for(int i = 0; i <bufferLength; i++) {

        if(buff[i] == '\n') {
            buff[i] = ' ';
        }
    }
    //printf("**lineSeparator function**%s", buff);      // This is for testing. Remove!!
    put_buff_2(buff);

}


void put_buff_1(char input[]) {

    // Lock Mutex
    pthread_mutex_lock(&mutex_1);
    // Fill buffer
    strcat(buffer_1, input);
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
    *   Desc: Gets input from stdin
    ***********************************************/

    char buffer[MAX_INPUT];

    // If there is a redirected file
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        
        // Get size of current buffer for clearing later
        size_t buffSize = strlen(buffer);
        // Check for STOP
        stop_flag = stopCheck(buffer);
        // Add to input
        if (stop_flag != 0) {
            break;
        }

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