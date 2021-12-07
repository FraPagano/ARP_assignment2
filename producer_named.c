/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "parameters.h"

int main(int argc, char *argv[])
{
    int fd, data[MAX];

    int noelement_to_send = atoi(argv[1]);

    fd = open(PIPE_PATH, O_WRONLY);

    for (int i = 0; i < MAX; i++)
    {
        data[i] = rand() % 10;
    }

    send_start_time();

    for (int i = 0; i < noelement_to_send; i++)
    {
        write(fd, &data[i], sizeof(int));

        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX;
            i = 0;
        }
    }
    sleep(1);
    close(fd);
    return 0;
}