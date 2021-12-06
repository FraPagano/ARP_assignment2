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
#define CBUFFER_SIZE 1000
#define MAX 250000
#define BUFFER_NOELEMENT 250

int main(int argc, char *argv[])
{
    int noelement_to_send = atoi(argv[1]);
    int fd_time_start;
    struct timespec start;
    char *fifo_time_start = "/tmp/fifo_time_start";

    sem_t *mutex = sem_open(SNAME_MUTEX, 0);
    sem_t *NotFull = sem_open(SNAME_NOTFULL, 0);
    sem_t *NotEmpty = sem_open(SNAME_NOTEMPTY, 0);

    int data[MAX];
    int head = 0;
    /*the mmap() funciton establishes a memory-mapped file containing the shared memory object. It also
    returns a pointer to the memory-mapped file that is used for accessing the shared memory object.
    */

    int shm_fd = shm_open(SHM_PATH, O_RDWR, 0666);
    void *shm_ptr = mmap(NULL, CBUFFER_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for (int i = 0; i < MAX; i++)
    {
        data[i] = rand() % 10;
    }
    fd_time_start = open(fifo_time_start, O_WRONLY);
    clock_gettime(CLOCK_REALTIME, &start);
    double time_start = start.tv_sec * 1000 + start.tv_nsec * pow(10, -6);
    write(fd_time_start, &time_start, sizeof(time_start));

    // sem_post(mutex);

    for (int i = 0; i < noelement_to_send; i++)
    {
        sem_wait(NotFull);
        sem_wait(mutex);

        memcpy(&(((int *)shm_ptr)[head]), &(data[i]), sizeof(int));
        head = (head + 1) % BUFFER_NOELEMENT;

        sem_post(mutex);
        sem_post(NotEmpty);

        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX;
            i = 0;
        }
    }

    return 0;
}
