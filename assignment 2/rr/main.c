#include "cpu.h"
#include "queue.h"
#include <stdlib.h>
#include <math.h>

int compareTo (PCB *pcb1, PCB #pcb2);
char *toSTring(PCB *pcb);

//I really have no idea how to start.  I guess I need partners next time...
void cpu_init()
{
    Queue myQueue;
    initQueue (&myQueue);
}

void disaptch()
{
    PCB* pcb_id = NULL;
    if(PTBR != NULL){
        pcb_id = PTBR -> pcb;
    }

    if(pcb_id != NULL && pcb -> status = running){
            insert_ready(pcb_id);
    }
}

void insert_ready(PCB *pcb)
{
    enQueue

}

char *toString(PCB *pcb)
{
    static char result[BUFFSIZ];
    sprintf (result, "pcb-%d(prior-%d) ", pcb -> pcb_id, pcb -> priority);
    return result;
}

int compareTo(PCB *pcb1, PCB *pcb2)
{
    return pcb -> pcb_id - pcb2 -> pcb_id;
}

/* end of module */
