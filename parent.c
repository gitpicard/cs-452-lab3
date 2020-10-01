#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "process.h"

static int done = 0;
static int usr1 = 0;
static int usr2 = 0;


static void signal_int(int num) {
    int *childern, child_num;
    /* tell all the childern to exit as well */
    childern = get_childern(&child_num);

    while (child_num >= 0) {
        kill(childern[child_num], SIGUSR1);
        child_num--;
    }

    done = 1;
    /* the user interrupts might fire in the time it takes us to tell eveyone else
     * to quit so make sure to disable those interrupts again */
    usr1 = 0;
    usr2 = 0;
}

static void signal_usr(int num) {
    /* it is not safe to call printf from inside a
     * signal/interrupt so we will set a flag that
     * we will check for inside of our main loop
     * in order to print there */
    if (num == SIGUSR1)
        usr1 = 1;
    else if (num == SIGUSR2)
        usr2 = 1;
}

void parent_loop(void) {
    /* install signal handlers for the varius things
     * that our parent process needs to listen to */
    install_signal(SIGINT, signal_int);
    install_signal(SIGUSR1, signal_usr);
    install_signal(SIGUSR2, signal_usr);

    while (1) {
        /* check the flags to see if one of the interrupts
         * from a child process was called */
        if (usr1) {
            printf("recevied USR1\n");
            usr1 = 0;
            usr2 = 0;
        }
        if (usr2) {
            printf("recevied USR2\n");
            usr2 = 0;
            usr1 = 0;
        }
        if (done) {
            /* don't print the carot because the os does that for us */
            printf(" recevied.\n");
            break;
        }

        printf("waiting...\t");
        fflush(stdout);
        /* wait for an interrupt from the user to end the program
         * or from a child process */
        pause();
    }

    /* wait for all the childern to end */
    wait(NULL);
}
