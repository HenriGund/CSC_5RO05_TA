#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

static volatile int counter = 0;
static timer_t tid;

/* Handler appelé à chaque expiration du timer */
void myHandler(int sig, siginfo_t *si, void *uc)
{
    (void)sig;
    (void)uc;

    counter++;
    printf("Compteur = %d\n", counter);
    fflush(stdout);

    if (counter >= 15) {
        timer_delete(tid);
        printf("Fin du programme\n");
        exit(0);
    }
}

int main(void)
{
    struct sigaction sa;
    struct sigevent sev;
    struct itimerspec its;

    /* Installation du handler */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = myHandler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Configuration du timer */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo  = SIGRTMIN;
    sev.sigev_value.sival_ptr = &tid;

    if (timer_create(CLOCK_REALTIME, &sev, &tid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    /* 2 Hz => période = 0.5 s */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 500000000;   // premier déclenchement (0.5 s)

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 500000000; // période (0.5 s)

    if (timer_settime(tid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    /* Boucle passive : on attend les signaux */
    while (1)
        pause();

    return 0;
}