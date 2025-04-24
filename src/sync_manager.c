// File: src/sync_manager.c
#include "../include/sync_manager.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>      // for read, fork, execl
#include <sys/types.h>   // for pid_t

#define MAX_WATCHES 128
#define EVENT_BUF_LEN (1024 * (sizeof(struct inotify_event) + 24))

sync_info_t *head = NULL;

pthread_t inotify_fd;
pthread_t inotify_thread;

void parse_config_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) error_exit("[init] Can't open file");

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char src[MAX_PATH_LEN], tgt[MAX_PATH_LEN];
        if (sscanf(line, "%s %s", src, tgt) == 2) {
            sync_info_t *node = malloc(sizeof(sync_info_t));
            strncpy(node->source, src, MAX_PATH_LEN);
            strncpy(node->target, tgt, MAX_PATH_LEN);
            node->active = 1;
            node->next = head;
            head = node;
        }
    }
    fclose(file);
}

void sync_pair_dump(void) {
    printf("Loaded sync pairs:\n");
    for (sync_info_t *cur = head; cur != NULL; cur = cur->next) {
        printf("  %s -> %s [active=%d]\n", cur->source, cur->target, cur->active);
    }
}

int add_sync_pair(const char *src, const char *tgt) {
    // Check if it already exists
    for (sync_info_t *cur = head; cur; cur = cur->next) {
        if (strcmp(cur->source, src) == 0 && strcmp(cur->target, tgt) == 0) {
            return 0; // already exists
        }
    }

    // Otherwise, add new pair
    sync_info_t *node = malloc(sizeof(sync_info_t));
    strncpy(node->source, src, MAX_PATH_LEN);
    strncpy(node->target, tgt, MAX_PATH_LEN);
    node->active = 1;
    node->next = head;
    head = node;

    return 1; // success
}

void add_watch(sync_info_t *pair) {
    int wd = inotify_add_watch(inotify_fd, pair->source,
               IN_CREATE | IN_MODIFY | IN_DELETE);
    if (wd < 0) {
        perror("inotify_add_watch");
        return;
    }
    printf("[watch] Monitoring %s (wd=%d)\n", pair->source, wd);
}

void *watcher_thread_fn(void *arg) {
    char buffer[EVENT_BUF_LEN];

    while (1) {
        int length = read(inotify_fd, buffer, EVENT_BUF_LEN);
        if (length < 0) {
            perror("inotify read");
            continue;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];

            if (event->len > 0) {
                const char *op = NULL;
                if (event->mask & IN_CREATE) op = "ADDED";
                else if (event->mask & IN_MODIFY) op = "MODIFIED";
                else if (event->mask & IN_DELETE) op = "DELETED";

                if (op && head) { // TEMP: only first pair
                    pid_t pid = fork();
                    if (pid == 0) {
                        execl("./bin/worker", "worker",
                              head->source, head->target,
                              event->name, op, (char *)NULL);
                        perror("exec worker");
                        exit(1);
                    } else if (pid > 0) {
                        printf("[watcher] %s: %s\n", op, event->name);
                    } else {
                        perror("fork");
                    }
                }
            }

            i += sizeof(struct inotify_event) + event->len;
        }
    }
    return NULL;
}