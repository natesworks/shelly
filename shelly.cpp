#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <sys/stat.h>

using namespace std;

fstream configFile;
string prompt;
string input;

int writeConfig(const string& prompt) {
    string directoryPath = "/etc/shelly";

    if (access(directoryPath.c_str(), F_OK) != 0) {
        if (mkdir(directoryPath.c_str(), 0755) != 0) {
            return 0;
        }
    }

    configFile.open("/etc/shelly/config", ios::out);
    if (configFile.is_open()) {
        configFile << prompt;
        configFile.close();
        return 1;
    }
    return 0;
}

int readConfig() {
    configFile.open("/etc/shelly/config", ios::in);
    if (configFile.is_open()) {
        getline(configFile, prompt);
        configFile.close();
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (!readConfig()) {
        // TO:DO make a setup wizard so lazy people dont have to go through the pain of running a command
        cout << "Config file not found or empty. Defaulting to \"$ \"." << endl;
        cout << "Run \"set prompt\" to set a prompt." << endl;
        prompt = "$ ";
    }

    while(true) {
        cout << prompt;
        getline(cin, input);
        if (input.find("set prompt ") == 0) {
            prompt = input.substr(11);
        }
        else if (!input.empty()) {
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
    }
    return 1;
}
