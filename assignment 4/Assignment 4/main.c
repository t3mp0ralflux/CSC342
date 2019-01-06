#include <stdio.h>
#include "files.h"

/****************************************************************************/
/*                                                                          */
/*                           Module FILES                                   */
/*                      External Declarations                               */
/*                                                                          */
/****************************************************************************/




/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                              Module FILESYS                              */
/*                             Internal Routines                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/*
                               |
                          --====|====--
                                |

                            .-"""""-.
                          .'_________'.
                         /_/_|__|__|_\_\
                        ;'-._       _.-';
   ,--------------------|    `-. .-'    |--------------------,
    ``""--..__    ___   ;       '       ;   ___    __..--""``
              `"-// \\.._\             /_..// \\-"`
                 \\_//    '._       _.'    \\_//
                  `"`        ``---``        `"`
WATCH OUT OR UNITED WILL BEAT YOU UP!!!
*/

void files_init()
{
        int i, j;
        for(i=0; i<MAX_OPENFILE; i++){
                theDirectory[i].free = true;
                theDirectory[i].filename = NULL;
                theDirectory[i].inode = &inodesTbl[i];
                inodesTbl[i].inode_id = i;
        }
        for(i=0; i<MAX_DEV; i++){
                Dev_Tbl[i].num_of_free_blocks = MAX_BLOCK;
                for(j=0; j<MAX_BLOCK; j++){
                        Dev_Tbl[i].free_blocks[j] = true;
                }
        }
}


void openf(char *filename, OFILE *file)
{
        int j;
        j = search_file(filename);
        if(j==-1){
                j=new_file(filename);
        }
        file->inode = &inodesTbl[j];
        file->dev_id = file->inode->dev_id;
        file->iorb_count=0;
        file->inode->count++;
}



EXIT_CODE closef(OFILE *file)
{
        int i=0;
        INODE *node;
        if(file->iorb_count>0){
                return fail;
        }
        file->inode->count--;
        if(file->inode->count = 0){
                delete_file(file->inode->inode_id);
        }
        return ok;
}



EXIT_CODE readf(OFILE *file, int position, int page_id, IORB  *iorb)
{
        int logBlk;
        int physBlk;
        PCB *pcb = PTBR->pcb;
        if(0< position && position < file->inode->filesize){
                logBlk = position / PAGE_SIZE;
                physBlk = file->inode->allocated_blocks[logBlk];
                file->iorb_count++;
                iorb->dev_id = file->inode->dev_id;
                iorb->block_id = physBlk;
                iorb->action = read;
                iorb->page_id = page_id;
                iorb->pcb = pcb;
                iorb->file = file;
                iorb->event->happened = false;
                Int_Vector.event = iorb->event;
                Int_Vector.iorb = iorb;
                Int_Vector.cause = iosvc;
                gen_int_handler();
                return ok;

        }
        if(0>=position || position > file->inode->filesize){
                iorb->dev_id = -1;
                return fail;
        }
}



EXIT_CODE writef(OFILE *file, int position, int page_id, IORB *iorb)
{
        int numBlockNeed, logBlk, lastBlk, physBlk;
        PCB *pcb = PTBR->pcb;
        if(position <= 0){
                iorb->dev_id = -1;
                return fail;
        }
        logBlk = position / PAGE_SIZE;
        if(!file->inode->filesize){
                lastBlk = -1;
        }else{
                lastBlk = (file->inode->filesize-1) / PAGE_SIZE;
        }
        if(logBlk > lastBlk){
                numBlockNeed = logBlk - lastBlk;
                if (numBlockNeed > Dev_Tbl[file->inode->dev_id].num_of_free_blocks){
                        iorb->dev_id = -1;
                        return fail;
                }
        allocate_blocks(file->inode, numBlockNeed);
        }
        if(file->inode->filesize <= position){
                file->inode->filesize = position+1;
        }
        physBlk = file->inode->allocated_blocks[logBlk];
        file->iorb_count++;
        iorb->dev_id = file->inode->dev_id;
        iorb->block_id = physBlk;
        iorb->action = write;
        iorb->page_id = page_id;
        iorb->pcb = pcb;
        iorb->file = file;
        iorb->event->happened = false;
        Int_Vector.event = iorb->event;
        Int_Vector.iorb = iorb;
        Int_Vector.cause = iosvc;
        gen_int_handler();
        return ok;
}



void notify_files(IORB *iorb)
{
        iorb->file->iorb_count--;
}

EXIT_CODE allocate_blocks(INODE *inode, int numBlocksNeeded)
{
        int i;
        int logBlock=0;
        int block=0;
        if(Dev_Tbl[inode->dev_id].num_of_free_blocks -  numBlocksNeeded <0){
                return fail;
        }
        if(inode->filesize>0){
                logBlock = (inode->filesize-1) / PAGE_SIZE+1;
        }else{
                logBlock = 0;
        }
        while(block<numBlocksNeeded){
                for(i=0; i<MAX_BLOCK; i++){
                        if(Dev_Tbl[inode->dev_id].free_blocks[i]){
                                Dev_Tbl[inode->dev_id].free_blocks[i]=false;
                                Dev_Tbl[inode->dev_id].num_of_free_blocks--;
                                inode->allocated_blocks[logBlock]=i;
                                logBlock++;
                                block++;
                                break;
                        }

                }
        }
        return ok;
}



int search_file(char *filename)
{
        int i;
        for (i=0; i<MAX_OPENFILE; i++){
                if(theDirectory[i].filename != NULL){
                        if(strcmp(filename, theDirectory[i].filename)==0 && !theDirectory[i].free){
                                return i;
                        }
                }
        }
        i=-1;
        return i;
    }


}



int new_file(char *filename)
{
        int i;
        int j;
        int dev;
        int freespace=0;
        for (i=0; i<MAX_OPENFILE; i++){
                if(theDirectory[i].free){
                        theDirectory[i].filename = filename;
                        theDirectory[i].free = false;
                        theDirectory[i].inode->filesize=0;
                        theDirectory[i].inode->count=0;
                        for(j=0; j<MAX_DEV; j++){
                                if(Dev_Tbl[j].num_of_free_blocks >= freespace){
                                        dev = j;
                                        freespace = Dev_Tbl[j].num_of_free_blocks;
                                }
                        }
                        theDirectory[i].inode->dev_id = dev;
                        for(j=0; j<MAX_BLOCK; j++){
                                theDirectory[i].inode->allocated_blocks[j]=-1;
                        }
                return i;
                }
        }
        printf("404: Free Space Not Found");
        i = -1;
        return i;
}




void delete_file(int dirNum)
{
        int i;
        for(i=0; i<MAX_BLOCK; i++){
                if(theDirectory[dirNum].inode->allocated_blocks[i]!=-1){
                        int physBlock = theDirectory[dirNum].inode->allocated_blocks[i];
                        Dev_Tbl[theDirectory[dirNum].inode->dev_id].free_blocks[physBlock]=true;
                        Dev_Tbl[theDirectory[dirNum].inode->dev_id].num_of_free_blocks++;
                        theDirectory[dirNum].inode->allocated_blocks[i]=-1;
                }
        }
        theDirectory[dirNum].free = true;
        theDirectory[dirNum].filename = NULL;
}



/* end of module */
