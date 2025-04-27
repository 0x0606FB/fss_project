//
// Created by redux on 4/19/25.
//

#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

void listen_for_command(void);

void add_command(const char *src, const char *tgt, int fd_out);
void cancel_command(const char *src, const char *tgt, int fd_out);
void status_command(int fd_out);
void sync_command(const char *src, const char *tgt, int fd_out);

#endif //COMMAND_PROCESSOR_H
