#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>

#include "parameters.h"

int main(int argc, char *argv[])
{
    int data, B[MAX];
    int tail = 0;

    int noelement_to_read = atoi(argv[1]);

    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    void *shm_ptr = mmap(NULL, CBUFFER_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    sem_t *mutex = sem_open(SNAME_MUTEX, 0);
    sem_t *NotFull = sem_open(SNAME_NOTFULL, 0);
    sem_t *NotEmpty = sem_open(SNAME_NOTEMPTY, 0);

    for (int i = 0; i < noelement_to_read; i++)
    {
        sem_wait(NotEmpty);
        sem_wait(mutex);

        memcpy(&data, &(((int *)shm_ptr)[tail]), sizeof(int));
        B[i] = data;
        tail = (tail + 1) % BUFFER_NOELEMENT;

        sem_post(mutex);
        sem_post(NotFull);

        if (i == MAX)
        {
            noelement_to_read = noelement_to_read - MAX;
            i = 0;
        }
    }

    send_end_time();
    sleep(1);
    close(shm_fd);
    return 0;
}