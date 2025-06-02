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
        
        int totalInUse = SLOIKI - availableJars;
        debug("Słoiki zajęte: %d (babcia + studentki), używane konfitury = %d", totalInUse, usingJams);
        //debug("Stan słoików i konfitur: dostępne słoiki = %d, używane konfitury = %d", availableJars, usingJams);
        
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

                sendToAll(REQ_JAR, priority);

                localAck = 0;
                pthread_mutex_lock(&availableJarsMut);
                do {
                    pthread_mutex_lock(&ackJarMut);
                    localAck = ackJarNum;
                    pthread_mutex_unlock(&ackJarMut);

                    jars = availableJars;
                    if ((BABCIE - 1 - localAck <= jars)) {
                        break;
                    }
                    pthread_cond_wait(&jarAvailableCond, &availableJarsMut);
                } while (1);

                if (availableJars > 0) {
                    availableJars--;
                }
                pthread_mutex_unlock(&availableJarsMut);
                changeState(INSECTION_BABCIA);
                break;
            
            case INSECTION_BABCIA:
                debug("Robię konfitury");
                make_jam();

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
                
                sendToAll(REQ_JAM, priority);

                localAck = 0;
                pthread_mutex_lock(&usingJamsMut);
                do {
                    pthread_mutex_lock(&ackJamMut);
                    localAck = ackJamNum;
                    pthread_mutex_unlock(&ackJamMut);

                    jams = usingJams;
                    if ((STUDENTKI - 1 - localAck <= jams)) {
                        break;
                    }
                    pthread_cond_wait(&jamAvailableCond, &usingJamsMut);
                } while (1);

                if (usingJams > 0) {
                    usingJams--;
                }
                pthread_mutex_unlock(&usingJamsMut);
                changeState(INSECTION_STUDENTKA);
                break;
            
            case INSECTION_STUDENTKA:
                debug("Jem konfitury");
                eat_jam();
                
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

