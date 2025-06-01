#ifndef MAINH
#define MAINH
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "util.h"

/* boolean */
#define TRUE 1
#define FALSE 0
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10

/* message tags */
#define REQ_JAR 1
#define ACK_JAR 2
#define REQ_JAM 3
#define ACK_JAM 4
#define NEW_JAM 5
#define FRE_RES 6

/* liczba babć i studentów */
#define BABCIE 3
#define STUDENTKI 4

/* liczba słoików i konfitur */
#define SLOIKI 5 
#define KONFITURY 4

/* zmienne pomocnicze */
extern int clockLamp;
extern int priority;
extern int availableJars;
extern int usingJams;
extern struct kolejka *JarQueue;
extern struct kolejka *JamQueue;
extern int ackJarNum;
extern int ackJamNum;

extern int rank;
extern int size;
typedef enum {INACTIVE_BABCIA, WAIT_BABCIA, INSECTION_BABCIA, INACTIVE_STUDENTKA, WAIT_STUDENTKA, INSECTION_STUDENTKA} state_t;

extern state_t stan;
extern pthread_t threadKom, threadMon;
extern pthread_mutex_t stateMut;
extern pthread_mutex_t clockMut;
extern pthread_mutex_t ackJarMut;
extern pthread_mutex_t ackJamMut;
extern pthread_mutex_t availableJarsMut;
extern pthread_mutex_t usingJamsMut;
extern pthread_mutex_t jarQueueMut;
extern pthread_mutex_t jamQueueMut;
extern pthread_mutex_t stdoutMut;
extern pthread_mutex_t pktMut;

/* funkcje */
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// ------------------------------------------
/* macro debug - działa jak printf, kiedy zdefiniowano
   DEBUG, kiedy DEBUG niezdefiniowane działa jak instrukcja pusta 
   
   używa się dokładnie jak printfa, tyle, że dodaje kolorków i automatycznie
   wyświetla rank

   w związku z tym, zmienna "rank" musi istnieć.

   w printfie: definicja znaku specjalnego "%c[%d;%dm [%d]" escape[styl bold/normal;kolor [RANK]
                                           FORMAT:argumenty doklejone z wywołania debug poprzez __VA_ARGS__
					   "%c[%d;%dm"       wyczyszczenie atrybutów    27,0,37
                                            UWAGA:
                                                27 == kod ascii escape. 
                                                Pierwsze %c[%d;%dm ( np 27[1;10m ) definiuje styl i kolor literek
                                                Drugie   %c[%d;%dm czyli 27[0;37m przywraca domyślne kolory i brak pogrubienia (bolda)
                                                ...  w definicji makra oznacza, że ma zmienną liczbę parametrów
                                            
*/
#ifdef DEBUG
#define debug(FORMAT,...) do { \
    char buf[1024]; \
    int len = snprintf(buf, sizeof(buf), "\033[%d;%dm[%d]: " FORMAT "\033[0m\n", \
                      (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__); \
    if (len > 0 && len < sizeof(buf)) { \
        write(STDOUT_FILENO, buf, len); \
    } \
} while(0)
#else
#define debug(...) ;
#endif

// makro println - to samo co debug, ale wyświetla się zawsze
#define println(FORMAT,...) do { \
    char buf[1024]; \
    int len = snprintf(buf, sizeof(buf), "\033[%d;%dm[%d]: " FORMAT "\033[0m\n", \
                      (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__); \
    if (len > 0 && len < sizeof(buf)) { \
        write(STDOUT_FILENO, buf, len); \
    } \
} while(0)

void changeState( state_t );

#endif
