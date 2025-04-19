// File: src/include/syncManager.h
#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

#define MAX_PATH_LEN 512

typedef struct sync_info {
    char source[MAX_PATH_LEN];
    char target[MAX_PATH_LEN];
    int active;
    struct sync_info *next;
} sync_info_t;

void parse_config_file(const char *path);
void sync_pair_dump(void);

#endif // SYNC_MANAGER_H
