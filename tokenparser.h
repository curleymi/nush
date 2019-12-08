/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include "strvec.h"

// adds each commandline token to the given string vector
// only accounts for operators: < > | & || && ; () ""
int parse_tokens(strvec* svec, char* command);

// removes newline chars preceeded by a backslash char
void remove_ignored_newlines(char* command);

#endif
