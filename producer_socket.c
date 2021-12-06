#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <netdb.h>
#include <sys/time.h>
#include <math.h>
#define MAX 250000
void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, fd_time_start;
    struct sockaddr_in serv_addr;
    struct timespec start;
    struct hostent *server;
    char *fifo_time_start = "/tmp/fifo_time_start";
    // The variable server is a pointer to a structure of type hostent. This structure is defined in the header file
    // netdb.h as follows:
    struct hostent
    {
        char *h_name;         /* official name of host */
        char **h_aliases;     /* alias list */
        int h_addrtype;       /* host address type */
        int h_length;         /* length of address */
        char **h_addr_list;   /* list of addresses from name server */
#define h_addr h_addr_list[0] /* address, for backward compatiblity */
    };

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    int noelement_to_send = atoi(argv[3]);
    int data[MAX];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    // argv[1] contains the name of a host on the Internet
    // gethostbyname() takes such a name as an argument and returns a pointer to a hostent containing information about that host.
    // The field char *h_addr contains the IP address.
    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // This code sets the fields in serv_addr. Much of it is the same as in the server. However, because the field
    // server->h_addr is a character string, we use the function:
    // void bcopy(char *s1, char *s2, int length)
    // which copies length bytes from s1 to s2.
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // The connect function is called by the client to establish a connection to the server. It takes three arguments,
    // the socket file descriptor, the address of the host to which it wants to connect (including the port number),
    // and the size of this address. This function returns 0 on success and -1 if it fails.
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // the client needs to know the port number of the server, but it does not need to know its own port
    // number. This is typically assigned by the system when connect is called
    fd_time_start = open(fifo_time_start, O_WRONLY);
    clock_gettime(CLOCK_REALTIME, &start);
    double time_start = start.tv_sec * 1000 + start.tv_nsec * pow(10, -6);
    write(fd_time_start, &time_start, sizeof(time_start));

    for (int i = 0; i < noelement_to_send; i++)
    {
        write(sockfd, &data[i], sizeof(int));

        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX;
            i = 0;
        }
    }
    // close pipe time
    return 0;
}