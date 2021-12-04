/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>

int pid_producer_named;
int pid_consumer_named;
int pid_producer_unnamed;
int pid_consumer_unnamed;
int pid_producer_sockets;
int pid_consumer_sockets;
int pid_producer_shm;
int pid_consumer_shm;
int input_size;
char *fifo_time = "/tmp/fifo_time";

/* FUNCTIONS HEADERS */
int spawn(const char *program, char **arg_list);

/* FUNCTIONS */
int spawn(const char *program, char **arg_list)
{
    /* Function to generate a child process, it returns the PID of the child. */

    pid_t child_pid = fork();
    if (child_pid != 0) // Father process.
        return child_pid;

    else
    { // Child process.
        execvp(program, arg_list);
        perror("exec failed"); // If it's executed, an error occurred.
        return -4;
    }
}

void create_fifo(const char *name)
{
    /* Function to generate a named pipe. */
    mkfifo(name, 0666);
}

float compute_time(struct timeval start, struct timeval end)
{

    float delta = 1000 * (end.tv_sec - start.tv_sec) + pow(10, -6) * (end.tv_usec - end.tv_usec);
    return delta;
}

int interpreter()
{
    float data_size;
    int c;
    printf("\nInsert how many MB do you want to send or 'q' to quit\n");
    scanf("%f", &data_size);

    if (data_size > 100)
    {
        printf("Too many MB\n");
    }
    else
    {
        input_size = data_size * 250000;
        printf("Choose a modality: \n");
        printf(" [1] Named pipe \n [2] unnamed pipe \n [3] sockets \n [4] shared memory \n [q] for exiting\n");
        scanf("%d", &c);
        fflush(stdout);

        if (c == 1) // 1
        {
            printf("Transfering data through named pipe...\n");
            fflush(stdout);
            return c;
        }
        else if (c == 2) // 2
        {
            printf("Transfer data through unnamed pipe\n");
        }
        else if (c == 3) // 3
        {
            printf("Transfer data through sockets\n");
        }
        else if (c == 4) // 4
        {
            printf("Transfer data through shared memory\n");
        }
        else
        {
            printf("Please, use the commands above\n");
        }
    }
}

/* MAIN */
int main()
{

    create_fifo("/tmp/fifo_time");
    int fd_time;
    struct timeval start, end;
    while (1)
    {
        int command = interpreter();
        if (command == 1) // named pipe
        {
            create_fifo("/tmp/fifo_p_to_c");
            char input_size_char[20];
            sprintf(input_size_char, "%d", input_size);
            char *arg_list_producer_named[] = {"./producer_named", input_size_char, (char *)NULL};
            pid_producer_named = spawn("./producer_named", arg_list_producer_named);
            char *arg_list_consumer_named[] = {"./consumer_named", input_size_char, (char *)NULL};
            pid_consumer_named = spawn("./consumer_named", arg_list_consumer_named);

            int prod_status, cons_status;
            waitpid(pid_producer_named, &prod_status, 0);
            waitpid(pid_consumer_named, &cons_status, 0);

            printf("aaaaaaaa");
            fflush(stdout);
            fd_time = open(fifo_time, O_RDONLY);

            read(fd_time, &start, sizeof(struct timeval));
            read(fd_time, &end, sizeof(struct timeval));

            float delta = compute_time(start, end);
            printf("Named pipe took: %f milliseconds\n", delta);

            close(fd_time);
            printf("\nProducer exited with status %d\n", prod_status);
            printf("\nConsumer exited with status %d\n", cons_status);

            unlink("/tmp/fifo_p_to_c");
        }
        if (command == 113)
        {
            break;
        }
    }
    unlink("/tmp/fifo_time");

    return 0;
}