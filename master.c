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

int pid_producer;
int pid_consumer;
int input_size;
float dataMB;
char *fifo_time_start = "/tmp/fifo_time_start";
char *fifo_time_end = "/tmp/fifo_time_end";

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

char interpreter()
{
    
    char c;

    while (1) {

        printf("Please, choose the data transmission modality: \n");
        printf(" [1] Named pipe \n [2] unnamed pipe \n [3] sockets \n [4] shared memory \n [q] for exiting\n");
        fflush(stdout);

        c = getchar();

        if (c == '1') // 1
        {
            printf("NAMED PIPE selected, ");
            fflush(stdout);
            break;

        }
        else if (c == '2') // 2
        {
            printf("UNNAMED PIPE selected, ");
            break;
        }
        else if (c == '3') // 3
        {
            printf("SOCKETS selected, ");
            break;
        }
        else if (c == '4') // 4
        {
            printf("SHARED MEMORY selected, ");
            break;
        }
        else if ( c == 'q' ) {
            printf("EXIT selected.\n");
            return c;

        }
        else
        {
            printf("Please, use the commands above.\n");
            fflush(stdout);
        }
    }

    while (1) {
        printf("insert how many MB do you want to send: [float < 100] \n");
        fflush(stdout);

        scanf("%f", &dataMB);

        if (dataMB > 100)
        {
            printf("The number must be less then or equal 100.\n Please, ");
        } else {
            input_size = dataMB * 250000;
            break;
        }
    }
    return c;
}

double closing_function () {

    int prod_status, cons_status;
    int fd_time_start, fd_time_end;
    double start, end;

    fd_time_start = open(fifo_time_start, O_RDONLY);
    fd_time_end = open(fifo_time_end, O_RDONLY);

    waitpid(pid_producer, &prod_status, 0);
    waitpid(pid_consumer, &cons_status, 0);

    read(fd_time_start, &start, sizeof(start));
    read(fd_time_end, &end, sizeof(end));

    close(fd_time_start);
    close(fd_time_end);

    printf("Producer (PID = %d) exited with status %d\n", pid_producer, prod_status);
    printf("Consumer (PID = %d) exited with status %d\n", pid_consumer, cons_status);

    return (end-start);
}

/* MAIN */
int main()
{
    // system("clear");
    create_fifo(fifo_time_start);
    create_fifo(fifo_time_end);

    // long int start, end;
    while (1)
    {
        char command = interpreter();

        char input_size_char[20];
        sprintf(input_size_char, "%d", input_size);

        if (command == '1') // named pipe
        {
            create_fifo("/tmp/fifo_p_to_c");

            char *arg_list_producer_named[] = {"./producer_named", input_size_char, (char *)NULL};
            pid_producer = spawn("./producer_named", arg_list_producer_named);

            char *arg_list_consumer_named[] = {"./consumer_named", input_size_char, (char *)NULL};
            pid_consumer = spawn("./consumer_named", arg_list_consumer_named);

            double time = closing_function();

            printf("---> NAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB );

            unlink("/tmp/fifo_p_to_c");
        }
        if (command == '2') // unnamed pipe
        {
            int fd_unnamed[2];
            pipe(fd_unnamed);

            char input_fd_char[20];
            sprintf(input_fd_char, "%d", fd_unnamed[1]);

            char *arg_list_producer[] = {"./producer_unnamed", input_size_char, input_fd_char, (char *)NULL};
            pid_producer = spawn("./producer_unnamed", arg_list_producer);

            sprintf(input_fd_char, "%d", fd_unnamed[0]);

            char *arg_list_consumer[] = {"./consumer_unnamed", input_size_char, input_fd_char, (char *)NULL};
            pid_consumer = spawn("./consumer_unnamed", arg_list_consumer);

            double time = closing_function();

            printf("---> UNNAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB );

            unlink("/tmp/fifo_p_to_c");
        }
        if ( command == 'q' ) {
            break;
        }
        sleep(1);
    }
    unlink("/tmp/fifo_time_start");
    unlink("/tmp/fifo_time_end");

    return 0;
}