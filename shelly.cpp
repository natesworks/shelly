#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pwd.h>
#include <filesystem>

using namespace std;

string prompt;
string welcome;
string input;

// Defining methods
string getHomeDirectory();
string applyPlaceholders(const string &value);
int executeCommand(string command);
int executeCommandFromFile(string file);
int set(string parameters);
int writeWelcome();
int getWelcome();
int writePrompt();
int getPrompt();

int main(int argc, char *argv[])
{
    // Get prompt from configuration and if not set use default prompt
    if (getPrompt())
    {
        cout << "Run \"set prompt\" to set a prompt." << endl;
        prompt = "[{username}@{hostname}]{cwd}% ";
        writePrompt();
    }

    // Get welcome from configuration and apply placeholders and colors
    getWelcome();
    if (welcome != "")
    {
        cout << applyPlaceholders(welcome) << endl;
    }

    while (true)
    {
        cout << applyPlaceholders(prompt) + "\033[0m";
        getline(cin, input);
        executeCommand(input);
    }
}

// Executes a command
int executeCommand(string command)
{
    if (command.find("cd ") == 0)
    {
        chdir(command.substr(3).data());
    }
    else if (command.find("set") == 0)
    {
        if (command.size() >= 4)
        {
            set(command.substr(4).data());
        }
        else
        {
            set("");
        }
    }
    else if (command.find("exit") == 0)
    {
        int exitcode = 1;
        if (command.size() > 5)
        {
            exitcode = stoi(command.substr(5));
        }
        exit(exitcode);
    }
    else
    {
        executeCommandFromFile(command);
    }
    return 0;
}

// Executes a file
int executeCommandFromFile(string file)
{
    istringstream iss(file);
    vector<char *> args;
    string arg;
    while (iss >> arg)
    {
        args.push_back(strdup(arg.c_str()));
    }
    args.push_back(nullptr);

    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], const_cast<char *const *>(args.data())) == -1)
        {
            if (errno == ENOENT)
            {
                cerr << "Error: Command '" << file << "' not found." << endl;
            }
        }
    }
    else if (pid > 0)
    {
        wait(NULL);
    }
    else
    {
        cout << "Failed to fork process." << endl;
    }
    for (auto &arg : args)
    {
        free(arg);
    }
    return 0;
}

// Change settings
int set(string parameters)
{
    if (parameters.find("prompt ") == 0)
    {
        prompt = parameters.substr(7);
        writePrompt();
    }
    if (parameters.find("welcome ") == 0)
    {
        welcome = parameters.substr(8);
        writeWelcome();
    }
    return 0;
}

// Gets the home directory
string getHomeDirectory()
{
    struct passwd *pw = getpwuid(getuid());
    return pw->pw_dir;
}

// Aplies placeholders and colors
string applyPlaceholders(const string &value)
{
    string newValue = value;
    size_t pos = 0;

    string username = getlogin();

    char hostname_buffer[1024];
    gethostname(hostname_buffer, sizeof(hostname_buffer) - 1);
    hostname_buffer[sizeof(hostname_buffer) - 1] = '\0';
    string hostname(hostname_buffer);

    while (pos < newValue.length())
    {
        if (newValue[pos] == '{')
        {
            size_t endPos = newValue.find('}', pos + 1);
            if (pos > 0 && newValue[pos - 1] == '\\') {
                ++pos;
                continue;
            }
            if (endPos != string::npos)
            {
                string placeholder = newValue.substr(pos, endPos - pos + 1);
                if (placeholder == "{cwd}")
                {
                    string cwd = filesystem::current_path().string();
                    newValue.replace(pos, placeholder.length(), cwd);
                    pos += cwd.length() - 1;
                }
                else if (placeholder == "{username}")
                {
                    newValue.replace(pos, placeholder.length(), username);
                    pos += username.length() - 1;
                }
                else if (placeholder == "{hostname}")
                {
                    newValue.replace(pos, placeholder.length(), hostname);
                    pos += hostname.length() - 1;
                }
            }
        }
        ++pos;
    }

    while ((pos = newValue.find("\\033")) != string::npos)
    {
        newValue.replace(pos, 4, "\033");
    }

    while ((pos = newValue.find("\\n")) != string::npos)
    {
        newValue.replace(pos, 3, "\n");
    }

    while((pos = newValue.find("\\")) != string::npos)
    {
        newValue.replace(pos, 1, "");
    }

    return newValue;
}

// Writes the welcome message to configuration
int writeWelcome()
{
    string configDir = getHomeDirectory() + "/.config/shelly/";
    string configFile = configDir + "welcome";

    struct stat st = {0};
    if (stat(configDir.c_str(), &st) == -1)
    {
        if (mkdir(configDir.c_str(), 0700) == -1)
        {
            cerr << "Error: Unable to create directory: " << configDir << endl;
            return 1;
        }
    }

    ofstream ofs(configFile);
    if (!ofs.is_open())
    {
        cerr << "Error: Unable to open config file for writing: " << configFile << endl;
        return 1;
    }

    ofs << welcome;
    ofs.close();
    return 0;
}

// Gets the welcome message from the configuration
int getWelcome()
{
    string configDir = getHomeDirectory() + "/.config/shelly/";
    string configFile = configDir + "welcome";

    ifstream configFileStream(configFile);
    if (configFileStream.is_open())
    {
        getline(configFileStream, welcome);
        configFileStream.close();
        return 0;
    }
    return 1;
}

// Writes the prompt to configuration
int writePrompt()
{
    string configDir = getHomeDirectory() + "/.config/shelly/";
    string configFile = configDir + "prompt";

    struct stat st = {0};
    if (stat(configDir.c_str(), &st) == -1)
    {
        if (mkdir(configDir.c_str(), 0700) == -1)
        {
            cerr << "Error: Unable to create directory: " << configDir << endl;
            return 1;
        }
    }

    ofstream ofs(configFile);
    if (!ofs.is_open())
    {
        cerr << "Error: Unable to open config file for writing: " << configFile << endl;
        return 1;
    }

    ofs << prompt;
    ofs.close();
    return 0;
}

// Gets the prompt from the configuration
int getPrompt()
{
    string configDir = getHomeDirectory() + "/.config/shelly/";
    string configFile = configDir + "prompt";

    ifstream configFileStream(configFile);
    if (configFileStream.is_open())
    {
        getline(configFileStream, prompt);
        configFileStream.close();
        return 0;
    }
    return 1;
}