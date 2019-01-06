#include <stdlib.h>
#include "memory.h"
#include "queue.h"

/*
WATCH OUT IT'S A SNAKE!
            _     _     _     _
           / \   / \   / \   / \
__________/  _\ /  _\ /  _\ /  _\_________________________
_________/  /  /  /  /  /  /______________________________
         | / \   / \   / \   /\   \_____
         |/   \_/   \_/   \_/  \      O \
                                \_______/--<
*/

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                              Module MEMORY                               */
/*                            Internal Routines                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
Queue *current;
char *toString(FRAME *f)
{
    static char result[BUFSIZ];

    // For each from that is printed, print the frame number, pcb id of the
    // process that owns it, the page number of the page it contains,
    // D or C if it is dirty or clean, and the lock count.

    // Also print refbit if we are using Second Chance
    if (f == NULL) sprintf (result, "(null) ");
    else sprintf (result, "Frame %d(pcb-%d,page-%d,%c,lock-%d,refbit-%d) ",
               (f - &Frame_Tbl[0]),
               f->pcb->pcb_id, f->page_id,
               (f->dirty ? 'D' : 'C') , f->lock_count, f->ref_bit);

    return result;
}


void memory_init()
{
//      printf("Memory Init");
        Queue myQueue;
        initQueue(&myQueue);

        int i;
        for(i=0; i<MAX_FRAME; i++){
            Frame_Tbl[i].free = true;
            Frame_Tbl[i].pcb = NULL;
            Frame_Tbl[i].page_id = -1;
            Frame_Tbl[i].dirty = false;
            Frame_Tbl[i].lock_count = 0;
            Frame_Tbl[i].ref_bit = 0;
        } current = NULL;
}




void prepage(PCB *pcb)
{
        return;
}


int start_cost(PCB *pcb)
{
        return 0;
}



void deallocate(PCB *pcb)
{
        if(current == NULL){
                return;
        }

        printf("deallocate");
        int i;
        for (i=0; i < MAX_PAGE; ++i){
                PAGE_ENTRY *page = &(pcb->page_tbl->page_entry[i]);
                page->valid = false;
                FRAME *frame = &(Frame_Tbl[page->frame_id]);
                frame->free = true;
                frame->pcb = NULL;
                frame->page_id = -1;
                frame->dirty = false;
                Queue *p = current;
                deQueue(p);
        }


}





void get_page(PCB *pcb, int page_id)
{
        int freeFrames = MAX_FRAME;

      if(current !=NULL){
                Queue *p = current;
                do{
                --freeFrames;
                advance(p);}while(p !=current);

        if(freeFrames < 4){
        //do nothing on empty queue
                if(current == NULL){
                        return;}
        //let's find a victim
                int i = 1;
                int empty = 0;
                Queue *currentFrame = current;
                while(empty < 1){
                        FRAME *queueData = currentFrame->current->data;
               //       FRAME *frame = &(Frame_Tbl[page->frame_id]);
                        if(queueData->lock_count == 0){
                                if(queueData->dirty == true){
                                queueData->lock_count == 1;
                                siodrum(write, queueData->pcb, queueData->page_id, 0);
                                queueData->dirty = false;
                                &(pcb->page_tbl->page_entry[page_id])->valid = false;
                                }

                                queueData->free = true;
                                queueData->pcb = NULL;
                                queueData->page_id = -1;
                                ++empty;
                                deQueue(current);
                        }
                        advance(current);
                }

        }


        int free_frame = -1;
        int i=0;



        for(i=0; i<MAX_FRAME; ++i){
            FRAME *frame = &(Frame_Tbl[i]);
            if(frame->free = true){
                free_frame = i;
                frame->free = false;
                frame->pcb = pcb;
                frame->page_id = page_id;
                frame->lock_count =1;
                break;
            }
        }

        siodrum(read, pcb, page_id, free_frame);
        page->frame_id = free_frame;
        page->valid = true;
        FRAME *frame = &(Frame_Tbl[page->frame_id]);
        enQueue(current, frame);
        }
}

void lock_page(IORB *iorb)
{
printf("lock page");
int pageID, frameID;
        PAGE_TBL *page_tbl_lck;

        page_tbl_lck = iorb->pcb->page_tbl;
        pageID = iorb->page_id;

        if(page_tbl_lck->page_entry[pageID].valid == false){
            Int_Vector.pcb = iorb->pcb;
            Int_Vector.page_id = pageID;
            Int_Vector.cause = pagefault;
            gen_int_handler();
        }

        frameID = page_tbl_lck->page_entry[pageID].frame_id;

        if(iorb->action == read)
            Frame_Tbl[frameID].dirty = true;
        Frame_Tbl[frameID].lock_count ++;
}





void unlock_page(IORB  *iorb)
{
printf("unlock page");
 int pageID;
    int frameID;
    PAGE_TBL *page_tbl_unl;

    page_tbl_unl = iorb->pcb->page_tbl;
    pageID = iorb->page_id;
    frameID = page_tbl_unl->page_entry[pageID].frame_id;

    Frame_Tbl[frameID].lock_count--;
}





void reference(int logic_addr, REFER_ACTION action)
{
if (______trace_switch)
                    printf("CLOCK> %6d# MEMORY: refer(addr=%d,action=%d), Welcome to Reference\n",
                        get_clock(),logic_addr, action);

int jobSize, pageNumber, frame_id;
        PCB *pcb;
        PAGE_TBL *page_tbl_ref;
        pcb= PTBR->pcb;

        jobSize = pcb->size;
        page_tbl_ref = pcb->page_tbl;

        if(logic_addr < MAX_PAGE*PAGE_SIZE && logic_addr < jobSize && logic_addr >=0){
            pageNumber = logic_addr / PAGE_SIZE;
            if(page_tbl_ref->page_entry[pageNumber].valid == false){
                Int_Vector.cause = pagefault;
                Int_Vector.pcb = pcb;
                Int_Vector.page_id = pageNumber;
                gen_int_handler();
            }
        }

        if((page_tbl_ref->page_entry[pageNumber].valid == true) && (action == store)){
            frame_id = page_tbl_ref->page_entry[pageNumber].frame_id;
            Frame_Tbl[frame_id].dirty = true;
            memoryAccess(action, Frame_Tbl[frame_id], logic_addr % PAGE_SIZE);
        }

}


