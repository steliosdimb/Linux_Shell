#include "alias.hpp"
#include "parser_proto.hpp"
#include <algorithm>
using namespace std;
void aliases(vector<string> &aliases_names, vector<string> keep_sp_strings, vector<string> &aliases_command, vector<string> &keep_sp_strings1)
{
    // case 6,we enter a string that is an already created alias
    // aliases_names is a vector with aliases names,aliases command is a vector that contains the command that will be executed for the current alias name
    if (find(aliases_names.begin(), aliases_names.end(), keep_sp_strings[0]) != aliases_names.end() && keep_sp_strings.size() == 1)
    {
        auto it = find(aliases_names.begin(), aliases_names.end(), keep_sp_strings[0]);
        int index = it - aliases_names.begin();
        keep_sp_strings1 = split_string_spaces(aliases_command[index]); // get the command for the current alias name,split it with space delimeter,so we have a new vector
                                                                        // with every splited string in a different index
    }
    // case 1,command is createalias
    else if (keep_sp_strings[0] == "createalias")
    {
        if (find(aliases_names.begin(), aliases_names.end(), keep_sp_strings[1]) != aliases_names.end())
        { // if alias name already exists overwrite it
            auto it = find(aliases_names.begin(), aliases_names.end(), keep_sp_strings[1]);
            int index = it - aliases_names.begin();        // found the element in index position
            string temp1 = keep_sp_strings[2].erase(0, 1); // erase " from begining and ending of the command
            int getlength = keep_sp_strings[keep_sp_strings.size() - 1].length();
            string temp2 = keep_sp_strings[keep_sp_strings.size() - 1].erase(getlength - 1, getlength);
            string make_command = keep_sp_strings[2];
            for (int r = 3; r < keep_sp_strings.size(); r++)
            {
                make_command = make_command + " " + keep_sp_strings[r];
            }
            aliases_command[index] = make_command; // replace the existing command
        }
        else
        {                                                  // create new alias,alias name doesnt exist
            aliases_names.push_back(keep_sp_strings[1]);   // keep the name of alias
            string temp1 = keep_sp_strings[2].erase(0, 1); // erase " from begining and ending of the command
            int getlength = keep_sp_strings[keep_sp_strings.size() - 1].length();
            string temp2 = keep_sp_strings[keep_sp_strings.size() - 1].erase(getlength - 1, getlength);
            string make_command = keep_sp_strings[2];
            for (int r = 3; r < keep_sp_strings.size(); r++)
            {
                make_command = make_command + " " + keep_sp_strings[r];
            }
            if(find(aliases_names.begin(), aliases_names.end(), make_command) != aliases_names.end()){//if the command of the new alias is already a created alias
                auto it = find(aliases_names.begin(), aliases_names.end(), make_command);
                int index = it - aliases_names.begin();//find the position of the alias with the name of the command
                make_command=aliases_command[index];//get the command from the name of alias
            }

            aliases_command.push_back(make_command);
        }
    }
    // case 1 destroyalias
    else if (keep_sp_strings[0] == "destroyalias")
    {
        for (int r = 0; r < aliases_names.size(); r++)
        {
            if (aliases_names[r].compare(keep_sp_strings[1]) == 0)
            {
                aliases_names.erase(aliases_names.begin() + r);
                aliases_command.erase(aliases_command.begin() + r); // find the name of the alias and delete name and command
                break;
            }
        }
    }
}
