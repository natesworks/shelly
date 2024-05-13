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
string formatText(const string &value);
int shell();
int executeCommand(string command);
int executeCommandFromFile(string file);
int help(string parameters);
int echo(string text);
int set(string parameters);
int writeWelcome();
int getWelcome();
int writePrompt();
int getPrompt();
int createConfigDir();

int main(int argc, char *argv[])
{
    if (argc >= 3 && strcmp(argv[1], "-c") == 0)
    {
        string command;
        for (int args = 2; args < argc; args++)
        {
            cout << args;
            command.append(argv[args]);
            command.append(" ");
        }
        executeCommand(command);
        cout << command;
        exit(0);
    }

    // Get prompt from configuration and if not set use default prompt
    if (getPrompt())
    {
        cout << "Run \"set prompt\" to set a prompt." << endl;
        prompt = "\033[36m[{username}@{hostname}] \033[37m{cwd} \033[36m$ ";
        writePrompt();
    }

    // Get welcome from configuration and apply placeholders and colors
    getWelcome();
    if (welcome != "")
    {
        cout << formatText(welcome) << endl;
    }

    while (true)
    {
        shell();
    }
}

// Prompt and input handler
int shell()
{
    cout << formatText(prompt) + "\033[0m";
    getline(cin, input);
    executeCommand(input);
    return 0;
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
    else if (command.find("echo") == 0)
    {
        if (command.size() > 5)
        {
            echo(command.substr(5).data());
        }
    }
    else if (command.find("help") == 0)
    {
        if (command.size() > 5)
        {
            help(command.substr(5).data());
        }
        else
        {
            help("");
        }
    }
    else if (command != "")
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
                exit(1);
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

// Shows the help message
int help(string parameters)
{
    if (parameters == "commands")
    {
        cout << "Shelly provides the following built-in commands:\n\n"
                     "- set <prompt/placeholder> <value>: Allows changing the prompt or placeholder.\n"
                     "- cd <directory>: Changes the current directory.\n"
                     "- echo [text]: Writes text to the console.\n"
                     "- exit [exit code]: Exits the shell.\n";
    }
    else if (parameters == "placeholders")
    {
        cout << "Placeholders are a symbolic representation of something that will later be replaced with it's actual value later by the shell. This can be used in your prompt or welcome message.\n\n"
                     "- {cwd}: Current working directory.\n"
                     "- {username}: Username of the current user.\n"
                     "- {hostname}: Hostname (PC name).\n";
    }
    else if (parameters == "colors")
    {
        cout << "You can use the \\033[Xm sequence to adjust the foreground and background colors of your prompt or welcome message text. Simply substitute 'X' with the appropriate color code from the provided table:\n\n"
                     "| Color Name  | Foreground Color Code  | Background Color Code |\n"
                     "| Black       | 30                     | 40                    |\n"
                     "| Red         | 31                     | 41                    |\n"
                     "| Green       | 32                     | 42                    |\n"
                     "| Yellow      | 33                     | 43                    |\n"
                     "| Blue        | 34                     | 44                    |\n"
                     "| Magenta     | 35                     | 45                    |\n"
                     "| Cyan        | 36                     | 46                    |\n"
                     "| White       | 37                     | 47                    |\n"
                     "| Default     | 39                     | 49                    |\n"
                     "| Reset       | 0                      | 0                     |\n";
    }
    else
    {
        cout << "Available help parameters:\n"
                     "commands: Provides information about built-in commands.\n"
                     "placeholders: Explains the usage of placeholders in prompts or welcome messages.\n"
                     "colors: Describes how to use color codes in prompts or welcome messages.\n";
    }
    return 0;
}

// Writes text
int echo(string text)
{
    string formattedText = formatText(text);
    cout << formattedText << endl;
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
string formatText(const string &value)
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
            if (pos > 0 && newValue[pos - 1] == '\\')
            {
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

    pos = 0;
    while ((pos = newValue.find("\\n", pos)) != string::npos)
    {
        if (pos == 0 || (pos > 0 && newValue[pos - 1] != '\\'))
        {
            newValue.replace(pos, 2, "\n");
        }
        ++pos;
    }

    pos = 0;
    while ((pos = newValue.find("\\033", pos)) != string::npos)
    {
        newValue.replace(pos, 4, "\033");
        pos += 2;
    }

    pos = 0;
    while ((pos = newValue.find("\\{", pos)) != string::npos)
    {
        newValue.replace(pos, 2, "{");
        ++pos;
    }

    pos = 0;
    while ((pos = newValue.find("\\\\", pos)) != string::npos)
    {
        newValue.replace(pos, 2, "\\");
        ++pos;
    }

    return newValue;
}

// Writes the welcome message to configuration
int writeWelcome()
{
    string configDir = getHomeDirectory() + "/.config/shelly/";
    string configFile = configDir + "welcome";

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

    createConfigDir();
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

// Creates the config directory
int createConfigDir()
{
    string configDir = getHomeDirectory() + "/.config/";
    string shellyConfigDir = getHomeDirectory() + "/.config/shelly/";

    struct stat st = {0};
    if (stat(configDir.c_str(), &st) == -1)
    {
        if (mkdir(configDir.c_str(), 0700) == -1)
        {
            cerr << "Error: Unable to create directory: " << configDir << endl;
            return 1;
        }
    }

    if (stat(configDir.c_str(), &st) == -1)
    {
        if (mkdir(shellyConfigDir.c_str(), 0700) == -1)
        {
            cerr << "Error: Unable to create directory: " << shellyConfigDir << endl;
            return 1;
        }
    }

    return 0;
}
