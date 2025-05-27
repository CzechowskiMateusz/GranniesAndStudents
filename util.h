#ifndef UTILH
#define UTILH
#include "main.h"

/* typ pakietu */
typedef struct {
    int ts;       
    int src;    
} packet_t;
#define NITEMS 2

/* Typy wiadomości */
#define REQ_JAR 1
#define ACK_JAR 2
#define REQ_JAM 3
#define ACK_JAM 4
#define NEW_JAM 5
#define FRE_RES 6

/* Kolejka nierozroznialna */
struct kolejka{
    packet_t *pkt;
    struct kolejka *next;
};

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();
void addsToQueue(int tag, int priority, struct kolejka **queue);
void dequeue(struct kolejka **queue);
int isAtQueueTop(struct kolejka *queue, int rank);
void make_jam();
void eat_jam();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag);
void sendToAll(int tag, int priority);
void sendAck( int tag, int destination);
int recvPacket(int expectedTag, int *ack, int expectedAckTag);

#endif
