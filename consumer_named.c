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
struct timespec end;

int main(int argc, char *argv[])
{
    const int size = atoi(argv[1]);
    int data[size];
    fd = open(fifo, O_RDONLY);

    for (int i = 0; i < size; i++)
    {
        read(fd, &data[i], sizeof(int));
    }

    clock_gettime(CLOCK_REALTIME, &end);
    for (int i = 0; i < size; i++)
    {
        printf("%d", data[i]);
        fflush(stdout);
    }
    sleep(1);
    return 0;
}