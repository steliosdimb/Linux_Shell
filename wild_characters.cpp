#include "wild_proto.hpp"
#include <glob.h>
#include <string.h>
vector<char *> search_for_wild(vector<char *> cstrings)
{
    glob_t glob_result;
    int flag;
    for (int i = 0; i < cstrings.size(); i++) // if a token contains any of the following characters then glob is called returning all the matching names it finds
    {
        flag = 0;
        if (strchr(cstrings[i], '*') != NULL || strchr(cstrings[i], '?') != NULL || (strchr(cstrings[i], '[') != NULL && strchr(cstrings[i], ']') != NULL) || (strchr(cstrings[i], '{') != NULL && strchr(cstrings[i], '}') != NULL))
        {
            if (glob(cstrings[i], GLOB_NOCHECK | GLOB_TILDE, NULL, &glob_result) != 0)
            {
                printf("Error: Could not expand wildcards in command\n");
                exit(1);
            }
            else
            {
                flag = 1;
            }
        }
        if (flag == 1)
        { // found wild character
            cstrings.erase(cstrings.begin() + i);
            int temp = i;
            for (int r = 0; r < glob_result.gl_pathc; r++)
            {
                cstrings.insert(cstrings.begin() + temp, glob_result.gl_pathv[r]);
                temp++;
            }
        }
    }
    return cstrings;
}