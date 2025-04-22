// File: src/worker.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define BUF_SIZE 4096

// Copy a single file from src_path to tgt_path using low-level syscalls
void copy_file(const char *src_path, const char *tgt_path) {
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        dprintf(STDOUT_FILENO, "ERROR: Failed to open %s: %s\n", src_path, strerror(errno));
        return;
    }

    int tgt_fd = open(tgt_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (tgt_fd < 0) {
        dprintf(STDOUT_FILENO, "ERROR: Failed to create %s: %s\n", tgt_path, strerror(errno));
        close(src_fd);
        return;
    }

    char buffer[BUF_SIZE];
    ssize_t bytes;
    while ((bytes = read(src_fd, buffer, BUF_SIZE)) > 0) {
        write(tgt_fd, buffer, bytes);
    }

    close(src_fd);
    close(tgt_fd);
    dprintf(STDOUT_FILENO, "Copied %s -> %s\n", src_path, tgt_path);
}

// Perform a full sync: copy all regular files from src_dir to tgt_dir
void full_sync(const char *src_dir, const char *tgt_dir) {
    DIR *src = opendir(src_dir);
    if (!src) {
        dprintf(STDOUT_FILENO, "ERROR: Failed to open source dir %s: %s\n", src_dir, strerror(errno));
        return;
    }

    mkdir(tgt_dir, 0755); // Make sure target exists

    struct dirent *entry;
    while ((entry = readdir(src)) != NULL) {
        if (entry->d_type != DT_REG) continue; // Skip non-regular files
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char src_path[512], tgt_path[512];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
        snprintf(tgt_path, sizeof(tgt_path), "%s/%s", tgt_dir, entry->d_name);

        copy_file(src_path, tgt_path);
    }

    closedir(src);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        dprintf(STDOUT_FILENO, "Usage: %s <source> <target> <filename> <operation>\n", argv[0]);
        return 1;
    }

    const char *src = argv[1];
    const char *tgt = argv[2];
    const char *filename = argv[3];  // currently unused for FULL
    const char *operation = argv[4];

    if (strcmp(operation, "FULL") == 0) {
        full_sync(src, tgt);
    } else {
        dprintf(STDOUT_FILENO, "Unsupported operation: %s\n", operation);
        return 1;
    }

    return 0;
}
