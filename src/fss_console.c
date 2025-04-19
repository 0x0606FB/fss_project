//
// Created by redux on 4/19/25.
//

#include "../include/fss_console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define PIPE_IN "fss_in"
#define PIPE_OUT "fss_out"
#define MAX_CMD_LEN 1024

FILE *console_log = NULL;

void log_command(const char *line) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char ts[64];
    strftime(ts, sizeof(ts), "[%Y-%m-%d %H:%M:%S]", t);
    fprintf(console_log, "%s Command %s", ts, line);
    fflush(console_log);
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-l") != 0) {
        fprintf(stderr, "Usage: %s -l <console-logfile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *log_path = argv[2];
    console_log = fopen(log_path, "a");
    if (!console_log) {
        perror("log file open");
        exit(EXIT_FAILURE);
    }

    int fd_in = open(PIPE_IN, O_WRONLY);
    if (fd_in < 0) {
        perror("open fss_in (write)");
        exit(EXIT_FAILURE);
    }

    int fd_out = open(PIPE_OUT, O_RDONLY);
    if (fd_out < 0) {
        perror("open fss_out (read)");
        exit(EXIT_FAILURE);
    }

    char line[MAX_CMD_LEN];
    printf("FSS Console ready. Type commands:\n");

    while (fgets(line, sizeof(line), stdin)) {
        write(fd_in, line, strlen(line));
        log_command(line);

        // Read response
        char response[MAX_CMD_LEN];
        ssize_t n = read(fd_out, response, sizeof(response) - 1);
        if (n > 0) {
            response[n] = '\0';
            printf("%s", response);
            fprintf(console_log, "%s", response);
        }

        if (strncmp(line, "shutdown", 8) == 0)
            break;
    }

    close(fd_in);
    close(fd_out);
    fclose(console_log);
    return 0;
}