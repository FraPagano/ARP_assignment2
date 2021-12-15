/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "../parameters/parameters.h"
#include "../timing/timing.c"

FILE *log_file; // log file

/*FUNCTION HEADER*/
void logPrint(char *string);

/*FUNCTION*/
void logPrint(char *string)
{
    /* Function to print on log file adding time stamps. */
    time_t ltime = time(NULL);
    fprintf(log_file, "%.19s: %s", ctime(&ltime), string);
    fflush(log_file);
}

/*MAIN*/
int main(int argc, char *argv[])
{
    int fd;        // File descriptor
    int data[MAX]; // Array of data to send

    log_file = fopen("../log_file/log.txt", "a");

    int noelement_to_send = atoi(argv[1]);

    fd = atoi(argv[2]);

    for (int i = 0; i < MAX; i++)
    {
        // Filling the array with random integer values
        data[i] = rand() % 10;
    }

    send_start_time(); // Start time instant

    for (int i = 0; i < noelement_to_send; i++)
    {
        CHECK(write(fd, &data[i], sizeof(int)));
        /*  We established a maximum array size of 250000 elements. Therefore we send
            the same data of the same array multiple times. When the whole array has
            been sent we make the for loop restart until all the data are sent.    */
        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX; // Reduce the number of element to send
            i = 0;                                       // Restart the for loop
        }
    }

    logPrint("Producer Unamed    :Data written\n");
    sleep(1);
    CHECK(close(fd));
    return 0;
}