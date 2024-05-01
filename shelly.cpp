#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

fstream configFile;
string prompt;
string input;

int writeConfig(const string& prompt) {
    configFile.open("config", ios::out);
    if (configFile.is_open()) {
        configFile << prompt;
        configFile.close();
        return 1;
    }
    return 0;
}

int readConfig() {
    configFile.open("config", ios::in);
    if (configFile.is_open()) {
        getline(configFile, prompt);
        configFile.close();
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc >= 4 && strcmp(argv[1], "set") == 0 && strcmp(argv[2], "prompt") == 0) {
        return writeConfig(argv[3]);
    }

    if (!readConfig()) {
        cout << "Config file not found or empty. Defaulting to \"$ \"." << endl;
        prompt = "$ ";
        writeConfig(prompt);
    }

    while(true) {
        cout << prompt;
        getline(cin, input);

        #ifdef _WIN32
            system(input.c_str());
        #else
            if (!input.empty()) {
                // i have no idea what the fuck im doing
                istringstream iss(input);
                vector<char*> args;
                string arg;
                while (iss >> arg) {
                    args.push_back(strdup(arg.c_str()));
                }
                args.push_back(nullptr); 

                // had to make it more complicated cuz the easy way exited the program after running a command even in while true loop
                pid_t pid = fork();
                if (pid == 0) {
                    execvp(args[0], args.data());
                    exit(1);
                } else if (pid > 0) {
                    wait(NULL);
                } else {
                    cout << "Failed to fork process." << endl;
                }

                for (auto& arg : args) {
                    free(arg);
                }
            }
        #endif
    }
    return 1;
}
