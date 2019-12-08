/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#ifndef NUSHEXEC_H
#define NUSHEXEC_H

#include "strvec.h"

// a general procedure can be almost any combination of the following
// restriction: OnTrue or OnFalse MUST be bitwise anded with Wait
typedef enum enum_procedure {
    // all base 2 numbers, procedures can be anded to create more complex procedures
    Wait = 1,
    OnTrue = 2,
    OnFalse = 4,
    Next = 8,
    Base = 16
} enum_procedure;

// executes the strvec between the given indexes, start inclusive end exclusive
int execute_recursive_nush(strvec* svec, int start, int end);

// executes a full strvec from 0 to svec->size
int execute_nush(strvec* svec);

#endif
