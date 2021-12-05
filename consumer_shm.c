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
    int shared_segment_size = sizeof(float) * 1;
    int shm_fd = shm_open(SH_MEM_OBJ, O_RDONLY, 0666);
    /*SHARED MEMORY START*/
    void *sh_in = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    // float in[1000000];
    // memset(in, 0, sizeof(in));
    // int i = 0;

    for (int i = 0; i < 10; i++)
    {
        // memcpy(&in[i], sh_in, sizeof(float));

        printf("%d", *((int *)sh_in));
        fflush(stdout);
        sh_in += sizeof(int);
        // sprintf(sh_in, "%f", in[i]);
        // printf("I read the %d float of in: %s\n", i, (char *)sh_in);
        // i++;
        sleep(1);
    }
    shm_unlink(SH_MEM_OBJ);
    /*SHARED MEMORY END*/

    return 0;
}