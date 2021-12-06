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
#define SNAME_MUTEX "/sem_mutex"
#define SNAME_NOTFULL "/sem_not_full"
#define SNAME_NOTEMPTY "/sem_not_empty"
#define SHM_PATH "/shm"
#define SIZE 1000
#define MAX 250000

int main(int argc, char *argv[])
{
    int size = atoi(argv[1]);
    struct timespec end;
    char *fifo_time_end = "/tmp/fifo_time_end";
    int fd_time_end;

    int shm_fd = shm_open(SHM_PATH, O_RDONLY, 0666);
    void *shm_ptr = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    sem_t *mutex = sem_open(SNAME_MUTEX, 0);
    sem_t *NotFull = sem_open(SNAME_NOTFULL, 0);
    sem_t *NotEmpty = sem_open(SNAME_NOTEMPTY, 0);

    int data;
    int B[MAX];
    int j = 0;

    for (int i = 0; i < size; i++)
    {
        sem_wait(NotEmpty);
        sem_wait(mutex);

        memcpy( &data, &(((int *)shm_ptr)[j]), sizeof(int) );
        B[i] = data;
        j = (j + 1) % 250;

        sem_post(mutex);
        sem_post(NotFull);

        if (i == MAX)
        {
            size = size - MAX;
            i = 0;
            printf("resetted");
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);

    double time_end = end.tv_sec * 1000 + end.tv_nsec * pow(10, -6);

    fd_time_end = open(fifo_time_end, O_WRONLY);

    write(fd_time_end, &time_end, sizeof(time_end));

    return 0;
}