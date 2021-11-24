#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[]) {

    int count = stoi(argv[1]);

    cout << "handler , count = " << count << endl;

    for (int i = 1; i <= count; ++i) {
        pid_t map_p = fork();
        if (map_p == 0) {
            int _pipe_[2];
            if (pipe(_pipe_) < 0) exit(1);
            char *map_handler[] = {(char *) "map", (char *) to_string(_pipe_[0]).c_str(), NULL};
            cout << "handler fork" << endl;
            write(_pipe_[1], to_string(i).c_str(), to_string(i).size());
            execv("./Build/map", map_handler);
            close(_pipe_[1]);
        } else wait(NULL);
    }

    cout << "handler end" << endl;
    return 0;
}
