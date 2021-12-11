/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "parameters.h"

FILE *log_file;
void logPrint(char *string);
void logPrint(char *string)
{
    /* Function to print on log file adding time stamps. */

    time_t ltime = time(NULL);
    fprintf(log_file, "%.19s: %s", ctime(&ltime), string);
    fflush(log_file);
}

int main(int argc, char *argv[])
{
    int data, fd, B[MAX];
    int j = 0;
    log_file = fopen("./log.txt", "a");
    int total_elements = atoi(argv[1]);
    int noelement_to_read = total_elements;
    fd = atoi(argv[2]);

    for (int i = 0; i < noelement_to_read; i++)
    {
        CHECK(read(fd, &data, sizeof(int)));

        if (j % (total_elements / 100) == 0)
        {
            loading_bar(j, total_elements);
        }

        B[i] = data;

        if (i == MAX)
        {
            noelement_to_read = noelement_to_read - MAX;
            i = 0;
        }
        j++;
    }

    send_end_time();
    logPrint("Producer Unnamed    :Data read\n");
    sleep(1);
    CHECK(close(fd));
    return 0;
}