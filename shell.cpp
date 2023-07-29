#include <iostream>
#include <vector>
#include <signal.h>
#include "parser_proto.hpp"
#include "alias.hpp"
#include "wild_proto.hpp"
#include "analyze_proto.hpp"
#include "history_proto.hpp"
#include "global.hpp"
using namespace std;

int main(void)
{

    int history_counter = 0; // history_counter is a counter for checking how many commands are executed in my shell
    vector<string> history;
    vector<string> aliases_names;
    vector<string> aliases_command;
    while (1)
    {
        int any_changes = 0;
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        history_counter++;
        terminal_prints();
        vector<string> keep_string_till_comma;
        keep_string_till_comma = tokenize(); // this will return a vector with strings seperated by " " or > < | >>
                                             // if the command is createalias it will return and only split by spaces
                                             // also splits the command with delimeter ;
        if(keep_string_till_comma[0]==""){
            continue;
        }
        if(keep_string_till_comma[0]=="exit"){
            exit(0);
        }
        int l = 0;
        int k;
        while (l < keep_string_till_comma.size())
        {
            
            vector<string> keep_sp_strings;
            vector<string> keep_sp_strings1;
            for (k = l; k < keep_string_till_comma.size(); k++) //continues to parse the string till it finds ;
            {
                if (keep_string_till_comma[k] == ";")
                {
                    break;
                }
                else
                {
                    keep_sp_strings.push_back(keep_string_till_comma[k]); //add substring to this vector
                }
            }
            if(keep_sp_strings[0]=="exit"){
                exit(0);
            }
            l = k + 1; //continue for the other string in the line seperated by ;(if it exists)
            // now we can have three cases
            // 1.command is createalias,or destroyalias
            // 2.command is myHistory i
            // 3.command is a valid command without any pipes or redirections etc
            // 4.command is not a valid command
            // 5.command is a valid command with redirections,pipes,etc or combinations of them
            // 6.command is a created alias
            // 7.command is myHistory
            aliases(aliases_names, keep_sp_strings, aliases_command, keep_sp_strings1);
            if (keep_sp_strings[0] != "createalias" && keep_sp_strings[0] != "destroyalias")
            { // command is not createalias or destroyalias
                vector<char *> cstrings;
                if (keep_sp_strings1.empty() == true)
                {
                    cstrings = to_chararray(keep_sp_strings);
                }
                else
                {
                    cstrings = to_chararray(keep_sp_strings1); // case 6,we have a created alias
                }
                // to_chararray converts string vector to char * vector,doing this because execvp takes char * arguments
                cstrings = search_for_wild(cstrings); // search_for_wild searches for wild characters
                int flag = 0;
                history = is_myHistory(cstrings, history, &flag, history_counter, keep_sp_strings, &any_changes,keep_string_till_comma); // checks if the command is myHistory command
                if (any_changes == 1)
                {
                    continue; // case 2 command is myHistory
                }
                else // checks if command is case 5
                {
                    analyze_tokens(cstrings, &flag,keep_string_till_comma); // analyzes the tokens checking if there are pipes ,redirections ,background commands
                    if(flag_bg==1){
                        break; //if bg commands then break,because it will rerun for the same commands 
                    }
                }
                just_sh_command(flag, cstrings); // checks if command is case 3 or 4,a single command or a not valid command
            }
        }
        if(any_changes==0){
            history = check_range(history, keep_string_till_comma, history_counter); // adding the command to the history vector
        }
    }
}


