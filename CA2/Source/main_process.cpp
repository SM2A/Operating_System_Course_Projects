#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

using namespace std;

int main() {

    pid_t mapper = fork();
    pid_t reducer = fork();

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

    if (reducer == 0) {
        char *reduce[] = {(char *) "reducer", NULL};
        execv("./Build/reduce", reduce);
    } else wait(NULL);

    return 0;
}
