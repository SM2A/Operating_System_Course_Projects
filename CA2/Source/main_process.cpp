#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fstream>

using namespace std;

int main() {

    pid_t mapper = fork();

    if (mapper == 0) {
        DIR *dp;
        int count = -2;
        struct dirent *ep;
        dp = opendir("testcases/");
        while ((ep = readdir(dp))) count++;
        closedir(dp);

        char *map_handler[] = {(char *) "map_handler", (char *) to_string(count).c_str(), NULL};
        execv("./Build/map_handler", map_handler);
    } else wait(NULL);

    sleep(1);

    int _pipe_[2];
    char data[2048] = {0};
    if (pipe(_pipe_) < 0) exit(1);

    pid_t reducer = fork();

    if (reducer == 0) {
        close(_pipe_[0]);
        char *reduce[] = {(char *) "reducer", (char *) to_string(_pipe_[1]).c_str(), NULL};
        execv("./Build/reduce", reduce);
    } else {
        close(_pipe_[1]);
        wait(NULL);
    }

    read(_pipe_[0], data, 2048);
    close(_pipe_[0]);

    ofstream result("output.csv");
    result << data;
    result.close();

    return 0;
}
