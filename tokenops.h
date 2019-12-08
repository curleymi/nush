/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#ifndef TOKENOPS_H
#define TOKENOPS_H

// a string has leading whitespace if first char is a space, tab or new line
int leading_whitespace(const char* command);

// a string is a new line if it starts with a \n
int new_line(const char* command);

// a null string starts with character 0
int null_str(const char* command);

// a string starts with an open paren
int open_paren(const char* command);

// a string starts with a close paren
int close_paren(const char* command);

// a string starts with a double quote
int doub_quot_str(const char* command);

// a redirect in token is a single_op "<"
int parse_op_token_redir_in(const char* command);

// a redirect out token is a single_op ">"
int parse_op_token_redir_out(const char* command);

// a pipe token is a single_op "|"
int parse_op_token_pipe(const char* command);

// a background token is a single_op "&"
int parse_op_token_background(const char* command);

// an or token is a double_op "||"
int parse_op_token_or(const char* command);

// an and token is a double_op "&&"
int parse_op_token_and(const char* command);

// a semi colon token is a single_op ";"
int parse_op_token_semic(const char* command);

// returns if the start of the given string is a single char operator
int single_char_op_token(const char* command);

// returns if the start of the given string is a double char operator
int double_char_op_token(const char* command);

// returns the length of the given string until the next whitespace,
// null or operator char(s)
int parse_non_op_token_len(const char* command);

// returns the length of the given string from the first open paren to its
// corresponding closing paren
int parse_paren_len(const char* command);

// returns the length of the given string from the first open quote to its
// corresponding closing quote
int parse_doub_quot_len(const char* command);

#endif
