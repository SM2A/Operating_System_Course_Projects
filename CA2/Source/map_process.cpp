#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

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
    file.close();

    char *fifo = (char *) "Build/my_fifo";
    mkfifo(fifo, 0666);

    string text;
    for (auto &word : words) text.append(word.first + " , " + to_string(word.second) + "\n");

    ofstream fd(fifo);
    while (!fd.is_open()) fd.open(fifo);
    fd << text;
    fd.close();

    return 0;
}
