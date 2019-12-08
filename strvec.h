/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#ifndef STRVEC_H
#define STRVEC_H

// the default capacity of a string vector
#define DEFAULT_CAP 4

// the factor at which the string vector grows by when more mem is needed
#define CAP_GROW_FACTOR 2

// a string vector has a size, capacity and string array of data
typedef struct strvec {
    int size;
    int cap;
    char** data;
} strvec;

// allocates a string vec of size 0 and capacity 4
strvec* make_strvec();

// frees the strvec and its data
void free_strvec(strvec* svec);

// resets size of the strvec and frees the data,
// does not realloc the vector capacity
void clear_strvec_data(strvec* svec);

// gets the string at the given index, does not copy string
char* get_str(strvec* svec, int i);

// increases the size of the strvec (if needed), sets size 
// index to the given string
void push_str(strvec* svec, char* str);

#endif
