
// File: src/fss_manager.c
#include "../include/sync_manager.h"
#include "../include/utils.h"
#include "../include/command_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define PIPE_IN "fss_in"
#define PIPE_OUT "fss_out"

#define MAX_CMD_LEN 1024

int worker_limit = 5;
char *logfile_path = NULL;
char *config_file_path = NULL;


int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: -l <logfile> -c <config_file> [-n <worker_limit>]\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
            logfile_path = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            config_file_path = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            worker_limit = atoi(argv[++i]);
        } else {
            fprintf(stderr, "Usage: -l <logfile> -c <config_file> [-n <worker_limit>]\n");
            exit(EXIT_FAILURE);
        }
    }

    if (!logfile_path || !config_file_path) {
        fprintf(stderr, "Usage: -l <logfile> -c <config_file> [-n <worker_limit>]\n");
        exit(EXIT_FAILURE);
    }

    printf("Logfile: %s\n", logfile_path);
    printf("Config: %s\n", config_file_path);
    printf("Worker Limit: %d\n", worker_limit);

    parse_config_file(config_file_path);
    pipes_init();
    listen_for_command();

    return 0;
}