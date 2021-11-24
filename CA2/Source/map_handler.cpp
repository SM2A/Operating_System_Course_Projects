#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {

    int count = stoi(argv[1]);

    for (int i = 1; i <= count; ++i) {
        pid_t map_p = fork();
        if (map_p == 0) {
            int _pipe_[2];
            if (pipe(_pipe_) < 0) exit(1);
            char *map_handler[] = {(char *) "map", (char *) to_string(_pipe_[0]).c_str(), NULL};

            write(_pipe_[1], to_string(i).c_str(), to_string(i).size());
            execv("./Build/map", map_handler);
            close(_pipe_[1]);
        }
    }

    return 0;
}
