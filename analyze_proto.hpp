#include <vector>
#include <iostream>

using namespace std;
void io_small(int, vector<char *>);
void io_big(int, vector<char *>);
void io_append(int, vector<char *>);
void io_pipe(int i, vector<char *>);
void just_sh_command(int, vector<char *>);
char **find_left_arguments(int, int, vector<char *>);
char **find_right_arguments(int, int, vector<char *>, int *);
void analyze_tokens(vector<char *>, int *,vector<string>);
void io_bg(int, vector<char *>,vector<string>);
