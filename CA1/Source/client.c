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
void turn(int sig);
void chat_room(int port, int my_fd, int server_fd);
int receive_port(const char input[]);
int get_port(const char input[]);
int get_users(const char input[]);

int contacts[ROOM_SIZE + 1];

int main(int argc, char *argv[]) {

    int server_port = atoi(argv[1]);
    int fd = connectServer(server_port);

    signal(SIGALRM, continue_loop);
    siginterrupt(SIGALRM, 1);

    while (1) {
        alarm(3);
        char buff[BUFFER] = {0};
        recv(fd, buff, 1024, 0);
        if (receive_port(buff) == 1) {
            alarm(0);
            int chat_port = get_port(buff);
            int me = get_users(buff);
            chat_room(chat_port, me, fd);
            return 0;
        } else printf("%s", buff);

        alarm(3);
        char answer[5] = {0};
        read(0, answer, 1024);
        send(fd, answer, strlen(answer), 0);
    }

    return 0;
}

void continue_loop(int sig) {}


void turn(int sig) {
    printf("Your turn is over\n");
}

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

int get_users(const char input[]) {
    int cu = 0;
    for (int i = 0; i < strlen(input); ++i) {
        if (input[i] == '*') {
            int ufd = 0;
            for (int j = i + 1; input[j] != '*'; i++, j++) {
                ufd += input[j] - 48;
                ufd *= 10;
            }
            ufd /= 10;
            contacts[cu] = ufd;
            cu++;
        }
    }
    return contacts[ROOM_SIZE];
}

void chat_room(int port, int my_fd, int server_fd) {

    int broadcast = 1, opt = 1;
    struct sockaddr_in bc_address;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr("192.168.1.255");

    bind(sock, (struct sockaddr *) &bc_address, sizeof(bc_address));

    signal(SIGALRM, turn);
    siginterrupt(SIGALRM, 1);

    printf("Your chat room is ready\n");

    for (int j = 0; j < ROOM_SIZE; ++j) {
        if (my_fd == contacts[j]) {
            printf("It's your turn . Ask your question\n");
//            alarm(TURN_TIMER);
            char question[BUFFER] = {0};
            read(0, question, BUFFER);
//            alarm(0);
            sendto(sock, question, strlen(question), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
            char temp[BUFFER] = {0};
            recv(sock, temp, 1024, 0);
            for (int k = 0; k < ROOM_SIZE - 1; ++k) {
//                alarm(TURN_TIMER);
                char buffer[BUFFER] = {0};
                recv(sock, buffer, 1024, 0);
                printf("%d : %s", k + 1, buffer);
//                alarm(0);
            }
            printf("Please choose best answer\n");
            char best_answer[10] = {0};
            read(0, best_answer, BUFFER);
            char buffer[BUFFER] = {0};
            sprintf(buffer, "Best answer is %s\n", best_answer);
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
            recv(sock, temp, 1024, 0);
        } else {
            if (contacts[(j + 1) % (ROOM_SIZE - 1)] == my_fd) {
                for (int k = 0; k < ROOM_SIZE + 1; ++k) {
                    if (k == 1) {
                        printf("It's your turn . Answer question\n");
//                        alarm(TURN_TIMER);
                        char answer[BUFFER] = {0};
                        read(0, answer, BUFFER);
//                        alarm(0);
                        sendto(sock, answer, strlen(answer), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
                        char temp[BUFFER] = {0};
                        recv(sock, temp, 1024, 0);
                    } else {
//                        alarm(TURN_TIMER);
                        char buffer[BUFFER] = {0};
                        recv(sock, buffer, 1024, 0);
                        printf("%s", buffer);
//                        alarm(0);
                    }
                }
            } else {
                for (int k = 0; k < ROOM_SIZE + 1; ++k) {
                    if (k == 2) {
                        printf("It's your turn . Answer question\n");
//                        alarm(TURN_TIMER);
                        char answer[BUFFER] = {0};
                        read(0, answer, BUFFER);
//                        alarm(0);
                        sendto(sock, answer, strlen(answer), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
                        char temp[BUFFER] = {0};
                        recv(sock, temp, 1024, 0);
                    } else {
//                        alarm(TURN_TIMER);
                        char buffer[BUFFER] = {0};
                        recv(sock, buffer, 1024, 0);
                        printf("%s", buffer);
//                        alarm(0);
                    }
                }
            }
        }
    }
}
