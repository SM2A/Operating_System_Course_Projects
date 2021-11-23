#include <iostream>
#include <unistd.h>
#include <dirent.h>

using namespace std;

int main() {

    DIR *dp;
    int count = -2;
    struct dirent *ep;
    dp = opendir("testcases/");
    while (ep = readdir(dp)) count++;
    closedir(dp);

    char *argv_list[] = {"map", const_cast<char *>(to_string(count).c_str()), NULL};
    char *argv_list1[] = {"reduce",const_cast<char *>(to_string(count).c_str()), NULL};

    pid_t child = fork();

    if (child == 0) {
        execv("./Build/map", argv_list);
    }
    child = fork();
    if (child == 0) {
        execv("./Build/reduce", argv_list1);
    }


    return 0;
}
