#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>

#define SIZE 10

using namespace std;

int main(int argc, char *argv[]) {

    int read_pipe = stoi(argv[1]);

    char file_name[SIZE];
    read(read_pipe, file_name, SIZE);
    close(read_pipe);

    map<string, int> words;

    string line;
    string file_path = "testcases/" + string(file_name) + ".csv";
    ifstream file(file_path);

    getline(file, line);
    stringstream stream(line);

    while (stream.good()) {
        string word;
        getline(stream, word, ',');
        if (!word.empty()) {
            auto iterator = words.find(word);
            if (iterator == words.end()) words.insert(pair<string, int>(word, 1));
            else iterator->second = (iterator->second + 1);
        }
    }

    for (auto & word : words)
        cout << word.first << " : " << word.second << endl;

    cout << endl;

    file.close();

    return 0;
}


/*
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char* argv[])
{

    int pid = fork();

    if(pid == 0) //child process: reducer process
    {
        for(int i = 1; i <= MAX_FILES; i++)
        {
            int fd[2];
            if(pipe(fd) < 0)
                perror("error on pipe!");

            int pid = fork();
            if(pid < 0)
                perror("error on fork mapper  process!");

            if(pid == 0) // child process : mapper process
            {
                close(fd[WRITE]);
                char buf[30];
                read(fd[READ], buf, 30);
                close(fd[READ]);

                char *args[]={(char *)"./mapper.out", buf, (char *)NULL};
                execv(args[0], args);
            }
            else // parent process : reducer process
            {
                close(fd[READ]);
                if(mkfifo(REDUCER_FIFO, 0777))
                    if(errno != EEXIST)
                        perror("error on mkfifo!");


                string filepath = argv[1] + to_string(i) + ".csv";
                write(fd[WRITE], filepath.c_str(),filepath.size());
                close(fd[WRITE]);

                // int fifo_d = open(REDUCER_FIFO, O_RDONLY);
                // char buf[2048];
                // bzero(buf, 2048);
                // read(fifo_d, buf, 1024);
                // close(fifo_d);
                // cout << buf << endl;
                wait(NULL);
            }
        }
        // char* args[] = {(char *)"./reducer.out", (char *)NULL};
        // execv(args[0], args);
    }
    else // parent process: main
    {
        wait(NULL);
    }
    return 0;
}*/
