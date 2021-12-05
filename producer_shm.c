#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    const char *SH_MEM_OBJ = "/shm_AOS";
    const int SIZE = 4096;

    float out[1000000];
    // memset(out, 0, sizeof(out));

    for (int i = 0; i < 1000000; i++)
    {
        out[i] = rand();
    }
    int i = 0;

    int shm_fd = shm_open(SH_MEM_OBJ, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, SIZE);
    // La share memory aperta diventa esattamente di size shared_memory_size

    /*the mmap() funciton establishes a memory-mapped file containing the shared memory object. It also
    returns a pointer to the memory-mapped file that is used for accessing the shared memory object.
    */
    void *sh_out = mmap(NULL, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // printf("Il primo elemento di out e': %f\n", out[0]);

    for (int i = 0; i < 10; i++)
    {

        *(int *)sh_out = i;
        sh_out += sizeof(int);

        sleep(1);
    }

    shm_unlink(SH_MEM_OBJ);
    return 0;
}
