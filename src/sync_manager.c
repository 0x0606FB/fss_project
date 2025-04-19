// File: src/sync_manager.c
#include "../include/sync_manager.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sync_info_t *head = NULL;

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
