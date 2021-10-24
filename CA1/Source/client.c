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
#include <signal.h>

int connectServer(int port);
void continue_loop(int sig);

int main(int argc, char *argv[]) {

    int fd;
    int port = atoi(argv[1]);
    fd = connectServer(port);

    signal(SIGALRM,continue_loop);
    siginterrupt(SIGALRM, 1);


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        alarm(3);
        char buff[BUFFER] = {0};
        recv(fd, buff, 1024, 0);
        printf("%s", buff);

        alarm(3);
        char answer[5] = {0};
        read(0, answer, 1024);
        send(fd, answer, strlen(answer), 0);
    }
#pragma clang diagnostic pop

    return 0;
}

void continue_loop(int sig){}

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
