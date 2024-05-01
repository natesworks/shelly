#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

using namespace std;

string prompt;
string input;

int writeConfig(const string& prompt) {
    string directoryPath = "/etc/shelly";

    if (access(directoryPath.c_str(), F_OK) != 0) {
        if (mkdir(directoryPath.c_str(), 0755) != 0) {
            return 0;
        }
    }

    ofstream configFile("/etc/shelly/config");
    if (configFile.is_open()) {
        configFile << prompt;
        configFile.close();
        return 1;
    }
    return 0;
}

int readConfig() {
    ifstream configFile("/etc/shelly/config");
    if (configFile.is_open()) {
        getline(configFile, prompt);
        configFile.close();
        return 1;
    }
    return 0;
}

string replaceCwd(const string& input) {
    string result = input;
    size_t pos = result.find("{cwd}");
    if (pos != string::npos) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            result.replace(pos, 5, cwd);
        }
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (!readConfig()) {
        cout << "Config file not found or empty. Defaulting to \"$ \"." << endl;
        cout << "Run \"set prompt\" to set a prompt." << endl;
        prompt = "$ ";
    }

    while(true) {
        string processedPrompt = replaceCwd(prompt);
        cout << processedPrompt;
        getline(cin, input);
        if (input.find("set prompt ") == 0) {
            prompt = input.substr(11);
        }
        else if (!input.empty()) {
            istringstream iss(input);
            vector<char*> args;
            string arg;
            while (iss >> arg) {
                args.push_back(strdup(arg.c_str()));
            }
            args.push_back(nullptr); 

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
