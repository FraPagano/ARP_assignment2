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

/*HEADER FILE. This file contains all the file paths, function headers, the CHECK function declaration and colors arrays */
#include "parameters.h"

/*GLOBAL VARIABLES*/
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
    /* Function to generate a named pipe with 0666 permission. */
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
        /*This is a kind of  User Interface in which the user can select the desired data trasmission modality*/
        printf(BHWHT "Please, choose the data transmission modality: " RESET "\n");
        printf(BHBLU " [1] Named pipe " RESET "\n" BHYEL " [2] Unnamed pipe " RESET "\n" BHCYN " [3] Sockets " RESET "\n" BHMAG " [4] Shared memory " RESET "\n" BHRED " [5] For exiting" RESET "\n");
        fflush(stdout);
        scanf(" %d", &c);

        if (c == 1) // NAMED PIPE
        {
            printf(BHGRN "NAMED PIPE selected, " RESET);
            logPrint("Master    : '1' keyboard key pressed\n");
            fflush(stdout);
            break;
        }
        else if (c == 2) // UNNAMED PIPE
        {
            printf(BHGRN "UNNAMED PIPE selected, " RESET);
            logPrint("Master    : '2' keyboard key pressed\n");
            break;
        }
        else if (c == 3) // SOCKETS
        {
            printf(BHGRN "SOCKETS selected, " RESET);
            logPrint("Master    : '3' keyboard key pressed\n");
            break;
        }
        else if (c == 4) // SHARED MEMORY
        {
            printf(BHGRN "SHARED MEMORY selected, " RESET);
            logPrint("Master    : '4' keyboard key pressed\n");
            break;
        }
        else if (c == 5) // EXIT COMMAND
        {
            printf(BHRED "Exiting..." RESET "\n");
            logPrint("Master    : '5' keyboard key pressed\n");
            return c;
        }
        else // WRONG COMMAND CASE
        {
            printf("Please, use the commands above.\n");
            logPrint("Master    : wrong command pressed\n");
            fflush(stdout);
            fgetc(stdin);
        }
    }

    while (1)
    {
        /*User can also choose the quantity of data to send*/
        printf(BHGRN "Insert how many MB do you want to send: [float < 100] " RESET "\n");
        fflush(stdout);

        scanf("%f", &dataMB);
        sprintf(str, "Master    : %f MB inserted\n", dataMB);
        logPrint(str);

        if (dataMB > 100) // 100 MB is the maximum selectable quantity
        {
            printf(BHRED "The number must be less then or equal 100.\n" RESET);
        }
        else
        {
            // conversion from MB quantity to number of 'int' data type to send
            noelement = dataMB * 250000;
            break;
        }
    }

    return c;
}

double closing_function()
/*This function is useful for waiting the producer and consumer child processes, check their return values
but also read time intervals from a named pipe*/
{

    int prod_status, cons_status;
    int fd_time_start, fd_time_end;
    double start, end;

    fd_time_start = open(TSTART_PATH, O_RDONLY); // named pipe for the time instant in which the transmision starts
    fd_time_end = open(TEND_PATH, O_RDONLY);     // named pipe for the time instant in which the transmision ends

    CHECK(waitpid(pid_producer, &prod_status, 0)); // wait the producer child
    CHECK(waitpid(pid_consumer, &cons_status, 0)); // wait the consumer child

    CHECK(read(fd_time_start, &start, sizeof(start))); // read the start time instant
    CHECK(read(fd_time_end, &end, sizeof(end)));       // read the end time instant

    CHECK(close(fd_time_start));
    CHECK(close(fd_time_end));

    // Make PIDs available to the shell
    printf(BHCYN "\n\nProducer " BHYEL " (PID = %d)" BHCYN " exited with status " BHGRN "%d" RESET "\n", pid_producer, prod_status);
    printf(BHCYN "Consumer " BHYEL "(PID = %d) " BHCYN "exited with status " BHGRN " %d" RESET "\n", pid_consumer, cons_status);

    // Make PIDs available to the log file
    sprintf(str, "Master    : Producer (PID = %d) exited with status %d\n", pid_producer, prod_status);
    logPrint(str);
    sprintf(str, "Master    : Consumer (PID = %d) exited with status %d\n", pid_consumer, cons_status);
    logPrint(str);

    if (prod_status != 0 || cons_status != 0) /*Checking the return values*/
    {

        printf(BHRED "\n An error occurred, killing all processes..." RESET "\n");
        logPrint("An error occurred, killing all processes...\n");

        // Make PIDs available to the shell
        printf(BHCYN "\n\nProducer " BHRED "(PID = %d) " BHCYN " exited with status " BHRED "%d" RESET "\n", pid_producer, prod_status);
        printf(BHCYN "Consumer " BHRED "(PID = %d)" BHCYN " exited with status " BHRED "%d" RESET "\n", pid_consumer, cons_status);

        // Make PIDs available to the log file
        sprintf(str, "Master    : Producer (PID = %d) exited with status %d\n", pid_producer, prod_status);
        logPrint(str);
        sprintf(str, "Master    : Consumer (PID = %d) exited with status %d\n", pid_consumer, cons_status);
        logPrint(str);

        /*If either the producer or the consumer ends with a non-zero status, kill all the processes*/
        kill(pid_producer, SIGKILL);
        kill(pid_consumer, SIGKILL);
        kill(getpid(), SIGKILL);
    }

    /*Return the time interval taken by the data transmission*/
    return (end - start);
}

