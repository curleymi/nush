/*
 * Michael Curley
 * Challenge 01
 * 2/16/19
 */

#include "nushexec.h"
#include "tokenparser.h"
#include "nushinterpreter.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// if the command is true or false, free the vector and exit with the
// corresponding code
void check_boolean(strvec* svec, int start) {
    if (!strcmp(get_str(svec, start), "true")) {
        free_strvec(svec);
        exit(0);
    }
    if (!strcmp(get_str(svec, start), "false")) {
        free_strvec(svec);
        exit(1);
    }
}

// executes the given command and kills current process,
// only call from child processes
void execute_strvec(strvec* svec, int start, int end) {
    // can only execute a command of length 1 or more
    if (end - start > 0) {
        // copy the args to a NULL terminating array
        int len = end - start;

        // if the command is only one arg check if it is a special keyword
        if (len == 1) {
            check_boolean(svec, start);
        }

        // copy the args to a null terminating array
        char** args = malloc((len + 1) * sizeof(char*));
        for (int i = 0; i < len; i++) {
            args[i] = strdup(get_str(svec, start + i));
        }
        args[len] = 0;

        // free the strvec
        free_strvec(svec);

        // execute the command
        execvp(args[0], args);

        // free the args array on error
        for (int i = 0; i < len + 1; i++) {
            free(args[i]);
        }
    }

    // exit with error code 1
    exit(1);
}

int execute_subshell(char* command) {
    // a subshell must begin and end with parens
    assert(command[0] == '(' && command[strlen(command) - 1] == ')');
    
    int cpid;
    int retcode = 0;

    // remove open paren and overwrite close paren to null char
    (++command)[strlen(command) - 1] = 0;
    
    // execute variable arg subshell in child process
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        retcode = WEXITSTATUS(status);
    }
    else {
        // a subshell only contains 1 command, so "-1" flag can be hardcoded
        execlp("./nush", "./nush", "-1", command, NULL);

        // on error exit with error code 1
        exit(1);
    }

    return retcode;
}

int execute_redirection(strvec* svec, int start, int idx, int end, int stdfd) {
    int cpid;
    int retcode = 0;

    // open the file for reading or writing based on the given std file descriptor
    int fd = stdfd ?
        open(get_str(svec, idx + 1), O_WRONLY | O_TRUNC | O_CREAT, 0644) :
        open(get_str(svec, idx + 1), O_RDONLY);  
    
    // if the file failed to open print message and return 1
    if (fd < 0) {
        printf("nush:\t%s\tError opening file.\n", get_str(svec, idx + 1));
        retcode = 1;
    }
    // if the file opened correctly, fork and execute the remaining commands
    else if ((cpid = fork())) {
        int status;

        // close file descriptor and wait for command to execute
        close(fd);
        waitpid(cpid, &status, 0);
        retcode = WEXITSTATUS(status);
    }
    else {
        // change the stdin or stdout to the opened file, close the duplicate stream
        dup2(fd, stdfd);
        close(fd);

        // execute the command
        execute_strvec(svec, start, idx);
    }
    return retcode;
}

int execute_pipe(strvec* svec, int start, int idx, int end) {
    int cpid;
    int retcode = 0;

    // execute the pipe in a child process, so stdin and out of
    // parent process are not corrupted
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        retcode = WEXITSTATUS(status);
    }
    else {
        int ccpid;
        int fds[2];

        // create the pipe
        pipe(fds);

        if ((ccpid = fork())) {
            int status;
            int lretcode;

            // modify std in to be the read end of pipe
            dup2(fds[0], 0);
            close(fds[0]);
            close(fds[1]);
           
            // execute the right side of pipe command concurrently with the left
            retcode = execute_recursive_nush(svec, idx + 1, end);
           
            // wait for the left side of pipe command to exit
            waitpid(ccpid, &status, 0);
            
            // set the left command exit code
            lretcode = WEXITSTATUS(status);

            // if the left side command exited on error return that code, otherwise return
            // the right command exit code
            exit(lretcode ? lretcode : retcode);
        }
        else {
            // modify std out to be the write end of the pipe
            dup2(fds[1], 1);

            // close the unused file descriptors
            close(fds[0]);
            close(fds[1]);

            // execute the command, could contain an operator so execute recursively
            exit(execute_recursive_nush(svec, start, idx));
        }
    }
    return retcode;
}

