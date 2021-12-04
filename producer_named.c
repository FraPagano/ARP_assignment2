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
#include <sys/time.h>
#include <time.h>
#define MAX 2000000

int fd, fd_time;
char *fifo = "/tmp/fifo_p_to_c";
char *fifo_time = "/tmp/fifo_time";
struct timeval start;
int main(int argc, char *argv[])
{
    int size = atoi(argv[1]);
    int data[MAX];

    for (int i = 0; i < MAX; i++)
    {
        data[i] = rand() % 10;
    }

    fd = open(fifo, O_WRONLY);

    gettimeofday(&start, NULL);

    int cycles = size / MAX;
    for (int i = 0; i < cycles; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            write(fd, &data[j], sizeof(int));
        }
    }

    size = size - cycles * MAX;

    for (int i = 0; i < size; i++)
    {
        write(fd, &data[i], sizeof(int));
    }
    printf("bbbbbssss");
    fflush(stdout);
    fd_time = open(fifo_time, O_WRONLY);
    printf("asssss");
    fflush(stdout);
    write(fd_time, &start, sizeof(struct timeval));

    sleep(1);

    return 0;
}