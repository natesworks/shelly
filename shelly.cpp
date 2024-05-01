#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/wait.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <unistd.h>

using namespace std;

fstream configFile;
string prompt;
char input[100];

int writeConfig(const string& prompt) {
    configFile.open("config", ios::out);
    configFile << prompt;
    configFile.close();
    return 1;
}

int readConfig() {
    configFile.open("config", ios::in);
    if (configFile) {
        getline(configFile, prompt);
        configFile.close();
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc >= 4 && strcmp(argv[1], "set") == 0 && strcmp(argv[2], "prompt") == 0) {
        writeConfig(argv[3]);
        return 0;
    }

    if (!readConfig()) {
        cout << "Config file not found or empty. Defaulting to \"$ \"." << endl;
        writeConfig("$ ");
        prompt = "$ ";
    }
    
    while(true)
    {
        cout << prompt;
        cin.getline(input, sizeof(input));

        #ifdef _WIN32
            system(input);
        #else
            if (strlen(input) > 0) {
                pid_t pid = fork();
                if (pid == 0) {
                    execlp(input, input, NULL);
                    exit(1);
                } else if (pid > 0) {
                    wait(NULL);
                } else {
                    cout << "Failed to fork process." << endl;
                }
            }
        #endif
    }
    return 1;
}
