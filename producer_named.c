/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int fd;
char *fifo = "/tmp/fifo_p_to_c";
struct timespec start;
int main(int argc, char *argv[])
{
    const int size = atoi(argv[1]);
    int data[size];

    for (int i = 0; i < size; i++)
    {
        data[i] = rand() % 10;
    }

    fd = open(fifo, O_WRONLY);

    clock_gettime(CLOCK_REALTIME, &start);
    write(fd, data, sizeof(int) * size);

    return 0;
}