#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

void error(char *msg) {
    perror(msg);
    exit(1);
}

void handleRequest(int newsockfd) {
    char buffer[256];
    bzero(buffer, sizeof(buffer));

    if (read(newsockfd, buffer, sizeof(buffer) - 1) < 0) { // read string buffer from client
        error("ERROR: failed to read from socket");
    }

    int num = atoi(buffer);
    char output[255];

    if(num == 0) {
        error("ERROR: invalid input received");
    }

    sprintf(output, "%d", num * num);
    if (write(newsockfd, output, sizeof(output)) < 0) { // respond to client connection
        error("ERROR: failed to write to socket");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,"ERROR: no port provided\n");
        exit(1);
    }

    // create TCP socket file descriptor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR: failed to open socket");
    }

    // define server address
    struct sockaddr_in serv_address;
    bzero((char *) &serv_address, sizeof(serv_address));

    serv_address.sin_family = AF_INET;  // use internet domain set of IP addresses
    serv_address.sin_addr.s_addr = INADDR_ANY; // inherit IP address from machine

    int port;
    sscanf(argv[1], "%d", &port);
    serv_address.sin_port = htons(port); // set port from command line arg

    // bind sock to server address
    if (bind(sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0) {
        error("ERROR: failed to bind to socket");
    }

    // listen to active socket, with 5 open connections
    listen(sockfd, 5);

    // define client socket
    struct sockaddr_in cli_address;
    int cli_length = sizeof(cli_address);

    int newsockfd, pid;

    while (1) {
        // suspend process until connection is established
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_address, (socklen_t*) &cli_length);
        if (newsockfd < 0) {
            error("ERROR: failed to accept socket");
        }

        pid = fork();
        if(pid < 0) {
            error("ERROR: failed on fork");
        } else if (pid == 0) {
            close(sockfd);

            handleRequest(newsockfd);

            exit(0);
        } else {
            close(newsockfd);
        }
    }
}