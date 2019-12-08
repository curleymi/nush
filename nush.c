/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#include "strvec.h"
#include "tokenparser.h"
#include "scriptparser.h"
#include "nushexec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

// getconf ARG_MAX: 0x200000, reduced by a factor of 16 (don't need that much memory for this program)
#define MAX_COMMAND_LEN 0x20000

// looked up from unix.stackexchange, the max length of a directory path
#define MAX_PWD_LEN 0x1000

// the colors to print to the console
#define BLUE_CONSOLE_TEXT "\x1B[36m"
#define NORM_CONSOLE_TEXT "\x1B[0m"

// processes the given user input, command must be null terminated
int process_command(strvec* svec, char* command) {
    int retcode = 1;
    
    // if parsing returns an error do not execute command
    if (!parse_tokens(svec, command)) {
        retcode = execute_nush(svec);
    }

    return retcode;
}

// given a strvec and a filename, executes the entirity of the script
int process_script(strvec* svec, char* file) {
    int currentLen = 0;
    int commandLen;
    int nextLen;
    char* command = parse_script(file);
    int retcode = 0;

    // only process the command if it was parsed correctly
    if (command) {
        // process commands until the end of the file is encountered
        commandLen = strlen(command);
        while (currentLen < commandLen) {
            // commands to be processed must be null terminated
            // end of a command from a script is a new line, replace them with null chars
            nextLen = length_to_newln(command + currentLen);
            command[currentLen + nextLen - 1] = 0;
            retcode = process_command(svec, command + currentLen);
            currentLen += nextLen;
        }

        // free the allocated command string
        free(command);
    }
    else {
        retcode = 1;
    }

    return retcode;
}

// nush can be called with one script file argument
// OR
// nush can be called with variable number of commands with -N flag
//  -1 indicates 1 command:  ./nush -1 "echo hello"
//  -2 indicates 2 commands: ./nush -2 "echo hello" "echo goodbye || echo no print"
//  etc...
int main(int argc, char* argv[]) {
    // create the vector for the tokens, init return code to zero
    strvec* svec = make_strvec();
    int retcode = 0;

    // if given a script execute only that
    if (argc == 2) {
        retcode = process_script(svec, argv[1]);
    }
    // if flag for executing a variable number of commands, indicated by first argument as "-N"
    else if (argc > 2 && argv[1][0] == '-') {
        // if the given flag number equals the number of arguments given, execute them
        if (atoi(argv[1] + 1) != argc - 2) {
            printf("Error: Variable command flag format\n\t-N \"commmand0\" \"command1\" ... \"commandN\"\n");
            retcode = 1;
        }
        else {
            // execute each command in the arguments
            for (int i = 2; i < argc; i++) {
                retcode = process_command(svec, argv[i]);
            }
        }
    }
    // no special instructions, execute normal shell operation
    else {
        struct passwd *pw;
        int uwdlen;
        int pwdlen;
        char command[MAX_COMMAND_LEN];
        char pwd[MAX_PWD_LEN];
        int offset = 0;
        
        // loop forever
        for (;;) {
            // print the variable prompt
            if (offset) {
                // print prompt for a continued command
                printf("> ");
            }
            else {
                // print the pwd in blue if no error getting pwd, then the nush prompt normally 
                if (getcwd(pwd, MAX_PWD_LEN) && (pw = getpwuid(getuid()))) {
                    // set the length values for conditionally printing the pwd
                    uwdlen = strlen(pw->pw_dir);
                    pwdlen = strlen(pwd);
                    
                    // replace user working directory with "~" if length permits
                    printf("%s%s%s ", BLUE_CONSOLE_TEXT, pwdlen >= uwdlen ? "~" : "", pwd + (pwdlen >= uwdlen ? uwdlen : 0));
                }
                printf("%snush$ ", NORM_CONSOLE_TEXT);
            }

            // if the input is end of file break the loop
            if (!fgets(command + offset, MAX_COMMAND_LEN - offset, stdin)) {
                break;
            }
            
            // check if command is terminated by newline, if the command ends
            // with a backslash then newline keep reading
            offset = strlen(command);
            if (command[offset - 2] == '\\' && command[offset - 1] != 'n') {
                offset -= 2;
                continue;
            }

            // command is terminated by newline
            offset = 0;

            // process the input
            retcode = process_command(svec, command);
        }

        // print new line after eof
        printf("\n");
    }

    // free the tokens
    free_strvec(svec);

    return retcode;
}
