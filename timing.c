#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "parameters.h"

void send_start_time () {
    struct timespec start;

    clock_gettime(CLOCK_REALTIME, &start);

    double time_start = start.tv_sec * 1000 + start.tv_nsec * pow(10, -6);

    int fd_time_start = open(TSTART_PATH, O_WRONLY);

    write(fd_time_start, &time_start, sizeof(time_start));

    close(fd_time_start);
}

void send_end_time () {
    struct timespec end;

    clock_gettime(CLOCK_REALTIME, &end);

    double time_end = end.tv_sec * 1000 + end.tv_nsec * pow(10, -6);

    int fd_time_end = open(TEND_PATH, O_WRONLY);

    write(fd_time_end, &time_end, sizeof(time_end));

    close(fd_time_end);
}