#include "main.h"
#include "util.h"
MPI_Datatype MPI_PAKIET_T;

// Tag komunikatu
struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = { 
    { "REQ_JAR", REQ_JAR}, 
    {"ACK_JAR", ACK_JAR}, 
    {"REQ_JAM", REQ_JAM}, 
    {"ACK_JAM", ACK_JAM}, 
    {"NEW_JAM", NEW_JAM}, 
    {"FRE_RES", FRE_RES}
};

const char const *tag2string( int tag ){
    for (int i=0; i <sizeof(tagNames)/sizeof(struct tagNames_t);i++) {
	    if ( tagNames[i].tag == tag )  
            return tagNames[i].name;
    }

    return "<unknown>";
}

// Typ Pakietu
void inicjuj_typ_pakietu(){
    int       blocklengths[NITEMS] = {1,1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT};

    MPI_Aint     offsets[NITEMS]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
}

void changeState( state_t newState ){
    pthread_mutex_lock( &stateMut );
    stan = newState;
    pthread_mutex_unlock( &stateMut );
}

void sendPacket(packet_t *pkt, int destination, int tag){
    int freepkt=0;
    if (pkt==0) { 
        pkt = malloc(sizeof(packet_t)); 
        freepkt=1;
    }

    pkt->src = rank;

    MPI_Send( pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d\n", tag2string( tag), destination);
    if (freepkt) free(pkt);
}

void sendToAll(int tag, int priority){
    packet_t pkt;
    pkt.ts = clockLamp;
    pkt.src = rank;

    for (int i=0; i<size; i++) { 
        if (i != rank) 
            sendPacket(&pkt, i, tag);
    }
}

void sendAck(int destination, int tag){
    packet_t pkt;
    pkt.ts = clockLamp;
    pkt.src = rank;

    debug("Wysyłam ACK do %d", destination);
    sendPacket(&pkt, destination, tag);
}

int recvPacket(int expectedTag, int *ack, int expectedAckTag) {
    MPI_Status status;
    int flag = 0;

    MPI_Iprobe(MPI_ANY_SOURCE, expectedTag, MPI_COMM_WORLD, &flag, &status);

    if (flag) {
        packet_t pkt;
        MPI_Recv(&pkt, 1, MPI_PAKIET_T, status.MPI_SOURCE, expectedTag, MPI_COMM_WORLD, &status);

        if (expectedTag == expectedAckTag) {
            (*ack)++;
            return TRUE;
        } 
    }

    return FALSE;
}


void addsToQueue(int tag, int priority, struct kolejka **queue){
    if (queue == NULL) return;

    packet_t *pkt = malloc(sizeof(packet_t));
    if (pkt == NULL) return;

    pkt->ts = priority;
    pkt->src = rank;

    struct kolejka *newNode = malloc(sizeof(struct kolejka));
    if (newNode == NULL) {
        free(pkt);
        return;
    }

    newNode->pkt = pkt;
    newNode->next = NULL;

    if (*queue == NULL) {
        *queue = newNode;
        return;
    }

    if (pkt->ts < (*queue)->pkt->ts || 
        (pkt->ts == (*queue)->pkt->ts && pkt->src < (*queue)->pkt->src)) {
        newNode->next = *queue;
        *queue = newNode;
        return;
    }

    struct kolejka *current = *queue;
    while (current->next != NULL && 
           (current->next->pkt->ts < pkt->ts || 
            (current->next->pkt->ts == pkt->ts && current->next->pkt->src < pkt->src))) {
        current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
}

void dequeue(struct kolejka **queue){
    if (*queue == NULL) return;

    struct kolejka *temp = *queue;
    *queue = (*queue)->next;
    free(temp->pkt);
    free(temp);
}

int isAtQueueTop(struct kolejka *queue, int rank){
    if(queue == NULL) 
        return 0;
    return (queue->pkt->src == rank);
}

void make_jam(){
    sleep(1);
}

void eat_jam(){
    sleep(1); 
}
