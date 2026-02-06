#include <time.h>
#include <stdio.h>
#include <unistd.h>


/*
timespec est une structure définie dans <time.h> qui représente 
une durée ou un point dans le temps. Elle contient deux membres : 
tv_sec, qui représente les secondes, et tv_nsec, qui représente les nanosecondes.
*/


/*
‍Déclarez et implémentez les fonctions suivantes permettant de convertir
des millisecondes en timespec et vice-versa: 
*/

/*
Pour le faire, il suffit de multiplier les secondes par 1000 pour obtenir les millisecondes, 
et de diviser les nanosecondes par 1e6 pour les convertir en millisecondes. Car :
1 s = 1000 ms
1 ns = 1e-6 ms
*/
double timespec_to_ms(const timespec& time_ts){
    return (time_ts.tv_sec * 1000.0) + (time_ts.tv_nsec / 1e6);
}

/*
Pour le faire, il faut diviser les millisecondes par 1000 pour obtenir les secondes,
et multiplier les millisecondes par 1e6 pour les convertir en nanosecondes. Car :
1 ms = 0.001 s  
1 ms = 1e6 ns
*/
timespec timespec_from_ms(double time_ms){
    timespec ts;
    ts.tv_sec = (time_t)(time_ms / 1000.0);
    ts.tv_nsec = (long)((time_ms - (ts.tv_sec * 1000.0)) * 1e6);
    return ts;
}

/*
Déclarez et implémentez les fonctions suivantes permettant respectivement
d’obtenir le temps courant et l’opposé d’un temps: 
*/

timespec timespec_now(){
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts;
}

timespec timespec_negate(const timespec& time_ts){
    timespec ts;
    ts.tv_sec = -time_ts.tv_sec;
    ts.tv_nsec = -time_ts.tv_nsec;
    if (ts.tv_nsec < 0) {
        ts.tv_sec -= 1;
        ts.tv_nsec += 1000000000;
    }
    return ts;
}

/*Déclarez et implémentez les fonctions suivantes permettant 
respectivement d’ajouter et de soustraire deux timespec: 
*/

timespec timespec_add(const timespec& time1_ts, const timespec& time2_ts){
    timespec ts;
    ts.tv_sec = time1_ts.tv_sec + time2_ts.tv_sec;
    ts.tv_nsec = time1_ts.tv_nsec + time2_ts.tv_nsec;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }
    return ts;
}

timespec timespec_subtract(const timespec& time1_ts, const timespec& time2_ts){
    timespec ts;
    ts.tv_sec = time1_ts.tv_sec - time2_ts.tv_sec;
    ts.tv_nsec = time1_ts.tv_nsec - time2_ts.tv_nsec;
    if (ts.tv_nsec < 0) {
        ts.tv_sec -= 1;
        ts.tv_nsec += 1000000000;
    }
    return ts;
}

/*En utilisant la fonction Posix nanosleep(), déclarez et implémentez 
la fonction suivante permettant d’endormir la tâche appelante durant le 
temps spécifié; attention, il faut gérer l’éventualité où l’appel de nanosleep() 
soit interrompu (voir man nanosleep) : 
*/

void timespec_wait(const timespec& delay_ts){
    timespec remaining_ts;
    timespec delay_copy = delay_ts;
    while (nanosleep(&delay_copy, &remaining_ts) == -1) {
        delay_copy = remaining_ts;
    }
}

/*
‍En utilisant les fonctions ci-dessous, déclarez et implémentez les opérateurs 
ci-dessous afin de pouvoir utiliser les variables timespec comme des scalaires: 
*/

timespec  operator- (const timespec& time_ts) {
    return timespec_negate(time_ts);
}

timespec  operator+ (const timespec& time1_ts, const timespec& time2_ts) {
    return timespec_add(time1_ts, time2_ts);
}

timespec  operator- (const timespec& time1_ts, const timespec& time2_ts) {
    return timespec_subtract(time1_ts, time2_ts);
}

timespec& operator+= (timespec& time_ts, const timespec& delay_ts) {
    time_ts = timespec_add(time_ts, delay_ts);
    return time_ts;
}
timespec& operator-= (timespec& time_ts, const timespec& delay_ts) {
    time_ts = timespec_subtract(time_ts, delay_ts);
    return time_ts;
}
bool operator== (const timespec& time1_ts, const timespec& time2_ts) {
    return (time1_ts.tv_sec == time2_ts.tv_sec) && (time1_ts.tv_nsec == time2_ts.tv_nsec);
}
bool operator!= (const timespec& time1_ts, const timespec& time2_ts) {
    return !(time1_ts == time2_ts);
}
bool operator< (const timespec& time1_ts, const timespec& time2_ts) {
    if (time1_ts.tv_sec < time2_ts.tv_sec) {
        return true;
    } else if (time1_ts.tv_sec == time2_ts.tv_sec) {
        return time1_ts.tv_nsec < time2_ts.tv_nsec;
    } else {
        return false;
    }
}
bool operator> (const timespec& time1_ts, const timespec& time2_ts) {
    return time2_ts < time1_ts;
}

/*
‍Écrivez un programme main() permettant de tester toutes ces fonctions. 
*/

