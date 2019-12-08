/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#ifndef NUSHINTERPRETER_H
#define NUSHINTERPRETER_H

#include "strvec.h"

// returns if the vector starts with an exit command
int exit_requested(strvec* svec, int start);

// returns if the vector starts with a change directory command
int change_directory(strvec* svec, int start);

// returns if the vector starts with a subshell command
int subshell(strvec* svec, int start);

// returns the index in the vector of a redirection operator,
// sets which operator in the retop pointer
int redirect_idx(strvec* svec, int start, int end, char* retop);

// returns the index of the vector of a pipe operator
int pipe_idx(strvec* svec, int start, int end);

// returns the index of the vector of a boolean operator,
// sets which operator in the retop pointer
int boolean_idx(strvec* svec, int start, int end, char* retop);

// returns the index of the vector of a termination operator,
// sets which operator in the retop pointer
int terminator_idx(strvec* svec, int start, int end, char* retop);

#endif
