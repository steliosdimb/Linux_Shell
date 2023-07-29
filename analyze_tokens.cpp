#include "analyze_proto.hpp"
#include <string.h>
#include <iostream>
#include <vector>
#include <sys/wait.h>
#include <algorithm>
#include <fcntl.h>
#include "global.hpp"
#include "parser_proto.hpp"
#include <unistd.h>
pid_t pid1;
pid_t pid2;
pid_t pid;
int child_running = 0;
int flag_bg=0;
using namespace std;
void analyze_tokens(vector<char *> cstrings, int *flag,vector<string> every_string)
{
    // if a token is < > | >> or & then flag=1 and appropriate function is called
    for (int i = 0; i < cstrings.size(); i++)
    {

        if (strcmp("<", cstrings[i]) == 0)
        {
            *flag = 1;
            io_small(i, cstrings);
        }
        else if (strcmp(">", cstrings[i]) == 0)
        {
            *flag = 1;
            io_big(i, cstrings);
        }
        else if (strcmp(">>", cstrings[i]) == 0)
        {
            *flag = 1;
            io_append(i, cstrings);
        }
        else if (strcmp("|", cstrings[i]) == 0)
        {
            *flag = 1;
            io_pipe(i, cstrings);
        }
        else if (strcmp("&", cstrings[i]) == 0)
        {
            *flag = 1;
            io_bg(i, cstrings,every_string);
        }
        if (*flag == 1)
        {
            break;
        }
    }
}
void io_small(int i, vector<char *> cstrings)
{
    // function called when found a character <
    int k = i - 1;
    char **outputargv;
    outputargv = find_left_arguments(i, k, cstrings); // find all left arguments(stops when it finds (<,>,|,etc) or its reaches the begining of the string)
    char *outfile = outputargv[0];                    // if it is a file should be one word file1.txt myprogram etc
    k = i + 1;
    char **inputargv;
    int temp;
    inputargv = find_right_arguments(i, k, cstrings, &temp); // same technique as left arguments
    char *infile;
    infile = inputargv[0];
    if (temp != cstrings.size() - 1)
    {
        int pos;
        for (int r = temp; r < cstrings.size(); r++)
        {
            if (strcmp(">", cstrings[r]) == 0)
            {
                pos = r;
                break;
            }
        }
        pid = fork();
        int status;
        if (pid == 0)
        {
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            int fd_in, fd_out;
            fd_in = open(infile, O_RDONLY | O_CREAT, 0666);
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
            char *out = cstrings[pos + 1];
            fd_out = open(out, O_WRONLY | O_CREAT, 0666);
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
            execvp(outfile, outputargv);
        }
        else
        {
            child_running = 1;
            waitpid(pid, &status, WUNTRACED);
            if (WIFSTOPPED(status))
            {
                cout << "Stopped" << endl;
            }
            child_running = 0;
        }
    }
    else
    {
        pid = fork(); // create a new process
        int status;
        if (pid == 0) // if process pid is 0 ,child process
        {
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            int fd_in, fd_out;
            fd_out = open(outfile, O_WRONLY | O_CREAT, 0666); // so output is right,open it for writing
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
            if (execvp(infile, inputargv) == -1)
            {
                fd_in = open(infile, O_RDONLY | O_CREAT, 0666); // so output is right,open it for writing
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
                execvp(outfile, outputargv);
            }
        }
        else // parent process waits for child process to finish
        {
            child_running = 1;
            waitpid(pid, &status, WUNTRACED);
            if (WIFSTOPPED(status))
            {
                cout << "Stopped" << endl;
            }
            child_running = 0;
        }
    }
}
void io_big(int i, vector<char *> cstrings)
{
    int k = i - 1;
    char **inputargv;
    inputargv = find_left_arguments(i, k, cstrings);
    char *infile = inputargv[0]; // if it is a file should be one word file1.txt myprogram etc
    k = i + 1;
    char **outputargv;
    int temp;
    outputargv = find_right_arguments(i, k, cstrings, &temp);
    char *outfile;

    outfile = outputargv[0];

    // same process as io_small but infile and outfile should be opposite
    pid = fork();
    int status;
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        int fd_in, fd_out;
        fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        cout << fd_out << endl;
        dup2(fd_out, STDOUT_FILENO);
        
        close(fd_out);
        if (execvp(infile, inputargv) == -1)
        {
            fd_in = open(infile, O_RDONLY | O_CREAT, 0666);
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
            execvp(outfile, outputargv);
        }
    }
    else
    {
        child_running = 1;
        waitpid(pid, &status, WUNTRACED);
        if (WIFSTOPPED(status))
        {
            cout << "Stopped" << endl;
        }
        child_running = 0;
    }
}
void io_append(int i, vector<char *> cstrings)
{
    int k = i - 1;
    char **inputargv;
    inputargv = find_left_arguments(i, k, cstrings);
    char *infile = inputargv[0]; // if it is a file should be one word file1.txt myprogram etc
    k = i + 1;
    int temp;
    char **outputargv;
    outputargv = find_right_arguments(i, k, cstrings, &temp);
    char *outfile;
    outfile = outputargv[0];

    // same technique like the functions above
    //  case for having a command in the input so the output should be written to the file
    pid = fork();
    int status;
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        int fd_in, fd_out;
        fd_out = open(outfile, O_WRONLY | O_APPEND, 0666); // give O_APPEND flag,append to an existing file and not overwrite it
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
        if (execvp(infile, inputargv) == -1)
        {
            fd_in = open(infile, O_RDONLY | O_CREAT, 0666);
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
            execvp(outfile, outputargv);
        }
    }
    else
    {
        child_running = 1;
        waitpid(pid, &status, WUNTRACED);
        if (WIFSTOPPED(status))
        {
            cout << "Stopped" << endl;
        }
        child_running = 0;
    }
}
void io_pipe(int i, vector<char *> cstrings)
{
    int k = i - 1;
    char **inputargv;
    inputargv = find_left_arguments(i, k, cstrings);
    char *infile = inputargv[0]; // if it is a file should be one word file1.txt myprogram etc
    k = i + 1;
    char **outputargv;
    int temp;
    outputargv = find_right_arguments(i, k, cstrings, &temp);
    char *outfile;
    outfile = outputargv[0];
    if (temp != cstrings.size() - 1) // we have a pipe and something else after
    {
        int pos;
        for (int r = temp; r < cstrings.size(); r++)
        {
            if (strcmp(">", cstrings[r]) == 0)
            {
                pos = r;
                break;
            }
        }

        int pipefds[2];
        // this is the technique so two processes can communicate
        if (pipe(pipefds) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid1 = fork(); // forking for the first process
        if (pid1 == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid1 == 0)
        { // Child process 1
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            dup2(pipefds[1], STDOUT_FILENO); // this process should write
            close(pipefds[0]);
            close(pipefds[1]);
            execvp(infile, inputargv);
            exit(EXIT_FAILURE);
        }
        else
        {                  // Parent process
            pid2 = fork(); // forking the other process
            if (pid2 == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid2 == 0)
            {
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                int fd_out; // Child process 2
                fd_out = open(cstrings[pos + 1], O_WRONLY | O_CREAT, 0666);
                dup2(pipefds[0], STDIN_FILENO); // this process is reading from the output of the first process
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
                close(pipefds[0]);
                close(pipefds[1]);
                execvp(outfile, outputargv);
                exit(EXIT_FAILURE);
            }
            else
            { // Parent process
                int status1;
                int status2;
                child_running = 1;
                close(pipefds[0]);
                close(pipefds[1]); // closing pipes and waiting for children to finish
                waitpid(pid1, &status1, WUNTRACED);
                if (WIFSTOPPED(status1))
                {
                    cout << "Stopped" << endl;
                }
                waitpid(pid2, &status2, WUNTRACED);
                if (WIFSTOPPED(status2))
                {
                    cout << "Stopped" << endl;
                }
                child_running = 0;
            }
        }
    }
    else
    {
        int pipefds[2];
        // this is the technique so two processes can communicate
        if (pipe(pipefds) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid1 = fork(); // forking for the first process
        if (pid1 == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid1 == 0)
        { // Child process 1
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            dup2(pipefds[1], STDOUT_FILENO); // this process should write
            close(pipefds[0]);
            close(pipefds[1]);
            execvp(infile, inputargv);
            exit(EXIT_FAILURE);
        }
        else
        {                  // Parent process
            pid2 = fork(); // forking the other process
            if (pid2 == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid2 == 0)
            { // Child process 2
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                dup2(pipefds[0], STDIN_FILENO); // this process is reading from the output of the first process
                close(pipefds[0]);
                close(pipefds[1]);
                execvp(outfile, outputargv);
                exit(EXIT_FAILURE);
            }
            else
            { // Parent process
                int status1, status2;
                child_running = 1;
                close(pipefds[0]);
                close(pipefds[1]); // closing pipes and waiting for children to finish
                waitpid(pid1, &status1, WUNTRACED);
                if (WIFSTOPPED(status1))
                {
                    cout << "Stopped" << endl;
                }
                waitpid(pid2, &status2, WUNTRACED);
                if (WIFSTOPPED(status2))
                {
                    cout << "Stopped" << endl;
                }
                child_running = 0;
            }
        }
    }
}
void io_bg(int i, vector<char *> cstrings,vector<string> every_string)
{
    int count=0;
    for(int r=0;r<every_string.size();r++){
        if(every_string[r]=="&"){
            count++;
        }
        if(every_string[r]==";"){
            every_string.erase(every_string.begin()+r);
        }
    }                                  //finding how many processes to run in background in one line
                                        //also remove ;
    vector<char*> cstrings1=to_chararray(every_string); //convert string vector to char * vector,result should be something like 0[./count1],1[&],2[./count2],3[&]
    pid_t pids[count];
    pid_t pgid=getpid();
    setpgid(pgid, pgid);//create the process group
    for (int r = 0; r < count; r++)
    {
        int k = i - 1;
        char **inputargv;
        inputargv = find_left_arguments(i, k, cstrings1);
        char *infile = inputargv[0]; // if it is a file should be one word file1.txt myprogram etc
        pids[i] = fork();                // create new process
        int status;
        if (pids[i] == -1)
        {
            perror("fork");
            exit(1);
        }
        else if (pids[i] == 0)
        { // child process
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            setpgid(0, pgid); // set the process group ID to the child PID
            execvp(infile, inputargv);
        }
        else
        {
            setpgid(pids[i],pgid); //add child to process group
            
        }
        i++;
        i++;
        flag_bg=1;  //having this global variable to check if there are bg commands
    }
    for(int r=0;r<count;r++){
        int status;
        child_running = 1;
        waitpid(pid, &status, WNOHANG);
        if (WIFSTOPPED(status))
        {
            cout << "Stopped" << endl;
        }
        child_running = 0;
    }
}

void just_sh_command(int flag, vector<char *> cstrings)
{
    if (flag == 0) // if no redirections,piping,background commands exists then input may be just a shell command or an executable or a wrong command
    {
        for (int r = 0; r < cstrings.size(); r++)
        {
            if (strcmp(cstrings[r], "$PATH") == 0)
            {
                cstrings[r] = getenv("PATH");
            }
        }
        char **argv = (char **)malloc((cstrings.size()) + 1 * sizeof(char *));
        int counter = 0;
        for (int r = 0; r < cstrings.size(); r++)
        {
            argv[counter] = (char *)malloc((strlen(cstrings[r]) + 1) * sizeof(char));
            strcpy(argv[counter], cstrings[r]);
            counter++;
        }
        argv[counter] = NULL;
        int status;
        int exit_status = 0;
        pid = fork();
        if (pid == 0) // create new process
        {
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            if (strcmp(cstrings[0], "cd") == 0)
            {
                if (cstrings[1] == NULL)
                {
                    cstrings[1] = getenv("HOME"); // case command is cd
                }
                if (chdir(cstrings[1]) == 0)
                {
                    // implement cd
                }
                else
                {
                    cout << "cd: " << cstrings[1] << ": No such file or directory " << endl;
                }
            }
            else if (execvp(argv[0], argv) == -1 && strcmp(cstrings[0], "myHistory") != 0 && strcmp(cstrings[0], "") != 0) // if execvp failed and we dont have one of these strings
            {
                cout << cstrings[0] << ": Command not found" << endl;
            }
        }
        else if (pid > 0) // else execvp is executed,wait for child to finish
        {
            child_running = 1;
            waitpid(pid, &status, WUNTRACED);
            if (WIFSTOPPED(status))
            {
                cout << "Stopped" << endl;
            }
            child_running = 0;
        }
    }
}
char **find_left_arguments(int i, int k, vector<char *> cstrings)
{
    while (k >= 0 && strcmp("<", cstrings[k]) != 0 && strcmp(">", cstrings[k]) != 0 && strcmp(">>", cstrings[k]) != 0 && strcmp("|", cstrings[k]) != 0 && strcmp("&", cstrings[k]) != 0)
    {
        k--; // go to the begining of the string
    }
    k++;
    int initial = k;
    int counter = 0;
    char **inputargv = (char **)malloc((i - k) + 1 * sizeof(char *)); //+1 beacause we have the NULL at the end
    while (strcmp("<", cstrings[k]) != 0 && strcmp(">", cstrings[k]) != 0 && strcmp(">>", cstrings[k]) != 0 && strcmp("|", cstrings[k]) != 0 && strcmp("&", cstrings[k]) != 0)
    {
        inputargv[counter] = (char *)malloc((strlen(cstrings[k]) + 1) * sizeof(char)); // create char *[] ,use it for execvp second argument
        strcpy(inputargv[counter], cstrings[k]);
        counter++;
        k++;
    }
    inputargv[counter] = NULL; // adding NULL at the end
    return inputargv;
}
char **find_right_arguments(int i, int k, vector<char *> cstrings, int *temp)
{
    k = i + 1;
    *temp = i + 1;
    while (*temp < cstrings.size() && strcmp("<", cstrings[*temp]) != 0 && strcmp(">", cstrings[*temp]) != 0 && strcmp(">>", cstrings[*temp]) != 0 && strcmp("|", cstrings[*temp]) != 0 && strcmp("&", cstrings[k]) != 0)
    {
        *temp = *temp + 1; // go to the begining of the string
    }
    *temp = *temp - 1;
    int initial = k;
    int counter = 0;
    char **inputargv = (char **)malloc((*temp - i) + 1 * sizeof(char *)); //+1 beacause we have the NULL at the end
    while (strcmp("<", cstrings[k]) != 0 && strcmp(">", cstrings[k]) != 0 && strcmp(">>", cstrings[k]) != 0 && strcmp("|", cstrings[k]) != 0 && k <= *temp && strcmp("&", cstrings[k]) != 0)
    {
        inputargv[counter] = (char *)malloc((strlen(cstrings[k]) + 1) * sizeof(char));
        strcpy(inputargv[counter], cstrings[k]);
        counter++;
        k++;
        if (k > *temp)
        {
            break;
        }
    }
    inputargv[counter] = NULL;
    return inputargv;
}
