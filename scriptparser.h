/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

// parses a given file into one single string
char* parse_script(char* fileName);

// returns the length of a full string script to the next newline char
int length_to_newln(char* script);

#endif
