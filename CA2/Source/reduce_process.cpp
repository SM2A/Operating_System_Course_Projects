#include <map>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sstream>

using namespace std;

int main(int argc, char *argv[]) {

    char *fifo = (char *) "Build/my_fifo";
    mkfifo(fifo, 0666);

    string line;
    ifstream fd(fifo);

    map<string, int> words;

    while (getline(fd, line)) {
        while (!fd.is_open()) fd.open(fifo);

        stringstream stream(line);
        string word, count;

        getline(stream, word, ',');
        getline(stream, count);

        auto iterator = words.find(word);
        if (iterator == words.end()) words.insert(pair<string, int>(word, stoi(count)));
        else iterator->second = (iterator->second + stoi(count));
    }
    fd.close();

    string text;
    for (auto &word : words) text.append(word.first + " , " + to_string(word.second) + "\n");
    cout << text << endl;

    return 0;
}
