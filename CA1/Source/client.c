#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "structure.h"

int connectServer(int port) {

    printf("Connection to port %d\n", port);

    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        printf("Error connecting to server\n");
        exit(0);
    }

    return fd;
}

int main(int argc, char *argv[]) {

    int fd;
    char buff[BUFFER] = {0};

    fd = connectServer(atoi(argv[1]));

    recv(fd, buff, 1024, 0);
    printf("%s", buff);

//    int answer = 0;
    char response[5];
//    scanf("%d",answer);
    read(0, response, 1024);
//    sprintf(response,"%d",answer);
    send(fd, response, strlen(response), 0);

    return 0;
}
