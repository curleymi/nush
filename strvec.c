/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#include "strvec.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

// allocates a string vec of size 0 and capacity 4
strvec* make_strvec() {
    strvec* svec = malloc(sizeof(strvec));
    svec->size = 0;
    svec->cap = DEFAULT_CAP;
    svec->data = malloc(svec->cap * sizeof(char*));
    return svec;
}

// not included in header, no use by external programs
// frees only the data strings for the given strvec
void free_strvec_data(strvec* svec) {
    for (int i = 0; i < svec->size; i++) {
       free(get_str(svec, i));
    }
}

// frees the strvec and its data
void free_strvec(strvec* svec) {
    free_strvec_data(svec);
    free(svec->data);
    free(svec);
}

// resets size of the strvec and frees the data,
// does not realloc the vector capacity
void clear_strvec_data(strvec* svec) {
    free_strvec_data(svec);
    svec->size = 0;
}

// gets the string at the given index, does not copy string
char* get_str(strvec* svec, int i) {
    assert(i >= 0 && i < svec->size);
    return svec->data[i];
}

// increases the size of the strvec (if needed), sets size 
// index to the given string
void push_str(strvec* svec, char* str) {
    if (svec->size == svec->cap) {
        svec->cap *= CAP_GROW_FACTOR;
        svec->data = realloc(svec->data, svec->cap * sizeof(char*));
    }

    svec->data[svec->size++] = str;
}
