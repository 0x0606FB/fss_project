#include "../include/command_processor.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define PIPE_IN "fss_in"
#define PIPE_OUT "fss_out"
#define MAX_CMD_LEN 1024

void listen_for_command(void) {

    int fd_in, fd_out;
    char buffer[MAX_CMD_LEN];

    fd_in = open(PIPE_IN, O_RDONLY);
    if (fd_in < 0) error_exit("open fss_in");


    fd_out = open(PIPE_OUT, O_WRONLY);
    if (fd_out < 0) error_exit("open fss_out");

    printf("[init] Listening for commands from console...\n");

    while (1) {
        ssize_t n = read(fd_in, buffer, sizeof(buffer) - 1);
        if (n <= 0) continue;

        buffer[n] = '\0';
        printf("[console cmd] %s\n", buffer);

        // Echo response for now
        dprintf(fd_out, "[manager] Received: %s", buffer);

        if (strncmp(buffer, "shutdown", 8) == 0) {
            printf("[manager] Shutdown command received.\n");
            break;
        }
    }

    close(fd_in);
    close(fd_out);
}