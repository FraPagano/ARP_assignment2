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
    int data[MAX];
    int head = 0;

    int noelement_to_send = atoi(argv[1]);

    sem_t *mutex = sem_open(SNAME_MUTEX, 0);
    sem_t *NotFull = sem_open(SNAME_NOTFULL, 0);
    sem_t *NotEmpty = sem_open(SNAME_NOTEMPTY, 0);

    if (mutex == SEM_FAILED)
    {
        perror("Failed to open mutex semaphore\n");
        exit(-1);
    }
    if (NotFull == SEM_FAILED)
    {
        perror("Failed to open NotFull semaphore\n");
        exit(-1);
    }
    if (NotEmpty == SEM_FAILED)
    {
        perror("Failed to open NotEmpty semaphore\n");
        exit(-1);
    }

    /*the mmap() funciton establishes a memory-mapped file containing the shared memory object. It also
    returns a pointer to the memory-mapped file that is used for accessing the shared memory object.
    */

    int shm_fd = CHECK(shm_open(SHM_NAME, O_RDWR, 0666));
    void *shm_ptr = mmap(NULL, CBUFFER_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for (int i = 0; i < MAX; i++)
    {
        data[i] = rand() % 10;
    }

    send_start_time();

    for (int i = 0; i < noelement_to_send; i++)
    {
        CHECK(sem_wait(NotFull));
        CHECK(sem_wait(mutex));

        memcpy(&(((int *)shm_ptr)[head]), &(data[i]), sizeof(int));
        head = (head + 1) % BUFFER_NOELEMENT;

        CHECK(sem_post(mutex));
        CHECK(sem_post(NotEmpty));

        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX;
            i = 0;
        }
    }
    sleep(1);
    CHECK(close(shm_fd));
    return 0;
}
