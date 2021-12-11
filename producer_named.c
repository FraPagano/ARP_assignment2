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
    int fd, data[MAX];
    log_file = fopen("./log.txt", "a");
    int noelement_to_send = atoi(argv[1]);

    fd = CHECK(open(PIPE_PATH, O_WRONLY));

    for (int i = 0; i < MAX; i++)
    {
        data[i] = rand() % 10;
    }

    send_start_time();

    for (int i = 0; i < noelement_to_send; i++)
    {
        CHECK(write(fd, &data[i], sizeof(int)));

        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX;
            i = 0;
        }
    }
    logPrint("Producer Named    :Data written\n");
    sleep(1);
    CHECK(close(fd));
}