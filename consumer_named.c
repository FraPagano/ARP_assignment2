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
/* Defining CHECK() tool. By using this methid the code results ligher and cleaner. */
#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stdout,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

int fd, fd_time;
char *fifo = "/tmp/fifo_p_to_c";
char *fifo_time = "/tmp/fifo_time";
struct timeval end;

int main(int argc, char *argv[])
{
    int data;
    const int size = atoi(argv[1]);
    fd = open(fifo, O_RDONLY);

    for (int i = 0; i < size; i++)
    {
        read(fd, &data, sizeof(int));
        printf(" %d ", data);
    }

    gettimeofday(&end, NULL);
    printf("aaaaaaaaa");
    fflush(stdout);
    fd_time = CHECK(open(fifo_time, O_WRONLY));
    printf("bbbbbbbbb");
    fflush(stdout);

    write(fd_time, &end, sizeof(struct timeval));
    sleep(1);
    return 0;
}