#include "main.h"
#include "watek_komunikacyjny.h"

void *startKomWatek(void *ptr)
{
    MPI_Status status;
    packet_t pakiet;

    while ( TRUE ) {
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        pthread_mutex_lock(&clockMut);
        clockLamp = MAX(clockLamp, pakiet.ts) + 1;
        pthread_mutex_unlock(&clockMut);

        switch (status.MPI_TAG) {
            case REQ_JAR:
                if (stan != WAIT_BABCIA ||
                    pakiet.ts < priority ||
                    (pakiet.ts == priority && pakiet.src < rank)) {
                    sendAck(pakiet.src, ACK_JAR);
                }
                break;

            case REQ_JAM:
                if (stan != WAIT_STUDENTKA ||
                    pakiet.ts < priority ||
                    (pakiet.ts == priority && pakiet.src < rank)) {
                    sendAck(pakiet.src, ACK_JAM);
                }
                break;

            case ACK_JAR:
                pthread_mutex_lock(&ackJarMut);
                ackJarNum++;
                pthread_mutex_unlock(&ackJarMut);
                
                pthread_mutex_lock(&availableJarsMut);
                pthread_cond_broadcast(&jarAvailableCond);
                pthread_mutex_unlock(&availableJarsMut);
                break;

            case ACK_JAM:
                pthread_mutex_lock(&ackJamMut);
                ackJamNum++;
                pthread_mutex_unlock(&ackJamMut);
                
                pthread_mutex_lock(&usingJamsMut);
                pthread_cond_broadcast(&jamAvailableCond);
                pthread_mutex_unlock(&usingJamsMut);
                break;

            case NEW_JAM:
                if (pakiet.src != rank) {
                    pthread_mutex_lock(&usingJamsMut);
                    if (usingJams < KONFITURY) {
                        usingJams++;
                    }
                    pthread_cond_broadcast(&jamAvailableCond);
                    pthread_mutex_unlock(&usingJamsMut);

                    pthread_mutex_lock(&availableJarsMut);
                    if (availableJars > 0) {
                        availableJars--;
                    }
                    pthread_mutex_unlock(&availableJarsMut);
                }
                break;

            case FRE_RES:
                if (pakiet.src != rank) {
                    pthread_mutex_lock(&usingJamsMut);
                    if (usingJams > 0) {
                        usingJams--;
                    }
                    pthread_mutex_unlock(&usingJamsMut);

                    pthread_mutex_lock(&availableJarsMut);
                    if (availableJars < SLOIKI) {
                        availableJars++;
                    }
                    pthread_cond_broadcast(&jarAvailableCond);
                    pthread_mutex_unlock(&availableJarsMut);
                }
                break;
        }
    }
}