int execute_general(strvec* svec, int start, int idx, int end, enum_procedure proc) {
    // procedure cannot be 0
    assert(proc);
    
    // coded with waiting for true or false but not waiting for return code to be set is a bug
    assert((proc & OnTrue) == OnTrue || (proc & OnFalse) == OnFalse ? (proc & Wait) == Wait : 1);

    int retcode = 0;

    // execute_recursive does waitpid for general execution, so normal execution is actually waiting
    if ((proc & Wait) == Wait) {
        retcode = execute_recursive_nush(svec, start, idx);
    }
    // execute in background, dont monitor background commands
    else if (!fork()) {
        // exit to ensure no stray background processes
        exit(execute_recursive_nush(svec, start, idx));
    }

    // execute the next command if specified
    if ((proc & Next) == Next) {
        // only proceed on true or false and return code is correct value
        // if not proceed on true or false set just proceed regardless of return code
        if ((proc & OnTrue) == OnTrue && !retcode ||
            (proc & OnFalse) == OnFalse && retcode ||
            !((proc & OnTrue) | (proc & OnFalse))) {
            retcode = execute_recursive_nush(svec, idx + 1, end);
        }    
    }

    return retcode;
}

int execute_base(strvec* svec, int start, int end) {
    int cpid;
    int retcode = 0;

    // if command is to exit, free the vector and exit nush
    if (exit_requested(svec, start)) {
        free_strvec(svec);
        exit(0);
    }
    // if its a cd command, change the pwd in the parent
    else if (change_directory(svec, start)) {
        // change to the specified directory, on error print message
        if ((retcode = chdir(get_str(svec, start + 1)))) {
            printf("nush:\t%s:\tNo such directory.\n", get_str(svec, start + 1));
        }
    }
    // if its a subshell command, execute the subshell
    else if (subshell(svec, start)) {
        retcode = execute_subshell(get_str(svec, start));
    }
    // if not any special cases, execute the command in child process, return its exit code
    else if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        retcode = WEXITSTATUS(status);
    }
    else {
        // execute the command
        execute_strvec(svec, start, end);
    }

    return retcode;
}

int execute_recursive_nush(strvec* svec, int start, int end) {
    // recursive base case, if the length of command to run is zero the full command has been executed
    if (end - start > 0) {
        // any valid index will be greater than or equal to start index
        int idx;
        char singleop[2];
        char doubleop[3];
        singleop[1] = 0;
        doubleop[2] = 0;

        // piping takes highest precedence, executes recursively both sides of the pipe
        if ((idx = pipe_idx(svec, start, end)) >= start) {
            return execute_pipe(svec, start, idx, end);
        }
        // redirection takes next highest precedence, executes recursively the right side of the redirect
        else if ((idx = redirect_idx(svec, start, end, singleop)) >= start) { 
            return execute_redirection(svec, start, idx, end, strcmp(singleop, "<") ? 1 : 0);
        }
        // termination (; and &) takes next highest precedence, recursively executes the right side of command
        else if ((idx = terminator_idx(svec, start, end, singleop)) >= start) {
            return execute_general(svec, start, idx, end, Next | (strcmp(singleop, "&") ? Wait : 0));
        }
        // booleans take next highest precedence, recursively executes the right side of command conditionally
        else if ((idx = boolean_idx(svec, start, end, doubleop)) >= start) {
            return execute_general(svec, start, idx, end,  Wait | Next | (strcmp(doubleop, "&&") ? OnFalse : OnTrue));
        }
        // no operators, execute the last command, no more recursion needed
        else {
            return execute_base(svec, start, end);
        }
    }

    return 0;
}

// general entry point for a strvec, calls the recursive execution
int execute_nush(strvec* svec) {
    return execute_recursive_nush(svec, 0, svec->size);
}
