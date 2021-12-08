#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#include "parameters.h"

int main(int argc, char *argv[])
{
    int sockfd, data[MAX];
    struct sockaddr_in serv_addr;

    // The variable server is a pointer to a structure of type hostent. This structure is defined in the header file netdb.h

    struct hostent *server = gethostbyname(argv[1]);
    int noelement_to_send = atoi(argv[2]);
    int portno;
    int fd_port = open(PORT_PATH, O_RDONLY);
    read(fd_port, &portno, sizeof(int));

    sockfd = CHECK(socket(AF_INET, SOCK_STREAM, 0));

    // argv[1] contains the name of a host on the Internet
    // gethostbyname() takes such a name as an argument and returns a pointer to a hostent containing information about that host.
    // The field char *h_addr contains the IP address.

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
    CHECK(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)));

    // the client needs to know the port number of the server, but it does not need to know its own port
    // number. This is typically assigned by the system when connect is called

    send_start_time();

    for (int i = 0; i < noelement_to_send; i++)
    {
        write(sockfd, &data[i], sizeof(int));

        if (i == MAX)
        {
            noelement_to_send = noelement_to_send - MAX;
            i = 0;
        }
    }
    sleep(1);

    close(fd_port);
    close(sockfd);
    return 0;
}