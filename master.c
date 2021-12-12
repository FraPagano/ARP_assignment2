/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>

#include "parameters.h"

int pid_producer;
int pid_consumer;
int noelement;
float dataMB;
char str[80];
FILE *log_file;
/* FUNCTIONS HEADERS */
int spawn(const char *program, char **arg_list);
void create_fifo(const char *name);
int interpreter();
double closing_function();
void logPrint(char *string);

/* FUNCTIONS */
int spawn(const char *program, char **arg_list)
{
    /* Function to generate a child process, it returns the PID of the child. */

    pid_t child_pid = fork();
    if (child_pid != 0) // Father process.
        return child_pid;

    else
    { // Child process.
        CHECK(execvp(program, arg_list));
        perror("exec failed"); // If it's executed, an error occurred.
        return -4;
    }
}

void create_fifo(const char *name)
{
    /* Function to generate a named pipe. */
    mkfifo(name, 0666);
}

void logPrint(char *string)
{
    /* Function to print on log file adding time stamps. */

    time_t ltime = time(NULL);
    fprintf(log_file, "%.19s: %s", ctime(&ltime), string);
    fflush(log_file);
}

int interpreter()
{

    int c;

    while (1)
    {

        printf(BHWHT "Please, choose the data transmission modality: " RESET "\n");
        printf(BHBLU " [1] Named pipe " RESET "\n" BHYEL " [2] Unnamed pipe " RESET "\n" BHCYN " [3] Sockets " RESET "\n" BHMAG " [4] Shared memory " RESET "\n" BHRED " [5] For exiting" RESET "\n");
        fflush(stdout);
        scanf(" %d", &c);

        if (c == 1)
        {
            printf(BHGRN "NAMED PIPE selected, " RESET);
            logPrint("Master    : '1' keyboard key pressed\n");
            fflush(stdout);
            break;
        }
        else if (c == 2)
        {
            printf(BHGRN "UNNAMED PIPE selected, " RESET);
            logPrint("Master    : '2' keyboard key pressed\n");
            break;
        }
        else if (c == 3)
        {
            printf(BHGRN "SOCKETS selected, " RESET);
            logPrint("Master    : '3' keyboard key pressed\n");
            break;
        }
        else if (c == 4)
        {
            printf(BHGRN "SHARED MEMORY selected, " RESET);
            logPrint("Master    : '4' keyboard key pressed\n");
            break;
        }
        else if (c == 5)
        {
            printf(BHRED "Exiting..." RESET "\n");
            logPrint("Master    : '5' keyboard key pressed\n");
            return c;
        }
        else
        {
            printf("Please, use the commands above.\n");
            logPrint("Master    : wrong command pressed\n");
            fflush(stdout);
            fgetc(stdin);
        }
    }

    while (1)
    {
        printf(BHGRN "Insert how many MB do you want to send: [float < 100] " RESET "\n");
        fflush(stdout);

        scanf("%f", &dataMB);
        sprintf(str, "Master    : %f MB inserted\n", dataMB);
        logPrint(str);

        if (dataMB > 100)
        {
            printf(BHRED "The number must be less then or equal 100.\n" RESET);
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

    CHECK(waitpid(pid_producer, &prod_status, 0));
    CHECK(waitpid(pid_consumer, &cons_status, 0));

    CHECK(read(fd_time_start, &start, sizeof(start)));
    CHECK(read(fd_time_end, &end, sizeof(end)));

    CHECK(close(fd_time_start));
    CHECK(close(fd_time_end));

    printf(BHCYN "\n\nProducer " BHYEL " (PID = %d)" BHCYN " exited with status " BHGRN "%d" RESET "\n", pid_producer, prod_status);
    printf(BHCYN "Consumer " BHYEL "(PID = %d) " BHCYN "exited with status " BHGRN " %d" RESET "\n", pid_consumer, cons_status);

    if (prod_status != 0 || cons_status != 0)
    {

        printf(BHRED "\n An error occurred, killing all processes..." RESET "\n");
        logPrint("An error occurred, killing all processes...\n");

        printf(BHCYN "\n\nProducer " BHRED "(PID = %d) " BHCYN " exited with status " BHRED "%d" RESET "\n", pid_producer, prod_status);
        printf(BHCYN "Consumer " BHRED "(PID = %d)" BHCYN " exited with status " BHRED "%d" RESET "\n", pid_consumer, cons_status);

        sprintf(str, "Master    : Producer (PID = %d) exited with status %d\n", pid_producer, prod_status);
        logPrint(str);

        sprintf(str, "Master    : Consumer (PID = %d) exited with status %d\n", pid_consumer, cons_status);
        logPrint(str);

        kill(pid_producer, SIGKILL);
        kill(pid_consumer, SIGKILL);
        kill(getpid(), SIGKILL);
    }

    sprintf(str, "Master    : Producer (PID = %d) exited with status %d\n", pid_producer, prod_status);
    logPrint(str);

    sprintf(str, "Master    : Consumer (PID = %d) exited with status %d\n", pid_consumer, cons_status);
    logPrint(str);

    return (end - start);
}

/* MAIN */
int main()
{
    FILE *tests = fopen("./tests_performed.txt", "w");
    FILE *log_file_create = fopen("./log.txt", "w");

    if (!log_file_create)
    { // Error management for fopen.
        perror("Error opening file");
        return -2; // return value put at -2 just to avoid confusing with the CHECK function control.
    }
    log_file = fopen("./log.txt", "a");

    logPrint("Master    : Log file created by master process.\n");
    printf(BHWHT "\nMaster process ID: " BHYEL "%d" RESET "\n\n", getpid());
    sprintf(str, "Master process ID: %d\n", getpid());
    logPrint(str);

    create_fifo(TSTART_PATH);
    create_fifo(TEND_PATH);

    while (1)
    {
        int command = interpreter();

        char noelement_char[20];
        sprintf(noelement_char, "%d", noelement);

        if (command == 1) // named pipe
        {
            logPrint("Master    : Named pipe selected\n");
            create_fifo(PIPE_PATH);

            printf(BHBLU "Sending data..." RESET "\n");
            fflush(stdout);

            logPrint("Master    : Creating producer and consumer child processes\n");
            char *arg_list_producer_named[] = {"./producer_named", noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_named", arg_list_producer_named);

            char *arg_list_consumer_named[] = {"./consumer_named", noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_named", arg_list_consumer_named);

            double time = closing_function();

            printf("\n" BHWHT "---> NAMED PIPE took " BHYEL "%f" BHWHT " milliseconds to transfer " BHYEL "%f MB." RESET "\n \n", time, dataMB);
            fprintf(tests, "---> NAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            sprintf(str, "Master    : NAMED PIPE took %f milliseconds to transfer %f MB\n", time, dataMB);
            logPrint(str);

            CHECK(unlink(PIPE_PATH));
        }
        if (command == 2) // unnamed pipe
        {
            int fd_unnamed[2];
            CHECK(pipe(fd_unnamed));

            char input_fd_char[20];
            sprintf(input_fd_char, "%d", fd_unnamed[1]);

            printf(BHBLU "Sending data..." RESET "\n");
            fflush(stdout);

            logPrint("Master    : Creating producer and consumer child processes\n");
            char *arg_list_producer[] = {"./producer_unnamed", noelement_char, input_fd_char, (char *)NULL};
            pid_producer = spawn("./producer_unnamed", arg_list_producer);

            sprintf(input_fd_char, "%d", fd_unnamed[0]);

            char *arg_list_consumer[] = {"./consumer_unnamed", noelement_char, input_fd_char, (char *)NULL};
            pid_consumer = spawn("./consumer_unnamed", arg_list_consumer);

            double time = closing_function();

            printf(BHWHT "\n---> UNNAMED PIPE took" BHYEL " %f " BHWHT " milliseconds to transfer" BHYEL " %f MB." RESET "\n \n", time, dataMB);
            fprintf(tests, "---> UNNAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            sprintf(str, "Master    : Unnamed pipe took %f milliseconds to transfer %f MB\n", time, dataMB);
            logPrint(str);
        }
        if (command == 3) // sockets
        {
            create_fifo(PORT_PATH);
            int portno;
            printf(BHBLU "OS is searching for a free port" RESET "\n");
            printf(BHBLU "Sending data..." RESET "\n");
            fflush(stdout);

            logPrint("Master    : Creating producer and consumer child processes\n");
            char *arg_list_consumer[] = {"./consumer_socket", noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_socket", arg_list_consumer);
            char *arg_list_producer[] = {"./producer_socket", "localhost", noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_socket", arg_list_producer);

            double time = closing_function();

            printf(BHWHT "\n---> SOCKET took " BHYEL "%f" BHWHT " milliseconds to transfer" BHYEL " %f MB." RESET "\n \n", time, dataMB);
            fprintf(tests, "---> SOCKET took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            sprintf(str, "Master    : SOCKET took %f milliseconds to transfer %f MB\n", time, dataMB);
            logPrint(str);

            CHECK(unlink(PORT_PATH));
        }
        if (command == 4)
        {
            printf(BHBLU "Sending data..." RESET "\n");
            fflush(stdout);

            int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
            ftruncate(shm_fd, SIZE);

            sem_t *mutex = sem_open(SNAME_MUTEX, O_CREAT, 0644, 1);
            sem_t *NotFull = sem_open(SNAME_NOTFULL, O_CREAT, 0644, 249);
            sem_t *NotEmpty = sem_open(SNAME_NOTEMPTY, O_CREAT, 0644, 0);

            logPrint("Master    : Creating producer and consumer child processes\n");
            char *arg_list_producer[] = {"./producer_shm", noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_shm", arg_list_producer);

            char *arg_list_consumer[] = {"./consumer_shm", noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_shm", arg_list_consumer);

            double time = closing_function();
            printf(BHWHT "\n---> SHARED MEMORY took " BHYEL "%f" BHWHT " milliseconds to transfer" BHYEL " %f MB" RESET ".\n \n", time, dataMB);
            fprintf(tests, "---> SHARED MEMORY took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            sprintf(str, "Master    : SHARED MEMORY took %f milliseconds to transfer %f MB\n", time, dataMB);
            logPrint(str);

            CHECK(shm_unlink(SHM_NAME));

            CHECK(sem_close(mutex));
            CHECK(sem_close(NotFull));
            CHECK(sem_close(NotEmpty));

            CHECK(sem_unlink(SNAME_MUTEX));
            CHECK(sem_unlink(SNAME_NOTFULL));
            CHECK(sem_unlink(SNAME_NOTEMPTY));

            logPrint("Master    : Shared memory space unlinked\n");
            logPrint("Master    : Semaphores unlinked\n");
        }

        if (command == 5)
        {
            logPrint("Master    : Exiting\n");
            break;
        }
    }
    CHECK(unlink(TSTART_PATH));
    CHECK(unlink(TEND_PATH));
    logPrint("Master    : Master is termitating \n");
    return 0;
}
