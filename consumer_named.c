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
#include <math.h>
#define MAX 250000
/* Defining CHECK() tool. By using this methid the code results ligher and cleaner. */
#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stdout,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

int fd, fd_time_end;
char *fifo = "/tmp/fifo_p_to_c";
char *fifo_time_end = "/tmp/fifo_time_end";
struct timespec end;
int B[MAX];

int main(int argc, char *argv[])
{
    int data;
    int size = atoi(argv[1]);
    fd = open(fifo, O_RDONLY);

    for (int i = 0; i < size; i++)
    {
        read(fd, &data, sizeof(int));

        B[i] = data;

        if (i == MAX)
        {
            size = size - MAX;
            i = 0;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);

    double time_end = end.tv_sec * 1000 + end.tv_nsec * pow(10, -6);

    fd_time_end = open(fifo_time_end, O_WRONLY);

    write(fd_time_end, &time_end, sizeof(time_end));
    sleep(1);
    return 0;
}