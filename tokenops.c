/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#include "tokenops.h"

// a single operator is the given char followed 
// by a char that is not the same char
int single_op(const char* command, char c) {
    return command[0] == c && command[1] != c;
}

// a double operator is the given char twice
// followed by a char that is not the same char
int double_op(const char* command, char c) {
    return command[0] == c && command[1] == c;
}

// a string has leading whitespace if first char is a space, tab or new line
int leading_whitespace(const char* command) {
    return command[0] == ' ' || command[0] == '\t' || command[0] == '\n';
}

// a string is a new line if it starts with \n
int new_line(const char* command) {
    return command[0] == '\n';
}

// a null string starts with character 0
int null_str(const char* command) {
    return command[0] == 0;
}

// returns if the first character is an open paren
int open_paren(const char* command) {
    return command[0] == '(';
}

// returns if the first character is a closing paren
int close_paren(const char* command) {
    return command[0] == ')';
}

// returns if the first character is a "
int doub_quot_str(const char* command) {
    return command[0] == '\"';
}

// a redirect in token is a single_op "<"
int parse_op_token_redir_in(const char* command) {
    return single_op(command, '<');
}

// a redirect out token is a single_op ">"
int parse_op_token_redir_out(const char* command) {
    return single_op(command, '>');
}

// a pipe token is a single_op "|"
int parse_op_token_pipe(const char* command) {
    return single_op(command, '|');
}

// a background token is a single_op "&"
int parse_op_token_background(const char* command) {
    return single_op(command, '&');
}

// an or token is a double_op "||"
int parse_op_token_or(const char* command) {
    return double_op(command, '|');
}

// an and token is a double_op "&&"
int parse_op_token_and(const char* command) {
    return double_op(command, '&');
}

// a semi colon token is a single_op ";"
int parse_op_token_semic(const char* command) {
    return single_op(command, ';');
}

// returns if the start of the given string is a single char operator
int single_char_op_token(const char* command) {
    return parse_op_token_redir_in(command) ||
        parse_op_token_redir_out(command) ||
        parse_op_token_pipe(command) ||
        parse_op_token_background(command) ||
        parse_op_token_semic(command);
}

// returns if the start of the given string is a double char operator
int double_char_op_token(const char* command) {
    return parse_op_token_or(command) ||
        parse_op_token_and(command);
}

// returns the length of the given string until the next whitespace,
// null or operator char(s)
int parse_non_op_token_len(const char* command) {
    // keep incrementing length until end of command, whitespace,
    // single char operator or double char operator
    int len = 0;
    while (!null_str(command + len) &&
            !leading_whitespace(command + len) &&
            !single_char_op_token(command + len) &&
            !double_char_op_token(command + len)) {
        len++;
    }

    return len;
}

// returns the length of the string until a closing paren is found
// if null str or new line is found before close returns idx -1
int parse_paren_len(const char* command) {
    if (!open_paren(command)) {
        return -1;
    }

    int len = 1;
    int innerlen;
    while (!close_paren(command + len)) {
        // no closing paren found
        if (null_str(command + len)) {
            return -1;
        }
        // if another open paren is found, skip over contents
        if (open_paren(command + len)) {
            if ((innerlen = parse_paren_len(command + len)) == -1) {
                return -1;
            }
            len += innerlen;
        }
        // if an open quote is found, skip over contents
        else if (doub_quot_str(command + len)) {
            if ((innerlen = parse_doub_quot_len(command + len)) == -1) {
                return -1;
            }
            len += innerlen;
        }
        // keep looking for closing paren
        else {
            len++;
        }
    }
    return len + 1;
}

// returns the length of the string to the corresponding closing double quote
int parse_doub_quot_len(const char* command) {
    if (!doub_quot_str(command)) {
        return -1;
    }

    int len = 1;
    while (!doub_quot_str(command + len)) {
        // no closing quote found
        if (null_str(command + len++)) {
            return -1;
        }
    }
    return len + 1;
}
