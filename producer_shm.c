/* LIBRARIES */
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

/*FUNCTION HEADER*/
void logPrint(char *string);

/*FUNCTION*/
void logPrint(char *string)
{
    /* Function to print on log file adding time stamps. */
    time_t ltime = time(NULL);
    fprintf(log_file, "%.19s: %s", ctime(&ltime), string);
    fflush(log_file);
}

/*MAIN*/
int main(int argc, char *argv[])
{
    int data[MAX]; // Array of data to read
    int head = 0;  // head of circular buffer

    log_file = fopen("./log.txt", "a");

    int noelement_to_send = atoi(argv[1]);

    /*opening semaphores*/
    sem_t *mutex = sem_open(SNAME_MUTEX, 0);
    sem_t *NotFull = sem_open(SNAME_NOTFULL, 0);
    sem_t *NotEmpty = sem_open(SNAME_NOTEMPTY, 0);

    /*checking the sem_open funciton execution*/
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

    /*OPENING SHARED MEMORY SPACE*/
    int shm_fd = CHECK(shm_open(SHM_NAME, O_RDWR, 0666));

    /*  the mmap() funciton establishes a memory-mapped file containing
        the shared memory object. It also returns a pointer to the memory-mapped
        file that is used for accessing the shared memory object. */
    void *shm_ptr = mmap(NULL, CBUFFER_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for (int i = 0; i < MAX; i++)
    {
        data[i] = rand() % 10; // Filling the array with random integer values
    }

    send_start_time(); // Start time instant

    for (int i = 0; i < noelement_to_send; i++)
    {
        // Decrement the semaphore value.
        CHECK(sem_wait(NotFull)); // Checking if the circular buffer is full
        CHECK(sem_wait(mutex));   // read-write synchronization

        memcpy(&(((int *)shm_ptr)[head]), &(data[i]), sizeof(int));
        head = (head + 1) % BUFFER_NOELEMENT;

        // Increment the semaphore value.
        CHECK(sem_post(mutex));
        CHECK(sem_post(NotEmpty));
        /*  We established a maximum array size of 250000 elements. Therefore we send
            the same data of the same array multiple times. When the whole array has
            been sent we make the for loop restart until all the data are sent.    */
        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX;
            i = 0;
        }
    }

    logPrint("Producer Shared Memory    :Data written\n");

    sleep(1);

    CHECK(close(shm_fd));

    return 0;
}
