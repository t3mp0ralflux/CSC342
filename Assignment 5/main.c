#include "queue.h"
#include "devices.h"
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                              Module DEVICES                              */
/*                             Internal Routines                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
Queue *myQueue[MAX_DEV];
int Direction[MAX_DEV];
//direction: 0 = left, 1 = right
//track number = block_id*PAGE_SIZE / TRACK_SIZE
//queue must be sorted by TRACK NUMBER ^

int compareTo(IORB *iorb1, IORB *iorb2)
{
    int iorb1Track, iorb2Track;
    iorb1Track = (iorb1->block_id)*PAGE_SIZE/TRACK_SIZE;
    iorb2Track = (iorb2->block_id)*PAGE_SIZE/TRACK_SIZE;
    return iorb1Track-iorb2Track;
}

void devices_init()
{
        int i;
        for(i=0; i<MAX_DEV; i++){
            initQueue(myQueue[i]);
            Dev_Tbl[i].iorb = NULL;
            Dev_Tbl[i].busy = false;
            Direction[i]=1;
        }
}

void enq_io(iorb)
IORB *iorb;
{
    enQueueSorted(myQueue[iorb->dev_id], iorb, compareTo);
    if(Dev_Tbl[iorb->dev_id].busy == false){
        lock_page(iorb);
        Dev_Tbl[iorb->dev_id].iorb = iorb;
        Dev_Tbl[iorb->dev_id].busy = true;
        siodev(iorb);
    }
}

void deq_io(iorb)
IORB *iorb;
{
    QueueNode *oldNode;
    IORB *iorb2;
    int deviceID = iorb->dev_id;
    oldNode = findNode(myQueue[deviceID], iorb, compareTo);
    unlock_page(iorb);
    notify_files(iorb);
    if(Direction[deviceID] == 1){
        advance(myQueue[deviceID]);
    }else{
        retreat(myQueue[deviceID]);
    }
    removeNode(myQueue, oldNode);
    if(isPastEnd(myQueue[deviceID])){
        Direction[deviceID] = !Direction[deviceID];
    }
    oldNode = getCurrentNode(myQueue[deviceID]);
    if(oldNode == NULL){
        Dev_Tbl[deviceID].busy = false;
        Dev_Tbl[deviceID].iorb = NULL;
    }else{
    iorb2 = getCurrentValue(myQueue[deviceID]);
    Dev_Tbl[deviceID].iorb = iorb2;
    Dev_Tbl[deviceID].busy = true;
    siodev(iorb2);
    }
}

void purge_iorbs(pcb)
PCB *pcb;
{
    QueueNode *node1, *node2;
    IORB *iorb;
    int i;
    for(i=0; i<MAX_DEV; i++){
        setCurrent(myQueue[i], backNode(myQueue[i]));
        while(!isPastEnd(myQueue[i])){
            node1 = getCurrentNode(myQueue[i]);
            iorb = getCurrentValue(myQueue[i]);
            if(iorb->pcb == pcb && Dev_Tbl[i].busy == false){
                advance(myQueue[i]);
                node2 = getCurrentNode(myQueue[i]);
                notify_files(iorb);
                removeNode(myQueue, node1);
            }else{
            advance(myQueue[i]);
            }
        }
    }
}
