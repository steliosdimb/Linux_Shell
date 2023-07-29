#include <iostream>
#include "history_proto.hpp"
#include "parser_proto.hpp"
#include "analyze_proto.hpp"
#include <string.h>
string push_to_history(vector<string> keep_sp_strings)
{
    string finalstring = keep_sp_strings[0];
    for (int i = 1; i < keep_sp_strings.size(); i++)
    {
        finalstring = finalstring + " " + keep_sp_strings[i];
    } // createsa string from a vector with string tokens and returns it
    return finalstring;
}
vector<string> is_myHistory(vector<char *> cstrings, vector<string> history, int *flag, int history_counter, vector<string> keep_sp_strings, int *any_changes,vector<string>every_string)
{
    if (strcmp("myHistory", cstrings[0]) == 0 && cstrings.size() == 2 && history.empty() == false) // if first string myHistory 2 arguments,and vector isnt empty
    {
        *any_changes = 1;         // command is myhistory
        if (cstrings.size() == 2) // if command is myHistory it only takes 1 argument a number between 1 and 20
        {
            int temp = atoi(cstrings[1]);
            if (temp >= 1 && temp <= 20)
            {
                // ektelese
                temp = temp - 1; // because if user gives 1 it means first element that is in the 0 position of the vector
                if (temp > history.size() - 1)
                {
                    cout << "there are only " << history.size() << " commands in history " << endl;
                    history = check_range(history, keep_sp_strings, history_counter);
                    return history;
                }
                string get_command;
                get_command = history[temp];                                     // taking the command in the x value from history vector
                vector<string> history_command;                                  // we need to tokenize it again because its form is for example(cat file1.txt > file2.txt)
                history_command = split_string_spaces(get_command);              // similar vector to keep_sp_string
                vector<char *> cstrings_history = to_chararray(history_command); // similar vector to cstrings
                analyze_tokens(cstrings_history, flag,every_string);
                if (*flag == 0)
                {
                    just_sh_command(*flag, cstrings_history);
                }

                history = check_range(history, keep_sp_strings, history_counter);
            }
            else
            {
                cout << "You exceeded the limit of myHistory you need to search in a range from 1 to 20" << endl;
                history = check_range(history, keep_sp_strings, history_counter);
            }
        }
    }
    else if (strcmp("myHistory", cstrings[0]) == 0 && cstrings.size() == 2 && history.empty() == true) // if command myHistory x and history is empty
    {
        *any_changes = 1;
        cout << "History is empty" << endl;
        history = check_range(history, keep_sp_strings, history_counter);
    }
    else if (strcmp("myHistory", cstrings[0]) == 0 && cstrings.size() == 1)
    { // case 7 myHistory without a second argument
        *any_changes = 1;
        for (int r = 0; r < history.size(); r++)
        {
            cout << r + 1 << "   " << history[r] << endl;
        }
        history = check_range(history, keep_sp_strings, history_counter);
    }
    return history;
}
vector<string> check_range(vector<string> history, vector<string> keep_sp_strings, int history_counter)
{
    // if history counter over 20 ,insert in the begining of the vector and delete the last item
    if (history_counter > 20)
    {
        history.pop_back(); // we excedeed the limit so we pop lasst element
        history.insert(history.begin(), push_to_history(keep_sp_strings));
    }
    else
    {
        // if not just enter in the begining
        history.insert(history.begin(), push_to_history(keep_sp_strings));
    }
    return history;
}