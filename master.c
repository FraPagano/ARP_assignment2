/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>

#include "paramethers.h"

int pid_producer;
int pid_consumer;
int noelement;
float dataMB;

/* FUNCTIONS HEADERS */
int spawn( const char *program, char **arg_list );
void create_fifo( const char *name );
int interpreter();
double closing_function();

/* FUNCTIONS */
int spawn( const char *program, char **arg_list )
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

void create_fifo( const char *name )
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
        printf(" [1] Named pipe \n [2] Unnamed pipe \n [3] Sockets \n [4] Shared memory \n [5] For exiting\n");
        fflush(stdout);
        scanf(" %d", &c);

        if (c == 1)
        {
            printf("NAMED PIPE selected, ");
            fflush(stdout);
            break;
        }
        else if (c == 2)
        {
            printf("UNNAMED PIPE selected, ");
            break;
        }
        else if (c == 3)
        {
            printf("SOCKETS selected, ");
            break;
        }
        else if (c == 4)
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
            fgetc(stdin);
        }
    }

    while (1)
    {
        printf("Insert how many MB do you want to send: [float < 100] \n");
        fflush(stdout);

        scanf("%f", &dataMB);

        if (dataMB > 100)
        {
            printf("The number must be less then or equal 100.\n Please, ");
        }
        else
        {
            noelement = dataMB * 250000;
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

    fd_time_start = open(TSTART_PATH, O_RDONLY);
    fd_time_end = open(TEND_PATH, O_RDONLY);

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
    FILE * tests = fopen( "./tests_performed.txt", "w");
    create_fifo(TSTART_PATH);
    create_fifo(TEND_PATH);

    // long int start, end;
    while (1)
    {
        int command = interpreter();

        char noelement_char[20];
        sprintf(noelement_char, "%d", noelement);

        if (command == 1) // named pipe
        {
            create_fifo(PIPE_PATH);

            printf("Sending data...\n");
            fflush(stdout);

            char *arg_list_producer_named[] = {"./producer_named", noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_named", arg_list_producer_named);

            char *arg_list_consumer_named[] = {"./consumer_named", noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_named", arg_list_consumer_named);

            double time = closing_function();

            printf("\n---> NAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);
            fprintf(tests, "---> NAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            unlink(PIPE_PATH);
        }
        if (command == 2) // unnamed pipe
        {
            int fd_unnamed[2];
            pipe(fd_unnamed);

            char input_fd_char[20];
            sprintf(input_fd_char, "%d", fd_unnamed[1]);

            printf("Sending data...\n");
            fflush(stdout);

            char *arg_list_producer[] = {"./producer_unnamed", noelement_char, input_fd_char, (char *)NULL};
            pid_producer = spawn("./producer_unnamed", arg_list_producer);

            sprintf(input_fd_char, "%d", fd_unnamed[0]);

            char *arg_list_consumer[] = {"./consumer_unnamed", noelement_char, input_fd_char, (char *)NULL};
            pid_consumer = spawn("./consumer_unnamed", arg_list_consumer);

            double time = closing_function();

            printf("\n---> UNNAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);
            fprintf(tests, "---> UNNAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);
        }
        if (command == 3) // sockets
        {

            int portno;
            printf("Insert the port number:\n");
            scanf("%d", &portno);
            char portno_char[30];
            sprintf(portno_char, "%d", portno);
            printf("Sending data...\n");
            fflush(stdout);

            char *arg_list_consumer[] = {"./consumer_socket", portno_char, noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_socket", arg_list_consumer);
            char *arg_list_producer[] = {"./producer_socket", "localhost", portno_char, noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_socket", arg_list_producer);

            double time = closing_function();

            printf("\n---> SOCKET took %f milliseconds to transfer %f MB.\n \n", time, dataMB);
            fprintf(tests, "---> SOCKET took %f milliseconds to transfer %f MB.\n \n", time, dataMB);
        }
        if (command == 4)
        {
            printf("Sending data...\n");
            fflush(stdout);

            int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
            ftruncate(shm_fd, SIZE);

            sem_t *mutex = sem_open(SNAME_MUTEX, O_CREAT, 0644, 1);
            sem_t *NotFull = sem_open(SNAME_NOTFULL, O_CREAT, 0644, 249);
            sem_t *NotEmpty = sem_open(SNAME_NOTEMPTY, O_CREAT, 0644, 0);

            char *arg_list_producer[] = {"./producer_shm", noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_shm", arg_list_producer);

            char *arg_list_consumer[] = {"./consumer_shm", noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_shm", arg_list_consumer);

            double time = closing_function();
            printf("\n---> SHARED MEMORY took %f milliseconds to transfer %f MB.\n \n", time, dataMB);
            fprintf(tests, "---> SHARED MEMORY took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            shm_unlink(SHM_NAME);

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
    }
    unlink(TSTART_PATH);
    unlink(TEND_PATH);

    return 0;
}