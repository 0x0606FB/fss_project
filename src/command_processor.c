#include "../include/sync_manager.h"
#include "../include/command_processor.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

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

        char src[MAX_PATH_LEN], tgt[MAX_PATH_LEN];

        if (strncmp(buffer, "shutdown", 8) == 0) {
            printf("[manager] Shutdown command received.\n");
            break;
        }
        else if (strncmp(buffer, "add", 3) == 0) {
            if (sscanf(buffer, "add %s %s", src, tgt) == 2) {
                add_command(src, tgt, fd_out);
            } else {
                dprintf(fd_out, "[manager] Invalid add command format.\n");
            }
        }
        else if (strncmp(buffer, "cancel", 6) == 0) {
            if (sscanf(buffer, "cancel %s %s", src, tgt) == 2) {
                cancel_command(src, tgt, fd_out);
            } else {
                dprintf(fd_out, "[manager] Invalid cancel command format.\n");
            }
        }
        else if (strncmp(buffer, "sync", 4) == 0) {
            if (sscanf(buffer, "sync %s %s", src, tgt) == 2) {
                sync_command(src, tgt, fd_out);
            } else {
                dprintf(fd_out, "[manager] Invalid sync command format.\n");
            }
        }
        else if (strncmp(buffer, "status", 6) == 0) {
            status_command(fd_out);
        }
        else {
            dprintf(fd_out, "[manager] Unknown command: %s", buffer);
        }
    }

    close(fd_in);
    close(fd_out);
}


void add_command(const char *src, const char *tgt, int fd_out) {

    time_t now = time(NULL);
    char ts[64];
    strftime(ts, sizeof(ts), "[%Y-%m-%d %H:%M:%S]", localtime(&now));

    if (!add_sync_pair(src, tgt)) {
        dprintf(fd_out, "%s Already in queue: %s\n", ts, src);
        return;
    }

    dprintf(fd_out, "%s Added directory: %s -> %s\n", ts, src, tgt);
    dprintf(fd_out, "%s Monitoring started for %s\n", ts, src);
}

void cancel_command(const char *src, const char *tgt, int fd_out){
  }

void status_command(int fd_out){
  }

void sync_command(const char *src, const char *tgt, int fd_out){
  }