/* MAIN */
int main()
{
    FILE *tests = fopen("./tests_performed.txt", "w"); // This '.txt' file contains a short recap of the taken time intervals
    FILE *log_file_create = fopen("./log.txt", "w");   // Log file create

    if (!log_file_create)
    {
        // Error management for fopen.
        perror("Error opening file");
        return -2; // return value put at -2 just to avoid confusing with the CHECK function control.
    }

    log_file = fopen("./log.txt", "a"); // Log file append
    logPrint("Master    : Log file created by master process.\n");

    // Make the Master process ID avaible to the shell
    printf(BHWHT "\nMaster process ID: " BHYEL "%d" RESET "\n\n", getpid());

    // Make the Master process ID avaible to the log file
    sprintf(str, "Master process ID: %d\n", getpid());
    logPrint(str);

    create_fifo(TSTART_PATH); // fifo for start time instant
    create_fifo(TEND_PATH);   // fifo for end time instant

    while (1)
    {
        int command = interpreter();
        /*The number of element to transfer will be passed through the argv[] array to each process*/
        char noelement_char[20];
        sprintf(noelement_char, "%d", noelement);

        if (command == 1) // NAMED PIPE SELECTED
        {
            logPrint("Master    : Named pipe selected\n");
            create_fifo(PIPE_PATH);

            printf(BHBLU "Sending data..." RESET "\n");
            fflush(stdout);

            logPrint("Master    : Creating producer and consumer child processes\n");

            /*Executing both the consumer and the producer child processes*/
            char *arg_list_producer_named[] = {"./producer_named", noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_named", arg_list_producer_named);
            char *arg_list_consumer_named[] = {"./consumer_named", noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_named", arg_list_consumer_named);

            double time = closing_function(); // Store the taken time interval in the 'time' variable

            printf("\n" BHWHT "---> NAMED PIPE took " BHYEL "%f" BHWHT " milliseconds to transfer " BHYEL "%f MB." RESET "\n \n", time, dataMB);

            fprintf(tests, "---> NAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            sprintf(str, "Master    : NAMED PIPE took %f milliseconds to transfer %f MB\n", time, dataMB);
            logPrint(str);

            CHECK(unlink(PIPE_PATH));
        }
        if (command == 2) // UNNMED PIPE SELECTED
        {
            int fd_unnamed[2];
            CHECK(pipe(fd_unnamed));

            char input_fd_char[20];
            sprintf(input_fd_char, "%d", fd_unnamed[1]);

            printf(BHBLU "Sending data..." RESET "\n");
            fflush(stdout);

            logPrint("Master    : Creating producer and consumer child processes\n");

            /*Executing both the consumer and the producer child processes*/
            char *arg_list_producer[] = {"./producer_unnamed", noelement_char, input_fd_char, (char *)NULL};
            pid_producer = spawn("./producer_unnamed", arg_list_producer);
            sprintf(input_fd_char, "%d", fd_unnamed[0]);
            char *arg_list_consumer[] = {"./consumer_unnamed", noelement_char, input_fd_char, (char *)NULL};
            pid_consumer = spawn("./consumer_unnamed", arg_list_consumer);

            double time = closing_function(); // Store the taken time interval in the 'time' variable

            printf(BHWHT "\n---> UNNAMED PIPE took" BHYEL " %f " BHWHT " milliseconds to transfer" BHYEL " %f MB." RESET "\n \n", time, dataMB);

            fprintf(tests, "---> UNNAMED PIPE took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            sprintf(str, "Master    : Unnamed pipe took %f milliseconds to transfer %f MB\n", time, dataMB);
            logPrint(str);
        }
        if (command == 3) // SOCKET SELECTED
        {
            create_fifo(PORT_PATH);
            int portno; // The used port number will be availble to the shell
            printf(BHBLU "OS is searching for a free port" RESET "\n");
            printf(BHBLU "Sending data..." RESET "\n");
            fflush(stdout);

            logPrint("Master    : Creating producer and consumer child processes\n");

            /*Executing both the consumer and the producer child processes*/
            char *arg_list_consumer[] = {"./consumer_socket", noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_socket", arg_list_consumer);
            char *arg_list_producer[] = {"./producer_socket", "localhost", noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_socket", arg_list_producer);

            double time = closing_function(); // Store the taken time interval in the 'time' variable

            printf(BHWHT "\n---> SOCKET took " BHYEL "%f" BHWHT " milliseconds to transfer" BHYEL " %f MB." RESET "\n \n", time, dataMB);

            fprintf(tests, "---> SOCKET took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            sprintf(str, "Master    : SOCKET took %f milliseconds to transfer %f MB\n", time, dataMB);
            logPrint(str);

            CHECK(unlink(PORT_PATH));
        }
        if (command == 4) // SHARED MEMORY SELECTED
        {
            printf(BHBLU "Sending data..." RESET "\n");
            fflush(stdout);

            /*OPENING SHARED MEMORY SPACE*/
            int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
            ftruncate(shm_fd, SIZE);

            /*OPENING SEMAPHORES*/
            sem_t *mutex = sem_open(SNAME_MUTEX, O_CREAT, 0644, 1);
            sem_t *NotFull = sem_open(SNAME_NOTFULL, O_CREAT, 0644, 249);
            sem_t *NotEmpty = sem_open(SNAME_NOTEMPTY, O_CREAT, 0644, 0);

            logPrint("Master    : Creating producer and consumer child processes\n");

            /*Executing both the consumer and the producer child processes*/
            char *arg_list_producer[] = {"./producer_shm", noelement_char, (char *)NULL};
            pid_producer = spawn("./producer_shm", arg_list_producer);
            char *arg_list_consumer[] = {"./consumer_shm", noelement_char, (char *)NULL};
            pid_consumer = spawn("./consumer_shm", arg_list_consumer);

            double time = closing_function(); // Store the taken time interval in the 'time' variable

            printf(BHWHT "\n---> SHARED MEMORY took " BHYEL "%f" BHWHT " milliseconds to transfer" BHYEL " %f MB" RESET ".\n \n", time, dataMB);

            fprintf(tests, "---> SHARED MEMORY took %f milliseconds to transfer %f MB.\n \n", time, dataMB);

            sprintf(str, "Master    : SHARED MEMORY took %f milliseconds to transfer %f MB\n", time, dataMB);
            logPrint(str);

            /*UNLINK SHARED MEMORY SPACE*/
            CHECK(shm_unlink(SHM_NAME));

            /*CLOSING SEMAPHORES*/
            CHECK(sem_close(mutex));
            CHECK(sem_close(NotFull));
            CHECK(sem_close(NotEmpty));

            /*UNLINK SEMAPHORES*/
            CHECK(sem_unlink(SNAME_MUTEX));
            CHECK(sem_unlink(SNAME_NOTFULL));
            CHECK(sem_unlink(SNAME_NOTEMPTY));

            logPrint("Master    : Shared memory space unlinked\n");
            logPrint("Master    : Semaphores unlinked\n");
        }

        if (command == 5) // EXIT COMMAND SELECTED
        {
            logPrint("Master    : Exiting\n");
            break;
        }
    }

    /*UNLINK NAMED PIPES USED FOR COMMUNICATION ABOUT TIME*/
    CHECK(unlink(TSTART_PATH));
    CHECK(unlink(TEND_PATH));

    logPrint("Master    : Master is termitating \n");

    return 0;
}
