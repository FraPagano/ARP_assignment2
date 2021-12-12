#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "parameters.h"

void send_start_time()
{
    struct timespec start;

    CHECK(clock_gettime(CLOCK_REALTIME, &start));

    double time_start = start.tv_sec * 1000 + start.tv_nsec * pow(10, -6);

    int fd_time_start = CHECK(open(TSTART_PATH, O_WRONLY));

    CHECK(write(fd_time_start, &time_start, sizeof(time_start)));

    CHECK(close(fd_time_start));
}

void send_end_time()
{
    struct timespec end;

    CHECK(clock_gettime(CLOCK_REALTIME, &end));

    double time_end = end.tv_sec * 1000 + end.tv_nsec * pow(10, -6);

    int fd_time_end = CHECK(open(TEND_PATH, O_WRONLY));

    CHECK(write(fd_time_end, &time_end, sizeof(time_end)));

    CHECK(close(fd_time_end));
}

void loading_bar(int percent, int buf_size)
{
    const int PROG = 30;
    int num_chars = (percent/ (buf_size / 100)) * PROG / 100;
    printf("\r[");
    for (int i = 0; i <= num_chars; i++)
    {
        printf("#");
    }
    for (int i = 0; i < PROG - num_chars - 1; i++)
    {
        printf(" ");
    }
    printf("] %d %% DONE", percent / ((buf_size / 100) + 1));
    fflush(stdout);
}
