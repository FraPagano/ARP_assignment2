/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "parameters.h"

int main(int argc, char *argv[])
{
    int data, fd, B[MAX];

    int noelement_to_read = atoi(argv[1]);

    fd = open(PIPE_PATH, O_RDONLY);

    for (int i = 0; i < noelement_to_read; i++)
    {
        read(fd, &data, sizeof(int));

        B[i] = data;

        if (i == MAX)
        {
            noelement_to_read = noelement_to_read - MAX;
            i = 0;
        }
    }

    send_end_time();

    sleep(1);
    close(fd);
    return 0;
}