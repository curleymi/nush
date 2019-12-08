/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#include "nushinterpreter.h"
#include "strvec.h"

#include <string.h>

// returns if the vector starts with an exit command
int exit_requested(strvec* svec, int start) {
    return !strcmp(get_str(svec, start), "exit");
}

// returns if the vector starts with a change directory command
int change_directory(strvec* svec, int start) {
    return !strcmp(get_str(svec, start), "cd");
}

// returns if the vector starts with a subshell command
int subshell(strvec* svec, int start) {
    return get_str(svec, start)[0] == '(' && get_str(svec, start)[strlen(get_str(svec, start)) - 1] == ')';
}

// returns the index in the vector of a redirection operator,
// sets which operator in the retop pointer
int redirect_idx(strvec* svec, int start, int end, char* retop) {
    char* redirin = "<";
    char* redirout = ">";

    for (int i = start; i < end; i++) {
        if (!strcmp(get_str(svec, i), redirin)) {
            retop[0] = redirin[0];
            return i;
        }
        if (!strcmp(get_str(svec, i), redirout)) {
            retop[0] = redirout[0];
            return i;
        }
    }

    return -1;
}

// returns the index of the vector of a pipe operator
int pipe_idx(strvec* svec, int start, int end) {
    char* pipe = "|";

    for (int i = start; i < end; i++) {
        if (!strcmp(get_str(svec, i), pipe)) {
            return i;
        }
    }

    return -1;
}

// returns the index of the vector of a boolean operator,
// sets which operator in the retop pointer
int boolean_idx(strvec* svec, int start, int end, char* retop) {
    char* and = "&&";
    char* or = "||";    

    for (int i = start; i < end; i++) {
        if (!strcmp(get_str(svec, i), and)) {
            retop[0] = and[0];
            retop[1] = and[1];
            return i;
        }
        if (!strcmp(get_str(svec, i), or)) {
            retop[0] = or[0];
            retop[1] = or[1];
            return i;
        }
    }

    return -1;
}

// returns the index of the vector of a termination operator,
// sets which operator in the retop pointer
int terminator_idx(strvec* svec, int start, int end, char* retop) {
    char* bkg = "&";
    char* semi = ";";

    for (int i = start; i < end; i++) {
        if (!strcmp(get_str(svec, i), bkg)) {
            retop[0] = bkg[0];
            return i;
        }
        if (!strcmp(get_str(svec, i), semi)) {
            retop[0] = semi[0];
            return i;
        }
    }

    return -1;
}
