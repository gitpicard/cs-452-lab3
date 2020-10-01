#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include "process.h"

static int done = 0;

static void signal_usr(int num) {
    done = 1;
}

void child_loop(void) {
    int sleep_time;
    struct timeval tv;
    unsigned long micros;
    /* pick a random seed so that we don't get the same result
     * all of the time, we will need to use microseconds because
     * the processes are all created at the same time and so
     * would have the same seed and generate the same numbers */
    gettimeofday(&tv, NULL);
    micros = 1000000 * tv.tv_sec + tv.tv_usec;
    srand(micros);

    install_signal(SIGUSR1, signal_usr);

    while (!done) {
        /* generate a random time to sleep for */
        sleep_time = rand() % 6;
        /* sleep for that amount of time */
        sleep(sleep_time);
        /* pick a random interrupt to fire */
        if (rand() % 2)
            kill(getppid(), SIGUSR1);
        else
            kill(getppid(), SIGUSR2);
    }
}