int main() {
    timespec ts1, ts2;
    ts1.tv_sec = 2;
    ts1.tv_nsec = 700000000;    // construction manuelle d’un timespec
    ts2.tv_sec = 1;
    ts2.tv_nsec = 300000000;    // construction manuelle d’un timespec

    printf("Timespec 1: %ld sec, %ld nsec\n", ts1.tv_sec, ts1.tv_nsec);
    printf("Timespec 2: %ld sec, %ld nsec\n", ts2.tv_sec, ts2.tv_nsec);


    // Test conversion functions
    double ms = timespec_to_ms(ts1);
    printf("Original timespec: %ld sec, %ld nsec\n", ts1.tv_sec, ts1.tv_nsec);
    printf("Converted to ms: %f ms\n", ms);

    timespec ts_from_ms = timespec_from_ms(ms);
    printf("Converted back to timespec: %ld sec, %ld nsec\n", ts_from_ms.tv_sec, ts_from_ms.tv_nsec);

    // Test current time function
    timespec now = timespec_now();
    printf("Current timespec: %ld sec, %ld nsec\n", now.tv_sec, now.tv_nsec);

    // Test negate function
    timespec negated = timespec_negate(ts1);
    printf("Negated timespec: %ld sec, %ld nsec\n", negated.tv_sec, negated.tv_nsec);

    // Test add and subtract functions
    timespec added = timespec_add(ts1, ts_from_ms);
    printf("Added timespec: %ld sec, %ld nsec\n", added.tv_sec, added.tv_nsec);

    timespec subtracted = timespec_subtract(ts1, ts_from_ms);
    printf("Subtracted timespec: %ld sec, %ld nsec\n", subtracted.tv_sec, subtracted.tv_nsec);

    // Test wait function (uncomment to test)
    // timespec_wait(ts1);

    // Test operator overloads
    timespec op_negated = -ts1;
    printf("Operator negated timespec: %ld sec, %ld nsec\n", op_negated.tv_sec, op_negated.tv_nsec);

    timespec op_added = ts1 + ts2;
    printf("Operator added timespec: %ld sec, %ld nsec\n", op_added.tv_sec, op_added.tv_nsec);

    timespec op_subtracted = ts1 - ts2;
    printf("Operator subtracted timespec: %ld sec, %ld nsec\n", op_subtracted.tv_sec, op_subtracted.tv_nsec);


    bool eq = (ts1 == ts2);
    bool eq2 = (ts1 == ts1);

    printf("Timespec 1 == Timespec 2: %s\n", eq ? "true" : "false");
    printf("Timespec 1 == Timespec 1: %s\n", eq2 ? "true" : "false");

    bool neq = (ts1 != ts_from_ms);
    printf("Timespec 1 != Timespec from ms: %s\n", neq ? "true" : "false");

    bool lt = (ts1 < ts_from_ms);
    printf("Timespec 1 < Timespec from ms: %s\n", lt ? "true" : "false");

    bool gt = (ts1 > ts_from_ms);
    printf("Timespec 1 > Timespec from ms: %s\n", gt ? "true" : "false");

    return 0;
}

/*
La classe Chrono implémente les fonctionnalités de mesures de temps d’un chronomètre. Son interface est définie par le diagramme de classe de la figure 1.

Les méthodes de la classes Chrono doivent respecter les spécifications suivantes :

    À sa création, le chronomètre initialise son temps de démarrage par un appel à restart().
    La méthode restart() initialise l’attribut m_startTime au temps courant et m_stopTime à la même valeur que m_startTime.
    L’appel de stop() fixe la valeur de m_stopTime au temps courant et renvoie le temps écoulé depuis restart().
    On considère que le chronomètre est actif tant que stop() n’a pas été appelé après un appel à restart().
    On considère que le chronomètre est désactivé tant que restart() n’a pas été appelé après un appel à stop().
    Si le chronomètre est actif, l’appel de lap() renvoie sous la forme d’une structure timespec la durée écoulée depuis le dernier appel à restart().
    Si le chronomètre est désactivé, l’appel de lap() renvoie sous la forme d’une structure timespec la durée écoulée entre les derniers appels à restart() et stop().
    L’appel à lap_ms() renvoie la même durée que l’appel à lap(), exprimée en millisecondes.

Déclarez la classe Chrono en spécifiant les méthodes qui doivent être const.
Implémentez la classe Chrono en C++ en utilisant vos fonctions timespec_* développées à la l’exercice du début.
Testez votre classe par exemple en utilisant votre propre montre. 
*/

class Chrono {
private:
    timespec m_startTime;
    timespec m_stopTime;
    bool m_running;
public:
    Chrono() {
        restart();
    }
    void restart() {
        m_startTime = timespec_now();
        m_stopTime = m_startTime;
        m_running = true;
    }
    timespec stop() {
        m_stopTime = timespec_now();
        m_running = false;
        return timespec_subtract(m_stopTime, m_startTime);
    }
    timespec lap() const {
        if (m_running) {
            timespec now = timespec_now();
            return timespec_subtract(now, m_startTime);
        } else {
            return timespec_subtract(m_stopTime, m_startTime);
        }
    }
    double lap_ms() const {
        timespec lap_time = lap();
        return timespec_to_ms(lap_time);
    }
};