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
#include <semaphore.h>
#include <sys/mman.h>

#define SIZE 1000
#define SNAME_MUTEX "/sem_mutex"
#define SNAME_NOTFULL "/sem_not_full"
#define SNAME_NOTEMPTY "/sem_not_empty"
#define SHM_PATH "/shm"

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

int interpreter()
{

    int c;

    while (1)
    {

        printf("Please, choose the data transmission modality: \n");
        printf(" [1] Named pipe \n [2] unnamed pipe \n [3] sockets \n [4] shared memory \n [5] for exiting\n");
        fflush(stdout);
        sleep(1);
        scanf("%d", &c);

        if (c == 1) // 1
        {
            printf("NAMED PIPE selected, ");
            fflush(stdout);
            break;
        }
        else if (c == 2) // 2
        {
            printf("UNNAMED PIPE selected, ");
            break;
        }
        else if (c == 3) // 3
        {
            printf("SOCKETS selected, ");
            break;
        }
        else if (c == 4) // 4
        {
            printf("SHARED MEMORY selected, ");
            break;
        }
        else if (c == 5)
        {
            printf("Exiting...\n");
            return c;
        }
        else
        {
            printf("Please, use the commands above.\n");
            fflush(stdout);
        }
    }

    while (1)
    {
        printf("insert how many MB do you want to send: [float < 100] \n");
        fflush(stdout);

        scanf("%f", &dataMB);

        if (dataMB > 100)
        {
            printf("The number must be less then or equal 100.\n Please, ");
        }
        else
        {
            input_size = dataMB * 250000;
            break;
        }
    }

    return c;
}

double closing_function()
{

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

    return (end - start);
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
        int command = interpreter();

        char input_size_char[20];
        sprintf(input_size_char, "%d", input_size);

        if (command == 1) // named pipe
        {
            create_fifo("/tmp/fifo_p_to_c");

            printf("Sending data...\n");
            fflush(stdout);

            char *arg_list_producer_named[] = {"./producer_named", input_size_char, (char *)NULL};
            pid_producer = spawn("./producer_named", arg_list_producer_named);

            char *arg_list_consumer_named[] = {"./consumer_named", input_size_char, (char *)NULL};
            pid_consumer = spawn("./consumer_named", arg_list_consumer_named);

            double time = closing_function();

            printf("---> NAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            unlink("/tmp/fifo_p_to_c");
        }
        if (command == 2) // unnamed pipe
        {
            int fd_unnamed[2];
            pipe(fd_unnamed);

            char input_fd_char[20];
            sprintf(input_fd_char, "%d", fd_unnamed[1]);

            printf("Sending data...\n");
            fflush(stdout);

            char *arg_list_producer[] = {"./producer_unnamed", input_size_char, input_fd_char, (char *)NULL};
            pid_producer = spawn("./producer_unnamed", arg_list_producer);

            sprintf(input_fd_char, "%d", fd_unnamed[0]);

            char *arg_list_consumer[] = {"./consumer_unnamed", input_size_char, input_fd_char, (char *)NULL};
            pid_consumer = spawn("./consumer_unnamed", arg_list_consumer);

            double time = closing_function();

            printf("---> UNNAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            unlink("/tmp/fifo_p_to_c");
        }
        if (command == 3) // sockets
        {
            printf("Sending data...\n");
            fflush(stdout);
            char *arg_list_consumer[] = {"./consumer_socket", "5500", input_size_char, (char *)NULL};
            pid_consumer = spawn("./consumer_socket", arg_list_consumer);
            char *arg_list_producer[] = {"./producer_socket", "localhost", "5500", input_size_char, (char *)NULL};
            pid_producer = spawn("./producer_socket", arg_list_producer);

            double time = closing_function();

            printf("---> SOCKET took %f milliseconds to transfer %f MB.\n \n", time, dataMB);
        }
        if (command == 4)
        {
            printf("Sending data...\n");
            fflush(stdout);

            int shm_fd = shm_open(SHM_PATH, O_CREAT | O_RDWR, 0666);
            ftruncate(shm_fd, SIZE);

            sem_t * mutex = sem_open(SNAME_MUTEX, O_CREAT, 0644, 1);
            sem_t * NotFull = sem_open(SNAME_NOTFULL, O_CREAT, 0644, 249);
            sem_t * NotEmpty = sem_open(SNAME_NOTEMPTY, O_CREAT, 0644, 0);

            char *arg_list_producer[] = {"./producer_shm", input_size_char, (char *)NULL};
            pid_producer = spawn("./producer_shm", arg_list_producer);

            char *arg_list_consumer[] = {"./consumer_shm", input_size_char, (char *)NULL};
            pid_consumer = spawn("./consumer_shm", arg_list_consumer);

            double time = closing_function();
            printf("---> SHARED MEMORY took %f milliseconds to transfer %f MB.\n \n", time, dataMB);
            shm_unlink(SHM_PATH);

            sem_close(mutex);
            sem_close(NotFull);
            sem_close(NotEmpty);

            sem_unlink(SNAME_MUTEX);
            sem_unlink(SNAME_NOTFULL);
            sem_unlink(SNAME_NOTEMPTY);
        }

        if (command == 5)
        {
            break;
        }
        sleep(1);
    }
    unlink("/tmp/fifo_time_start");
    unlink("/tmp/fifo_time_end");

    return 0;
}