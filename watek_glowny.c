#include "main.h"
#include "watek_glowny.h"

void mainLoop()
{
    srandom(rank);
    int wants_to_make_jam = 0;
    int wants_to_eat_jam = 0;
    int localAck = 0;
    int jars=0, jams=0;

    while (TRUE) {

        debug("Stan słoików i konfitur: dostępne słoiki = %d, używane konfitury = %d", availableJars, usingJams);
        switch (stan) {
            // BABCIE
            case INACTIVE_BABCIA:
                wants_to_make_jam = random() % 2; 
                if (wants_to_make_jam) {
                    debug("Chcę robić konfitury.");
                    changeState(WAIT_BABCIA);   
                } else {
                    debug("Nie chcę robić konfitury.");
                }
                break;

            case WAIT_BABCIA:
                debug("Czy mogę robić konfitury?");
                pthread_mutex_lock(&clockMut);
                priority = ++clockLamp;
                pthread_mutex_unlock(&clockMut);

                pthread_mutex_lock(&ackJarMut);
                ackJarNum = 0;
                pthread_mutex_unlock(&ackJarMut);

                pthread_mutex_lock(&jarQueueMut);
                addsToQueue(REQ_JAR, priority, &JarQueue);
                pthread_mutex_unlock(&jarQueueMut);
                sendToAll(REQ_JAR, priority);

                localAck = 0;
                do {
                    pthread_mutex_lock(&ackJarMut);
                    localAck = ackJarNum;
                    pthread_mutex_unlock(&ackJarMut);

                    pthread_mutex_lock(&availableJarsMut);
                    jars = availableJars;
                    pthread_mutex_unlock(&availableJarsMut);
                } while ((BABCIE - 1 - localAck >= SLOIKI) || (jars < 1));

                pthread_mutex_lock(&jarQueueMut);
                while(!isAtQueueTop(JarQueue, rank)){
                    pthread_mutex_unlock(&jarQueueMut);
                    usleep(1000);
                    pthread_mutex_lock(&jarQueueMut);
                }
                dequeue(&JarQueue);
                pthread_mutex_unlock(&jarQueueMut);

                pthread_mutex_lock(&availableJarsMut);
                if (availableJars > 0) {
                    availableJars--;
                }
                pthread_mutex_unlock(&availableJarsMut);
                changeState(INSECTION_BABCIA);
                break;
            
            case INSECTION_BABCIA:
                debug("Robię konfitury");
                //make_jam();

                pthread_mutex_lock(&usingJamsMut);
                if (usingJams < KONFITURY) {
                    usingJams++;
                }
                pthread_mutex_unlock(&usingJamsMut);

                sendToAll(NEW_JAM, priority);
                changeState(INACTIVE_BABCIA);
                debug("Zrobiłam konfitury");
                break;

            // STUDENTKI
            case INACTIVE_STUDENTKA:
                wants_to_eat_jam = random() % 2; 
                if (wants_to_eat_jam) {
                    debug("Chcę jeść konfitury.");
                    changeState(WAIT_STUDENTKA);   
                } else {
                    debug("Nie chcę jeść konfitury.");
                }
                break;

            case WAIT_STUDENTKA:
                debug("Czy mogę zjeść konfiture?");
                pthread_mutex_lock(&clockMut);
                priority = ++clockLamp;
                pthread_mutex_unlock(&clockMut);

                pthread_mutex_lock(&ackJamMut);
                ackJamNum = 0;
                pthread_mutex_unlock(&ackJamMut);
                
                pthread_mutex_lock(&jamQueueMut);
                addsToQueue(REQ_JAM, priority, &JamQueue);
                pthread_mutex_unlock(&jamQueueMut);
                sendToAll(REQ_JAM, priority);

                localAck = 0;
                do {
                    pthread_mutex_lock(&ackJamMut);
                    localAck = ackJamNum;
                    pthread_mutex_unlock(&ackJamMut);

                    pthread_mutex_lock(&usingJamsMut);
                    jams = usingJams;
                    pthread_mutex_unlock(&usingJamsMut);
                } while ((STUDENTKI - 1 - localAck >= KONFITURY) || (jams < KONFITURY-1));
  
                pthread_mutex_lock(&jamQueueMut);
                while(!isAtQueueTop(JamQueue, rank)){
                    pthread_mutex_unlock(&jamQueueMut);
                    usleep(1000);
                    pthread_mutex_lock(&jamQueueMut);
                }
                dequeue(&JamQueue);
                pthread_mutex_unlock(&jamQueueMut);

                changeState(INSECTION_STUDENTKA);
                break;
            
            case INSECTION_STUDENTKA:
                debug("Jem konfitury");
                //eat_jam();
                
                pthread_mutex_lock(&usingJamsMut);
                if (usingJams > 0) {
                    usingJams--;
                }
                pthread_mutex_unlock(&usingJamsMut);

                pthread_mutex_lock(&availableJarsMut);
                if (availableJars < SLOIKI) {
                    availableJars++;
                }
                pthread_mutex_unlock(&availableJarsMut);

                sendToAll(FRE_RES, priority);
                changeState(INACTIVE_STUDENTKA);
                debug("Zjadłam konfitury");
                break;
        }
        sleep(SEC_IN_STATE);
    }
}

