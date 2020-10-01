#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "process.h"

static int* childern = NULL;
static int childern_num = 0;

void install_signal(int num, signal_handler handler) {
    struct sigaction sig;
    sig.sa_handler = handler;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(num, &sig, NULL);
}

int* get_childern(int* num) {
    /* return an array of the pids and how many there are */
    *num = childern_num;
    return childern;
}

int main(int argc, char* argv[]) {
    int num_child = 1;
    int is_child = 0;

    if (argc == 2) {
        /* read from the argument how many child processes to create */
        num_child = atoi(argv[1]);
    }
    else if (argc > 2) {
        /* display a short usage message */
        printf("usage: %s [num-childern]\n", argv[0]);
        return 1;
    }

    /* allocate a list of the pid of all the child processes */
    childern = malloc(sizeof(int) * num_child);

    if (childern == NULL) {
        perror("memory alloc failed");
        return 1;
    }

    /* during the creation of the processes we need to temporarily
     * override the signal handler in the parent becuase the childern
     * could send signals before the parent knows what to do with them
     * and that would result in the parent exiting since that is
     * the default signal handler */
    install_signal(SIGUSR1, NULL);
    install_signal(SIGUSR2, NULL);

    for (childern_num = 0; childern_num < num_child; childern_num++) {
        int pid = fork();
        if (pid == 0) {
            /* this flag will control which main loop we go into */
            is_child = 1;
            /* make sure that the child does not create more childern */
            break;
        }
        else if (pid > 0)
            childern[childern_num] = pid;
        else {
            perror("fork failed");
            free(childern);
            return 1;
        }
    }

    /* correctly count the number of childern since the loop
     * will leave us at count - 1 */
    childern_num++;

    if (is_child)
        child_loop();
    else {
        /* renable the signal handlers */
        signal(SIGUSR1, SIG_DFL);
        signal(SIGUSR2, SIG_DFL);
        parent_loop();
    }

    free(childern);
    return 0;
}
