#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>
#include "parameters.h"

FILE *log_file;
void logPrint(char *string);
void logPrint(char *string)
{
    /* Function to print on log file adding time stamps. */

    time_t ltime = time(NULL);
    fprintf(log_file, "%.19s: %s", ctime(&ltime), string);
    fflush(log_file);
}

int main(int argc, char *argv[])
{
    int data, B[MAX];
    int tail = 0;
    log_file = fopen("./log.txt", "a");
    int noelement_to_read = atoi(argv[1]);

    int shm_fd = CHECK(shm_open(SHM_NAME, O_RDONLY, 0666));
    void *shm_ptr = mmap(NULL, CBUFFER_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

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

    for (int i = 0; i < noelement_to_read; i++)
    {
        CHECK(sem_wait(NotEmpty));
        CHECK(sem_wait(mutex));

        memcpy(&data, &(((int *)shm_ptr)[tail]), sizeof(int));
        B[i] = data;
        tail = (tail + 1) % BUFFER_NOELEMENT;

        CHECK(sem_post(mutex));
        CHECK(sem_post(NotFull));

        if (i == MAX)
        {
            noelement_to_read = noelement_to_read - MAX;
            i = 0;
        }
    }
    logPrint("Consumer Shared Memory    :Data read\n");
    send_end_time();
    sleep(1);
    CHECK(close(shm_fd));
    return 0;
}