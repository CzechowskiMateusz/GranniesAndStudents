#include "main.h"
#include "watek_komunikacyjny.h"

void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;

    while ( TRUE ) {
	debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch ( status.MPI_TAG ) {
            case REQ_JAR:
                if(pakiet.ts < priority || (pakiet.ts == priority && pakiet.src < rank)) {
                    sendAck(pakiet.src, ACK_JAR);
                }else{
                    addsToQueue(REQ_JAR, priority, JarQueue);
                }
            break;

            case ACK_JAR: 
                debug("Otrzymałem ACK_JAR");  // MUTEX?
            break;

            case NEW_JAM:
                availableJars--;  
                usingJams++;
            break;

            case FRE_RES:
                usingJams--;
                availableJars++;
            break;
        }
    }
}
