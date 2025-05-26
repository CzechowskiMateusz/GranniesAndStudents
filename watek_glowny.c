#include "main.h"
#include "watek_glowny.h"

void mainLoop()
{
    srandom(rank);
    int tag;
    int wants_to_make_jam = 0;
    int wants_to_eat_jam = 0;
    int ack_jar=0, ack_jam=0;
    
    while (TRUE) {

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
                priority = clockLamp+1;
                
                pthread_mutex_lock(&ackJarMut);
                ack_jar = 0;
                pthread_mutex_unlock(&ackJarMut);

                addsToQueue(REQ_JAR, priority, JarQueue);
                sendToAll(REQ_JAR, priority);

                debug("Czekam na odpowiedzi");
                while (1) {
                    pthread_mutex_lock(&ackJarMut);
                    int current_ack_jar = ack_jar;
                    pthread_mutex_unlock(&ackJarMut);

                    pthread_mutex_lock(&availableJarsMut);
                    int current_availableJars = availableJars;
                    pthread_mutex_unlock(&availableJarsMut);

                    if (((BABCIE - 1) - current_ack_jar >= SLOIKI) && current_availableJars <= 0) {
                        break;
                    }

                    if (recvPacket(REQ_JAR, &ack_jar, ACK_JAR)) {
                        pthread_mutex_lock(&ackJarMut);
                        ack_jar++;
                        pthread_mutex_unlock(&ackJarMut);
                    }
                }

                while(!isAtQueueTop(JarQueue, rank)){}

                dequeue(JarQueue);
                pthread_mutex_lock(&availableJarsMut);
                availableJars--;
                debug("Stan słoików i konfitur: dostępne słoiki = %d, używane konfitury = %d", availableJars, usingJams);
                pthread_mutex_unlock(&availableJarsMut);
                changeState(INSECTION_BABCIA);
                break;
            
            case INSECTION_BABCIA:
                debug("Robię konfitury");
                //make_jam();

                pthread_mutex_lock(&usingJamsMut);
                usingJams += 1;
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
                priority = clockLamp+1;

                pthread_mutex_lock(&ackJamMut);
                ack_jam = 0;
                pthread_mutex_unlock(&ackJamMut);

                addsToQueue(REQ_JAM, priority, JamQueue);
                sendToAll(REQ_JAM, priority);

                debug("Czekam na odpowiedzi");
                while (1) {
                    pthread_mutex_lock(&ackJamMut);
                    int current_ack_jam = ack_jam;
                    pthread_mutex_unlock(&ackJamMut);

                    pthread_mutex_lock(&usingJamsMut);
                    int current_usingJams = usingJams;
                    pthread_mutex_unlock(&usingJamsMut);

                    if (((STUDENTKI - 1) - current_ack_jam >= KONFITURY) && current_usingJams >= KONFITURY) {
                        break;
                    }

                    if (recvPacket(REQ_JAM, &ack_jam, ACK_JAM)) {
                        pthread_mutex_lock(&ackJamMut);
                        ack_jam++;
                        pthread_mutex_unlock(&ackJamMut);
                    }
                }

                while(!isAtQueueTop(JamQueue, rank)){}

                dequeue(JamQueue);

                pthread_mutex_lock(&usingJamsMut);
                usingJams--;
                pthread_mutex_unlock(&usingJamsMut);

                changeState(INSECTION_STUDENTKA);
                break;
            
            
            case INSECTION_STUDENTKA:
                debug("Jem konfitury");
                //eat_jam();
                
                pthread_mutex_lock(&availableJarsMut);
                availableJars += 1;
                pthread_mutex_unlock(&availableJarsMut);

                sendToAll(FRE_RES, priority);
                changeState(INACTIVE_STUDENTKA);
                debug("Zjadłam konfitury");
                break;

            }   
        }


}

