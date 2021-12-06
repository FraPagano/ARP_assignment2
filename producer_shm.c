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
    int fd_time_start;
    struct timespec start;
    char *fifo_time_start = "/tmp/fifo_time_start";
    sem_t *mutex = sem_open(SEM_PATH_MUTEX, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t *NotFull = sem_open(SEM_PATH_NOT_FULL, O_CREAT, S_IRUSR | S_IWUSR, 249);
    sem_t *NotEmpty = sem_open(SEM_PATH_NOT_EMPTY, O_CREAT, S_IRUSR | S_IWUSR, 0);

    int data[size];
    int j = 0;
    /*the mmap() funciton establishes a memory-mapped file containing the shared memory object. It also
    returns a pointer to the memory-mapped file that is used for accessing the shared memory object.
    */
    void *shm_ptr = mmap(NULL, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for (int i = 0; i < MAX; i++)
    {
        data[i] = rand() % 10;
    }
    fd_time_start = open(fifo_time_start, O_WRONLY);
    clock_gettime(CLOCK_REALTIME, &start);
    double time_start = start.tv_sec * 1000 + start.tv_nsec * pow(10, -6);
    write(fd_time_start, &time_start, sizeof(time_start));

    for (int i = 0; i < size; i++)
    {
        sem_wait(NotFull);
        printf("aaa");
        fflush(stdout);
        // sem_wait(mutex);
        printf("bbb");
        fflush(stdout);

        //((int *)shm_ptr)[j] = data[i];
        j = (j + 1) % SIZE;

        // sem_post(mutex);
        printf("ccc");
        fflush(stdout);
        sem_post(NotEmpty);
        printf("ddd");
        fflush(stdout);

        if (i == MAX)
        {
            size = size - MAX;
            i = 0;
        }
    }

    return 0;
}
