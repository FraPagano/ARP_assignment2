/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include "parameters.h"

FILE *log_file; // log file

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
    int sockfd;    // File descriptor
    int data[MAX]; // Array of data to send
    struct sockaddr_in serv_addr;
    log_file = fopen("./log.txt", "a");

    /*  The variable server is a pointer to a structure of type hostent.
        This structure is defined in the header file netdb.h*/
    struct hostent *server = gethostbyname(argv[1]); // argv[1] contains the name of a host on the Internet
    int noelement_to_send = atoi(argv[2]);           // number of element to send
    int portno;                                      // Port number

    int fd_port = CHECK(open(PORT_PATH, O_RDONLY));
    CHECK(read(fd_port, &portno, sizeof(int))); // reading the port number

    sockfd = CHECK(socket(AF_INET, SOCK_STREAM, 0)); // creating the socket in the Internet

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // This code sets the fields in serv_addr.
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // The connect function is called by the client to establish a connection to the server.
    CHECK(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)));

    for (int i = 0; i < MAX; i++)
    {
        // Filling the array with random integer values
        data[i] = rand() % 10;
    }

    send_start_time(); // Start time instant

    for (int i = 0; i < noelement_to_send; i++)
    {
        CHECK(write(sockfd, &data[i], sizeof(int)));
        /*  We established a maximum array size of 250000 elements. Therefore we send
            the same data of the same array multiple times. When the whole array has
            been sent we make the for loop restart until all the data are sent.    */
        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX; // Reduce the number of element to send
            i = 0;                                       // Restart the for loop
        }
    }
    logPrint("Producer Socket    : Data written\n");
    sleep(1);

    CHECK(close(fd_port));
    CHECK(close(sockfd));
    return 0;
}