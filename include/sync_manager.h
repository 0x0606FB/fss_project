#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

#define MAX_PATH_LEN 512

extern int inotify_fd;
extern pthread_t inotify_thread;
extern struct sync_info *head;

typedef struct sync_info {
    char source[MAX_PATH_LEN];
    char target[MAX_PATH_LEN];
    int active;
    struct sync_info *next;
} sync_info_t;

void parse_config_file(const char *path);
void sync_pair_dump(void);

int add_sync_pair(const char *src, const char *tgt);
void add_watch(sync_info_t *pair);
void *watcher_thread_fn(void *arg);

#endif // SYNC_MANAGER_H
