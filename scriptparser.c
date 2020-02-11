/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#include "scriptparser.h"
#include "tokenparser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// parses a file into a newly allocated null terminating string
char* parse_script(char* fileName) {
    // create the pointer for the string and set the total length of the file to 0
    char* file = 0;
    int fd;
    int len;
    int totalLen = 0;

    // open the file and begin reading if no error
    if ((fd = open(fileName, O_RDONLY)) > 0) {
        // allocate the file buffer
        char fileBlock[4000];
        do {
            // try to read 4000 char
            len = read(fd, fileBlock, 4000);

            // reallocate the memory (or malloc initial if null) of the real file for the
            // new data + 1 for the null terminator
            file = file
               ? realloc(file, (totalLen + len + 1) * sizeof(char))
               : malloc((len + 1) * sizeof(char));
        
            // append the newly read chars to the existing file
            memcpy(file + totalLen, fileBlock, len);

            // increment the total length of the file
            totalLen += len;
        }
        while (len);

        // close the file
        close(fd);

        // null terminate the string
        file[totalLen] = 0;

        // process special characters like newlines and '\\'
        process_special_characters(file);
    }
    else {
        printf("error opening %s\n", fileName);
    }

    return file; // on error file is a null pointer
}

// returns the length of the string to the next newline char
// if string terminates before new line that length is returned
int length_to_newln(char* script) {
    int len = 0;
    while (script[len] && script[len] != '\n') {
        len++;
    }
    return len + 1;
}
