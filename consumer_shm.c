#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>
#include <math.h>
#define SEM_PATH_MUTEX "/sem_mutex"
#define SEM_PATH_NOT_FULL "/sem_not_full"
#define SEM_PATH_NOT_EMPTY "/sem_not_empty"
#define SIZE 1000
#define MAX 250000

int main(int argc, char *argv[])
{
    int size = atoi(argv[1]);
    int shm_fd = atoi(argv[2]);
    struct timespec end;
    char *fifo_time_end = "/tmp/fifo_time_end";
    int fd_time_end;
    void *shm_ptr = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    printf("dgdgd");
    fflush(stdout);
    sem_t *mutex = sem_open(SEM_PATH_MUTEX, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t *NotFull = sem_open(SEM_PATH_NOT_FULL, O_CREAT, S_IRUSR | S_IWUSR, 249);
    sem_t *NotEmpty = sem_open(SEM_PATH_NOT_EMPTY, O_CREAT, S_IRUSR | S_IWUSR, 0);
    printf("qqqq");
    fflush(stdout);
    int data;
    int B[size];
    int j = 0;

    for (int i = 0; i < size; i++)
    {
        printf("eee");
        fflush(stdout);
        sem_wait(NotEmpty);
        printf("fff");
        fflush(stdout);
        sem_wait(mutex);
        printf("ggg");
        fflush(stdout);

        data = ((int *)shm_ptr)[j];
        B[i] = data;
        j = (j + 1) % SIZE;

        sem_post(mutex);
        printf("hhhh");
        fflush(stdout);
        sem_post(NotFull);
        printf("iii");
        fflush(stdout);

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

    return 0;
}