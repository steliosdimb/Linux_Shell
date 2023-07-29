#include <iostream>
#include <string.h>
#include <vector>
#include <sys/wait.h>
#include <glob.h>
#include <algorithm>
#include <limits.h>
#include <fcntl.h>
#include "global.hpp"
#include <unistd.h>
using namespace std;
vector<string> remove_new_line(vector<string> keep_sp_strings)
{
    // removes new line chararacter from the last string of the vector
    int length = keep_sp_strings[keep_sp_strings.size() - 1].length();
    if (keep_sp_strings[keep_sp_strings.size() - 1][length - 1] == '\n')
    {
        keep_sp_strings[keep_sp_strings.size() - 1].pop_back();
    }
    return keep_sp_strings;
}
vector<string> tokenize(void)
{
    // reading the input from the user,and splitting the string into tokens with space delimeter
    vector<string> keep_sp_strings;
    char data[1024];
    char *splitter;
    if (fgets(data, sizeof data, stdin))
    {
        splitter = strtok(data, " ");
        while (splitter != NULL)
        {
            if (splitter == " ")
            {
                splitter = strtok(NULL, " ");
                continue;
            }
            keep_sp_strings.push_back(splitter);
            splitter = strtok(NULL, " ");
        }
    }
    keep_sp_strings = remove_new_line(keep_sp_strings);
    // split tokens with different delimeters such as > < | >> ,for example having a command ls> should be splitted in two tokens (ls >)
    for (int i = 0; i < keep_sp_strings.size(); i++)
    {
        if (keep_sp_strings[i].compare("<") != 0 && keep_sp_strings[i].compare(">") != 0 && keep_sp_strings[i].compare(">>") != 0 && keep_sp_strings[i].compare("|") != 0 && (keep_sp_strings[i][0] != '&') && keep_sp_strings[i].compare(";") != 0)
        {
            if (keep_sp_strings[i].find('<') != string::npos && keep_sp_strings[0] != "createalias")
            {
                int index = keep_sp_strings[i].find('<');
                if (index == 0) // having a string with the < character in the begining
                {
                    keep_sp_strings.insert(keep_sp_strings.begin() + i, "<");
                    keep_sp_strings[i + 1].erase(keep_sp_strings[i + 1].begin() + index);
                }
                else if (index == keep_sp_strings[i].size() - 1) // having the < character in the end
                {
                    keep_sp_strings.insert(keep_sp_strings.begin() + i + 1, "<");
                    keep_sp_strings[i].erase(keep_sp_strings[i].begin() + index);
                }
                else // when < character is not in the end or the begining it means that the string must be splitted into three atleast tokens(or more)
                {
                    size_t pos = 0;
                    string tok;
                    int counter = i;
                    while ((pos = keep_sp_strings[counter].find("<")) != string::npos)
                    {
                        tok = keep_sp_strings[counter].substr(0, pos);
                        keep_sp_strings.insert(keep_sp_strings.begin() + counter, tok);
                        keep_sp_strings[counter + 1].erase(0, pos);
                    }
                }
            } // same for > character
            if (keep_sp_strings[i].find('>') != string::npos && keep_sp_strings[0] != "createalias")
            {
                int index = keep_sp_strings[i].find('>');
                if (keep_sp_strings[i][index + 1] == '>') // checking if there is an append >> in the string if so handle it differently
                {
                    // we have >>
                    if (index == 0)
                    {
                        keep_sp_strings.insert(keep_sp_strings.begin() + i, ">>");
                        keep_sp_strings[i + 1].erase(keep_sp_strings[i + 1].begin() + index);
                        keep_sp_strings[i + 1].erase(keep_sp_strings[i + 1].begin() + index);
                    }
                    else if (index + 1 == keep_sp_strings[i].size() - 1)
                    {
                        keep_sp_strings.insert(keep_sp_strings.begin() + i + 1, ">>");
                        keep_sp_strings[i].erase(keep_sp_strings[i].begin() + index + 1);
                        keep_sp_strings[i].erase(keep_sp_strings[i].begin() + index);
                    }
                    else
                    {
                        size_t pos = 0;
                        string tok;
                        int counter = i;
                        while ((pos = keep_sp_strings[counter].find(">>")) != string::npos)
                        {
                            tok = keep_sp_strings[counter].substr(0, pos);
                            keep_sp_strings.insert(keep_sp_strings.begin() + counter, tok);
                            keep_sp_strings[counter + 1].erase(0, pos);
                        }
                    }
                }
                else // case for >
                {
                    if (index == 0)
                    {
                        keep_sp_strings.insert(keep_sp_strings.begin() + i, ">");
                        keep_sp_strings[i + 1].erase(keep_sp_strings[i + 1].begin() + index);
                    }
                    else if (index == keep_sp_strings[i].size() - 1)
                    {
                        keep_sp_strings.insert(keep_sp_strings.begin() + i + 1, ">");
                        keep_sp_strings[i].erase(keep_sp_strings[i].begin() + index);
                    }
                    else
                    {
                        size_t pos = 0;
                        string tok;
                        int counter = i;
                        while ((pos = keep_sp_strings[counter].find(">")) != string::npos)
                        {
                            tok = keep_sp_strings[counter].substr(0, pos);
                            keep_sp_strings.insert(keep_sp_strings.begin() + counter, tok);
                            keep_sp_strings[counter + 1].erase(0, pos);
                        }
                    }
                }
            } // case of |
            if (keep_sp_strings[i].find('|') != string::npos && keep_sp_strings[0] != "createalias")
            {
                int index = keep_sp_strings[i].find('|');
                if (index == 0)
                {
                    keep_sp_strings.insert(keep_sp_strings.begin() + i, "|");
                    keep_sp_strings[i + 1].erase(keep_sp_strings[i + 1].begin() + index);
                }
                else if (index == keep_sp_strings[i].size() - 1)
                {
                    keep_sp_strings.insert(keep_sp_strings.begin() + i + 1, "|");
                    keep_sp_strings[i].erase(keep_sp_strings[i].begin() + index);
                }
                else
                {
                    size_t pos = 0;
                    string tok;
                    int counter = i;
                    while ((pos = keep_sp_strings[counter].find("|")) != string::npos)
                    {
                        tok = keep_sp_strings[counter].substr(0, pos);
                        keep_sp_strings.insert(keep_sp_strings.begin() + counter, tok);
                        keep_sp_strings[counter + 1].erase(0, pos);
                    }
                }
            }
            if (keep_sp_strings[i].find(';') != string::npos)
            {
                int index = keep_sp_strings[i].find(';');
                if (index == 0) // having a string with the ; character in the begining
                {
                    keep_sp_strings.insert(keep_sp_strings.begin() + i, ";");
                    keep_sp_strings[i + 1].erase(keep_sp_strings[i + 1].begin() + index);
                }
                else if (index == keep_sp_strings[i].size() - 1) // having the ; character in the end
                {
                    keep_sp_strings.insert(keep_sp_strings.begin() + i + 1, ";");
                    keep_sp_strings[i].erase(keep_sp_strings[i].begin() + index);
                }
                else // when ; character is not in the end or the begining it means that the string must be splitted into three atleast tokens(or more)
                {
                    size_t pos = 0;
                    string tok;
                    int counter = i;
                    while ((pos = keep_sp_strings[counter].find(";")) != string::npos)
                    {
                        tok = keep_sp_strings[counter].substr(0, pos);
                        keep_sp_strings.insert(keep_sp_strings.begin() + counter, tok);
                        keep_sp_strings[counter + 1].erase(0, pos);
                    }
                }
            } // case of &
            if (keep_sp_strings[i].find('&') != string::npos && keep_sp_strings[0] != "createalias")
            { //& can only be at the end of a string
                int index = keep_sp_strings[i].find('&');
                if (index == keep_sp_strings[i].size() - 1 || index == keep_sp_strings[i].size() - 2) // it can also contains ; after &
                {
                    keep_sp_strings.insert(keep_sp_strings.begin() + i + 1, "&");
                    keep_sp_strings[i].erase(keep_sp_strings[i].begin() + index);
                }
            }
        }
    }

    return keep_sp_strings; // return string tokens
}
vector<char *> to_chararray(vector<string> keep_sp_strings)
{
    std::vector<char *> char_array;
    char_array.reserve(keep_sp_strings.size());

    for (const auto &str : keep_sp_strings)
    {
        char *cstr = new char[str.length() + 1];
        strcpy(cstr, str.c_str());
        char_array.push_back(cstr);
    }
    // this function converts string vector to char * vector
    return char_array;
}
vector<string> split_string_spaces(string command)
{
    // splits a string with delimeter space
    size_t pos = 0;
    string token;
    vector<string> splitted;
    string del = " ";
    while ((pos = command.find(del)) != string::npos)
    {
        token = command.substr(0, pos);
        splitted.push_back(token);
        command.erase(0, pos + del.length());
    }
    splitted.push_back(command);
    return splitted;
}

void terminal_prints(void)
{
    char cwd[PATH_MAX];
    cout << "in-mysh-now:~";
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {

        cout << "\033[1;32m" << cwd << "\033[0m";
    }
    else
    {
        perror("getcwd() error");
        return;
    }
}



