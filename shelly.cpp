#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pwd.h>

using namespace std;

string prompt;
string input;

string getHomeDirectory() {
    struct passwd *pw = getpwuid(getuid());
    return pw->pw_dir;
}

int writeConfig(const string& prompt) {
    string configDir = getHomeDirectory() + "/.config/shelly/";
    string configFile = configDir + "prompt";

    struct stat st = {0};
    if (stat(configDir.c_str(), &st) == -1) {
        if (mkdir(configDir.c_str(), 0700) == -1) {
            cerr << "Error: Unable to create directory: " << configDir << endl;
            return 0;
        }
    }

    ofstream ofs(configFile);
    if (!ofs.is_open()) {
        cerr << "Error: Unable to open config file for writing: " << configFile << endl;
        return 0;
    }

    ofs << prompt;
    ofs.close();
    return 1;
}

int readConfig() {
    string configDir = getHomeDirectory() + "/.config/shelly/";
    string configFile = configDir + "prompt";

    ifstream configFileStream(configFile);
    if (configFileStream.is_open()) {
        getline(configFileStream, prompt);
        configFileStream.close();
        return 1;
    }
    return 0;
}

string getPrompt()
{
    string refactoredPrompt = prompt;
    size_t pos = prompt.find("{cwd}");
    if (pos != string::npos) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            refactoredPrompt.replace(pos, 5, cwd);
        }
    }

    pos = prompt.find("{hostname}");
    if (pos != string::npos) {
        char hostname[1024];
        if (gethostname(hostname, sizeof(hostname)) != -1) {
            refactoredPrompt.replace(pos, 10, hostname);
        }
    }

    pos = prompt.find("{username}");
    if (pos != string::npos) {
        char username[1024];
        if (getlogin_r(username, sizeof(username)) == 0) {
            refactoredPrompt.replace(pos, 10, username);
        }
    }

    pos = prompt.find("\\033");
    if (pos != string::npos) {
        refactoredPrompt.replace(pos, 4, "\033");
    }

    return refactoredPrompt;
}

int main(int argc, char* argv[]) {
    if (!readConfig()) {
        cout << "Run \"set prompt\" to set a prompt." << endl;
        prompt = "[{username}@{hostname}]{cwd}% ";
    }

    while(true) {
        string refactoredPrompt = getPrompt();
        cout << refactoredPrompt + "\033[0m";
        getline(cin, input);
        if (input.find("set prompt ") == 0) {
            prompt = input.substr(11);
            if (!writeConfig(prompt)) {
                cerr << "Error: Failed to set prompt." << endl;
            }
        }
        else if (input == "exit") {
            exit(1);
        }
        else if (input.find("cd ") == 0) {
            chdir(input.substr(3).data());
        }
        else if (input == "get prompt") {
            cout << prompt << endl;
        }
        else if (input.find("echo ") == 0) {
            cout << input.substr(4) << endl;
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
