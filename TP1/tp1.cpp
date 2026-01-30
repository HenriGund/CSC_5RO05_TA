#include "timep.h"

time_t tv_sec;
long tv_nsec;
timespec tsp, tsn;
 
// Initialize with 2.7 seconds
// Equivalent to 2 seconds and 700 million nanoseconds
tsp.tv_sec = 2;
tsp.tv_nsec = 700000000;

/*
‍Déclarez et implémentez les fonctions suivantes permettant de convertir
des millisecondes en timespec et vice-versa: 
*/

double timespec_to_ms(const timespec& time_ts){
    return (time_ts.tv_sec * 1000.0) + (time_ts.tv_nsec / 1e6);
}

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
    // Test conversion functions
    double ms = timespec_to_ms(tsp);
    timespec ts_from_ms = timespec_from_ms(ms);

    // Test current time function
    timespec now = timespec_now();

    // Test negate function
    timespec negated = timespec_negate(tsp);

    // Test add and subtract functions
    timespec added = timespec_add(tsp, ts_from_ms);
    timespec subtracted = timespec_subtract(tsp, ts_from_ms);

    // Test wait function (uncomment to test)
    // timespec_wait(tsp);

    // Test operator overloads
    timespec op_negated = -tsp;
    timespec op_added = tsp + ts_from_ms;
    timespec op_subtracted = tsp - ts_from_ms;

    tsp += ts_from_ms;
    tsp -= ts_from_ms;

    bool eq = (tsp == ts_from_ms);
    bool neq = (tsp != ts_from_ms);
    bool lt = (tsp < ts_from_ms);
    bool gt = (tsp > ts_from_ms);

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