#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"

int rank, size;

pthread_t threadKom, threadMon;
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clockMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ackJarMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ackJamMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t availableJarsMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t usingJamsMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t jarQueueMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t jamQueueMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stdoutMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pktMut = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t jarAvailableCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t jamAvailableCond = PTHREAD_COND_INITIALIZER;

void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    pthread_mutex_destroy( &clockMut);
    pthread_mutex_destroy( &ackJarMut);
    pthread_mutex_destroy( &ackJamMut);
    pthread_mutex_destroy( &availableJarsMut);
    pthread_mutex_destroy( &usingJamsMut);
    pthread_mutex_destroy( &jarQueueMut);
    pthread_mutex_destroy( &jamQueueMut);
    pthread_mutex_destroy( &pktMut);
    pthread_cond_destroy(&jarAvailableCond);
    pthread_cond_destroy(&jamAvailableCond);

    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    pthread_join(threadKom,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}

int clockLamp = 0;
int priority = 0;
int availableJars = SLOIKI;
int usingJams = 0;
struct kolejka *JarQueue = NULL;
struct kolejka *JamQueue = NULL;
int ackJarNum = 0;
int ackJamNum = 0;
state_t stan;

int main(int argc, char **argv)
{
    MPI_Status status;
    int provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);
    srand(rank);
    inicjuj_typ_pakietu(); 
    
    packet_t pkt;
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    if (size < BABCIE + STUDENTKI) {
        fprintf(stderr, "Za mało procesów! Potrzebuję przynajmniej %d procesów.\n", BABCIE + STUDENTKI);
        finalizuj();
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank < BABCIE) { 
        stan=INACTIVE_BABCIA;
    }else{
        stan=INACTIVE_STUDENTKA;
    }

    pthread_create( &threadKom, NULL, startKomWatek , 0);

    mainLoop();
    
    finalizuj();
    return 0;
}

