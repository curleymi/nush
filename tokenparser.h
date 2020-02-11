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

// processes special characters
void process_special_characters(char* command);

#endif
