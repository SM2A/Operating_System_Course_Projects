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
void send_qa(char qa[3][BUFFER], int server_fd, int best_answer);

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
        } else {
            char output[BUFFER] = {0};
            sprintf(output, "%s", buff);
            write(1, output, strlen(output));
        }

        alarm(3);
        char answer[5] = {0};
        read(0, answer, 1024);
        send(fd, answer, strlen(answer), 0);
    }

    return 0;
}

void continue_loop(int sig) {}


void turn(int sig) {
    write(1, "Your turn is over\n", strlen("Your turn is over\n"));
}

int connectServer(int port) {

    char output[BUFFER] = {0};
    sprintf(output, "Connection to port %d\n", port);
    write(1, output, strlen(output));

    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        write(1, "Error connecting to server\n", strlen("Error connecting to server\n"));
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

    write(1, "Your chat room is ready\n", strlen("Your chat room is ready\n"));

    for (int j = 0; j < ROOM_SIZE; ++j) {
        char qa[3][BUFFER] = {0, 0, 0};
        if (my_fd == contacts[j]) {
            write(1, "It's your turn . Ask your question\n", strlen("It's your turn . Ask your question\n"));
            read(0, qa[0], BUFFER);
            sendto(sock, qa[0], strlen(qa[0]), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
            char temp[BUFFER] = {0};
            recv(sock, temp, 1024, 0);
            for (int k = 0; k < ROOM_SIZE - 1; ++k) {
                recv(sock, qa[k + 1], 1024, 0);
                char output[BUFFER] = {0};
                sprintf(output, "%d : %s", k + 1, qa[k + 1]);
                write(1, output, strlen(output));
            }
            if ((strcmp(qa[0], "@@PASS@@\n") != 0) && (strcmp(qa[1], "@@PASS@@\n"))) {
                write(1, "Please choose best answer\n", strlen("Please choose best answer\n"));
                char best_answer[10] = {0};
                read(0, best_answer, BUFFER);
                char buffer[BUFFER] = {0};
                sprintf(buffer, "Best answer is %s\n", best_answer);
                sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
                recv(sock, temp, 1024, 0);
                send_qa(qa, server_fd, atoi(best_answer));
            } else {
                char buffer[BUFFER] = {0};
                sprintf(buffer, "No Answer\n");
                sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
                recv(sock, temp, 1024, 0);
                send_qa(qa, server_fd, 0);
            }
        } else {
            if (contacts[(j + 1) % (ROOM_SIZE - 1)] == my_fd) {
                for (int k = 0; k < ROOM_SIZE + 1; ++k) {
                    if (k == 1) {
                        write(1, "It's your turn . Answer question\n", strlen("It's your turn . Answer question\n"));
                        char answer[BUFFER] = {0};
                        alarm(TURN_TIMER);
                        int is = read(0, answer, BUFFER);
                        alarm(0);
                        if (is == -1) sprintf(answer, "@@PASS@@\n");
                        sendto(sock, answer, strlen(answer), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
                        char temp[BUFFER] = {0};
                        recv(sock, temp, 1024, 0);
                    } else {
                        char buffer[BUFFER] = {0};
                        recv(sock, buffer, 1024, 0);
                        char output[BUFFER] = {0};
                        sprintf(output, "%s\n", buffer);
                        write(1, output, strlen(output));
                    }
                }
            } else {
                for (int k = 0; k < ROOM_SIZE + 1; ++k) {
                    if (k == 2) {
                        write(1, "It's your turn . Answer question\n", strlen("It's your turn . Answer question\n"));
                        char answer[BUFFER] = {0};
                        alarm(TURN_TIMER);
                        int is = read(0, answer, BUFFER);
                        alarm(0);
                        if (is == -1) sprintf(answer, "@@PASS@@\n");
                        sendto(sock, answer, strlen(answer), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
                        char temp[BUFFER] = {0};
                        recv(sock, temp, 1024, 0);
                    } else {
                        char buffer[BUFFER] = {0};
                        recv(sock, buffer, 1024, 0);
                        char output[BUFFER] = {0};
                        sprintf(output, "%s\n", buffer);
                        write(1, output, strlen(output));
                    }
                }
            }
        }
    }
    write(1, "Good Bye\n", strlen("Good Bye\n"));
}

void send_qa(char qa[3][BUFFER], int server_fd, int best_answer) {

    if ((strcmp(qa[0], "@@PASS@@\n") != 0) && (strcmp(qa[1], "@@PASS@@\n"))) {
        int end = strlen(qa[best_answer]) - 1;
        qa[best_answer][end] = ' ';
        qa[best_answer][end + 1] = '*';
        qa[best_answer][end + 2] = '\n';
    }

    char qa_str[BUFFER] = {0};
    sprintf(qa_str, "%s%s%s\n", qa[0], qa[1], qa[2]);
    send(server_fd, qa_str, strlen(qa_str), 0);
}
