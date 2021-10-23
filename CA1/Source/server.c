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
int choose_group(int client_fd);
int add_to_group(int client_fd, int group);
void start_group(int group);

int port;
category rooms[CATEGORY_COUNT];

int main(int argc, char *argv[]) {

    char buffer[BUFFER] = {0};
    fd_set master_set, working_set;
    int server_fd, new_socket, max_sd;
    category rooms[CATEGORY_COUNT];

    for (int i = 0; i < CATEGORY_COUNT; ++i) rooms[i].index = 0;
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

                if (i == server_fd) {
                    new_socket = acceptClient(server_fd);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd) max_sd = new_socket;
                    printf("New client with file descriptor %d connected\n", new_socket);
                    send(new_socket, buffer, strlen(buffer), 0);
                    int gp = choose_group(new_socket);
                    int gp_s = add_to_group(new_socket,gp);
                    if (gp_s) start_group(gp);
                } else {
                    int bytes_received;
                    bytes_received = recv(i, buffer, BUFFER, 0);

                    if (bytes_received == 0) {
                        printf("Client with file descriptor %d disconnected\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    printf("Client %d : %s", i, buffer);
                    memset(buffer, 0, BUFFER);
                }
            }
        }
    }
    return 0;
}

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *) &client_address, (socklen_t *) &address_len);
    return client_fd;
}

int setupServer(int port) {

    printf("Starting server on port %d\n", port);

    struct sockaddr_in address;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *) &address, sizeof(address));
    listen(server_fd, 10);

    return server_fd;
}

int choose_group(int client_fd) {
    char buffer[BUFFER] = {0};
    sprintf(buffer, "Hello from server, you're client %d\nPlease choose your category :\n"
                    "1 - Computer\n2 - Electric\n3 - Civil\n4 - Mechanic\n", client_fd);

    send(client_fd, buffer, strlen(buffer), 0);

    char response[10] = {0};
    recv(client_fd, response, BUFFER, 0);

    return atoi(response) - 1;
//    return COMPUTER;
}

int add_to_group(int client_fd, int group) {
    rooms[group].rooms[rooms[group].index].users[rooms[group].rooms[rooms[group].index].to_fill-1] = client_fd;
    rooms[group].rooms[rooms[group].index].to_fill--;
    rooms[group].index++;
    return rooms[group].rooms[rooms[group].index].to_fill == 0;
}

void start_group(int group){
    port++;
    rooms[group].rooms[rooms[group].index-1].port = port;
    char port_str[10];
    sprintf(port_str,"%d",port);

    for (int i = 0; i < ROOM_SIZE ; ++i) {
        send(rooms[group].rooms[rooms[group].index-1].users[i],port_str,strlen(port_str),0);
    }
}
