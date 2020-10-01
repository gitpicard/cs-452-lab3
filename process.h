#ifndef PROCESS_H
#define PROCESS_H

typedef void (*signal_handler)(int);
void install_signal(int num, signal_handler handler);
int* get_childern(int* buffer);

void parent_loop(void);
void child_loop(void);

#endif
