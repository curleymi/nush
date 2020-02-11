/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#include "strvec.h"
#include "tokenops.h"
#include "tokenparser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// not included in header, don't let external programs use function
// pushes the given string of specified len from the buff to the token vec
void push_command_token(strvec* svec, const char* command, int len) {
    if (len) {
        char* token = malloc((len + 1) * sizeof(char));
        memcpy(token, command, len);
        token[len] = 0;
        push_str(svec, token);
    }
}

// shifts the characters back in the command by the given length
void shift_back(char* command, int len) {
    int i = len;
    while (command[i]) {
        command[i - len] = command[i];
        i++;
    }
    command[i - len] = 0;
}

// new lines will be ignored if preceeded by a backslash, necessary
// if user intends newline as white space and not a termination case
void process_special_characters(char* command) {
    // iterate over all characters
    while (*command) {
        if (*command == '\\') {
            switch (*(command + 1)) {
                case '\\':
                    shift_back(command, 1);
                    break;
                case '\n':
                    shift_back(command, 2);
                    break;
            }
        }
        command++;
    }
}

// adds each commandline token to the given string vector
// only accounts for operators: < > | & || && ; () ""
// command must be null terminated, new lines are treated as white space
int parse_tokens(strvec* svec, char* command) {
    // clear the current list of tokens
    clear_strvec_data(svec);
    int token_len = 0;
   
    // loop until end of command
    while (!null_str(command)) {
        // clear leading white space
        if (leading_whitespace(command)) {
            command++;
        }
        // if the next token is a double quote get its entire contents as one token
        else if (doub_quot_str(command)) {
            if ((token_len = parse_doub_quot_len(command)) == -1) {
                printf("Error: Expecting Closing Double Quote.\n");
                return 1;
            }
            push_command_token(svec, command + 1, token_len - 2);
            command += token_len;
        }
        // if the next token is a single char operator push it
        else if (single_char_op_token(command)) {
            push_command_token(svec, command, 1);
            command++;
        }
        // if the next token is a double char operator push it
        else if (double_char_op_token(command)) {
            push_command_token(svec, command, 2);
            command += 2;
        }
        // if the next token is the beginning of a subshell command parse its entirity to one token
        else if (open_paren(command)) {
            if ((token_len = parse_paren_len(command)) == -1) {
                printf("Error: Expected Closing Paren in Command.\n");
                return 1;
            }
            push_command_token(svec, command, token_len);
            command += token_len;
        }
        // next char is not an operator or sub shell command, parse its variable size and push it
        else {
            token_len = parse_non_op_token_len(command);
            push_command_token(svec, command, token_len);
            command += token_len;
        }
    }

    return 0;
}
