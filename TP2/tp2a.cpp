#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static volatile int counter = 0;

void myHandler(int signum) {
    (void)signum;
    counter++;
    printf("Compteur = %d\n", counter);

    if (counter >= 15) {
        printf("Fin du programme\n");
        exit(0);
    }
}

int main(void){
    struct sigaction sa;
    struct sigevent sev;
    timer_t tid;
    itimerspec its;

    /* Installation du handler */
    sa.sa_handler = myHandler;   
    sa.sa_flags = 0;             
    sigemptyset(&sa.sa_mask);

    sigaction(SIGRTMIN, &sa, NULL);
     
    /* Configuration du timer */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo  = SIGRTMIN;
    sev.sigev_value.sival_ptr = &tid;

    timer_create(CLOCK_REALTIME, &sev, &tid);

    /* 2 Hz => période 0.5 s */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 500000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 500000000;

    timer_settime(tid, 0, &its, NULL);

    while(1) {
        pause();
    }
    
    return 0;
}