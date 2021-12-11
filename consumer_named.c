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
    log_file = fopen("./log.txt", "a");
    int noelement_to_read = atoi(argv[1]);

    fd = CHECK(open(PIPE_PATH, O_RDONLY));

    for (int i = 0; i < noelement_to_read; i++)
    {
        CHECK(read(fd, &data, sizeof(int)));

        if (i % (noelement_to_read / 100) == 0)
        {
            loading_bar(i, noelement_to_read);
        }

        B[i] = data;

        if (i == MAX)
        {
            noelement_to_read = noelement_to_read - MAX;
            i = 0;
        }
    }

    send_end_time();
    logPrint("Producer Named    :Data read\n");
    sleep(1);
    CHECK(close(fd));
    return 0;
}