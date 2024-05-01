#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

class Shelly;

fstream configFile;
string input;
string prompt;

int writeConfig(string prompt)
{
    configFile.open("config", ios::out);
    configFile << prompt;
    return 1;
}

int readConfig()
{
    configFile.open("config", ios::in);
    configFile >> prompt;
    return 1;
}

int main(int argc, char* argv[])
{    
    if(argc >= 2)
    {
        if(strcmp(argv[1], "set") == 0 && strcmp(argv[2], "prompt") == 0) 
        {
            writeConfig(argv[3]);
            exit(1);
        }
    }
    readConfig();
    cout << prompt;
    cin >> input;
    return 1;
}

