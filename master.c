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

int pid_producer_named;
int pid_consumer_named;
int pid_producer_unnamed;
int pid_consumer_unnamed;
int pid_producer_sockets;
int pid_consumer_sockets;
int pid_producer_shm;
int pid_consumer_shm;
int input_size;

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

int interpreter()
{
    int c;
    printf("\nHow many numbers do you want to send?\n");
    scanf("%d", &input_size);
    printf("Choose a modality: \n");
    printf(" [1] Named pipe \n [2] unnamed pipe \n [3] sockets \n [4] shared memory \n");
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

/* MAIN */
int main()
{

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
            wait(NULL);
        }
        // if(command==2){}
    }
    unlink("/tmp/fifo_p_to_c");
    return 0;
}