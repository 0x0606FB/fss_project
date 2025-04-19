#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define PIPE_IN "fss_in"
#define PIPE_OUT "fss_out"

void error_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void pipes_init(void) {
    // Clean up any existing pipes
    unlink(PIPE_IN);
    unlink(PIPE_OUT);

    if (mkfifo(PIPE_IN, 0666) == -1 && errno != EEXIST) {
        error_exit("mkfifo fss_in");
    }

    if (mkfifo(PIPE_OUT, 0666) == -1 && errno != EEXIST) {
        error_exit("mkfifo fss_out");
    }

    printf("[init] Named pipes created: %s, %s\n", PIPE_IN, PIPE_OUT);
}
