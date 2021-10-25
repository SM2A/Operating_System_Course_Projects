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
void chat_room(int port);
int receive_port(const char input[]);
int get_port(const char input[]);

int main(int argc, char *argv[]) {

    int fd;
    int server_port = atoi(argv[1]);
    fd = connectServer(server_port);

    signal(SIGALRM, continue_loop);
    siginterrupt(SIGALRM, 1);


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        alarm(3);
        char buff[BUFFER] = {0};
        recv(fd, buff, 1024, 0);
        if (receive_port(buff) == 1) {
            alarm(0);
            int chat_port = get_port(buff);
            chat_room(chat_port);
        } else printf("%s", buff);

        alarm(3);
        char answer[5] = {0};
        read(0, answer, 1024);
        send(fd, answer, strlen(answer), 0);
    }
#pragma clang diagnostic pop

    return 0;
}

void continue_loop(int sig) {}

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

int receive_port(const char input[]) {
    int i;
    char indicator[8] = "$YCISOP$";
    for (i = 0; (i < strlen(input)) && (i < 8); ++i)
        if (input[i] != indicator[i]) return 0;
    if (i == 8) return 1;
    else return 0;
}

int get_port(const char input[]) {
    int cp = 0;
    for (int i = 9; input[i] != '#'; ++i) {
        cp += input[i] - 48;
        cp *= 10;
    }
    cp /= 10;
    return cp;
}

void chat_room(int port) {

    for (int i = 0; i < 5; ++i) {
        printf("chatting in port %d\n", port);
    }
}
