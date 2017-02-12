#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>

void error(char *msg)  {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])  {
    if (argc < 3) {
        fprintf(stderr, "Invalid number of args\n");
        exit(0);
    }

    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // init server instance by location (eg. localhost)
    struct hostent *server;
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // define server address
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    bcopy((char *) server -> h_addr, (char *) &serv_addr.sin_addr.s_addr, server -> h_length);

    // establish TCP connection
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    // prompt client for number
    char buffer[256];
    bzero(buffer, 256);
    printf("Enter num to be squared: ");
    fgets(buffer, 255, stdin);

    // write to socket
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR writing to socket");
    }

    bzero(buffer, 256);

    // read response from server
    n = read(sockfd, buffer, 255);
    if (n < 0) {
        error("ERROR reading from socket");
    }

    printf("%s\n", buffer);
    return 0;
}