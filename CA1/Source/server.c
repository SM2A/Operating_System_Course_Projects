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

int setupServer(int port);
int acceptClient(int server_fd);
int choose_group(char buffer[]);
int add_to_group(int client_fd, int group);
void start_group(int group);
int find_user(int fd);

int port;
int last_user;
category rooms[CATEGORY_COUNT];
user users[50];

int main(int argc, char *argv[]) {

    fd_set master_set, working_set;
    int server_fd, new_socket, max_sd;

    last_user = 0;
    for (int i = 0; i < CATEGORY_COUNT; ++i) rooms[i].index = 0;
    for (int i = 0; i < CATEGORY_COUNT; ++i) {
        for (int j = 0; j < CATEGORY_SIZE; ++j) {
            rooms[i].rooms[j].to_fill = ROOM_SIZE;
        }
    }
    port = atoi(argv[1]);
    server_fd = setupServer(port);

    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(server_fd, &master_set);

    write(1, "Server is running\n", 18);

    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                char buffer[BUFFER] = {0};
                if (i == server_fd) {
                    new_socket = acceptClient(server_fd);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd) max_sd = new_socket;

                    char output[BUFFER] = {0};
                    sprintf(output, "New client with file descriptor %d connected\n", new_socket);
                    write(1, output, strlen(output));

                    sprintf(buffer, "Hello from server, you're client %d\nPlease choose your category :\n"
                                    "1 - Computer\n2 - Electric\n3 - Civil\n4 - Mechanic\n", new_socket);
                    send(new_socket, buffer, strlen(buffer), 0);

                    users[last_user].stage = CHOOSE_GROUP;
                    users[last_user].fd = new_socket;
                    last_user++;
                } else {
                    int bytes_received;
                    bytes_received = recv(i, buffer, BUFFER, 0);

                    if (bytes_received == 0) {
                        char output[BUFFER] = {0};
                        sprintf(output, "Client with file descriptor %d disconnected\n", i);
                        write(1, output, strlen(output));

                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    if (users[find_user(i)].stage == CHOOSE_GROUP) {
                        int gp = choose_group(buffer);
                        int gp_s = add_to_group(i, gp);
                        users[find_user(i)].stage = WAITING_START;
                        users[find_user(i)].group_category = gp;
                        if (gp_s) start_group(gp);
                    } else if (users[find_user(i)].stage == WAITING_START) {
                        sprintf(buffer, "Please wait ...\n");
                        send(i, buffer, strlen(buffer), 0);
                    } else if (users[find_user(i)].stage == IN_CHAT) {
                        int qa_file = open("QA.txt", O_APPEND | O_RDWR);
                        if (qa_file < 0) qa_file = open("QA.txt", O_CREAT | O_RDWR);
                        write(qa_file, buffer, strlen(buffer));
                        close(qa_file);
                    } else {
                        char output[BUFFER] = {0};
                        sprintf(output, "Client %d : %s", i, buffer);
                        write(1, output, strlen(output));
                    }
                }
            }
        }
    }
    return 0;
}

int acceptClient(int server_fd) {
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr *) &client_address, (socklen_t *) &address_len);
    return client_fd;
}

int setupServer(int s_port) {

    char output[BUFFER] = {0};
    sprintf(output, "Starting server on port %d\n", s_port);
    write(1, output, strlen(output));

    struct sockaddr_in address;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(s_port);

    bind(server_fd, (struct sockaddr *) &address, sizeof(address));
    listen(server_fd, 10);

    return server_fd;
}

int choose_group(char buffer[]) {
    return atoi(buffer) - 1;
}

int add_to_group(int client_fd, int group) {
    room current = rooms[group].rooms[rooms[group].index];
    current.users[current.to_fill - 1] = client_fd;
    current.to_fill--;
    rooms[group].rooms[rooms[group].index] = current;
    if (current.to_fill == 0) return 1;
    else {
        char buffer[BUFFER] = {0};
        sprintf(buffer, "Please wait until your group is full\n");
        send(client_fd, buffer, strlen(buffer), 0);
        return 0;
    }
}

int find_user(int fd) {
    for (int i = 0; i < 50; ++i) {
        if (users[i].fd == fd)
            return i;
    }
    return -1;
}

void start_group(int group) {
    port++;
    room current = rooms[group].rooms[rooms[group].index];
    current.port = port;

    char chat[BUFFER] = {0};
    sprintf(chat, "New chat in group %d started\n", group);
    write(1, chat, strlen(chat));

    for (int i = 0; i < ROOM_SIZE; ++i) {
        char buffer[BUFFER] = {0};
        sprintf(buffer, "$YCISOP$#%d#*%d*%d*%d*%d*\n",
                port, current.users[0], current.users[1], current.users[2], current.users[i]);
        send(current.users[i], buffer, strlen(buffer), 0);
        users[find_user(current.users[i])].stage = IN_CHAT;

        char output[BUFFER] = {0};
        sprintf(output, "Port %d transmitted to client %d \n", port, current.users[i]);
        write(1, output, strlen(output));
    }

    rooms[group].index++;
}
