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





void files_init()
{
    int i, j;
    INODE *inodes;
    for(i=0; i<MAX_OPENFILE; i++){
        theDirectory[i].free = true;
        theDirectory[i].filename = NULL;
        inodes = &inodesTbl[i];
        theDirectory[i].inode = inodes;
        inodesTbl[i].inode_id = i;
    };
    for(i=0; i<MAX_DEV; i++){
        Dev_Tbl[i].num_of_free_blocks = MAX_BLOCK;
        for(j=0; j<MAX_BLOCK; j++){
                Dev_Tbl[i].free_blocks[j] = true;
        };
    };
}


void openf(char *filename, OFILE *file)
{
    int j;
    int directoryArray;
    j = search_file(filename);
    if(j=-1){
        new_file(filename);
    }
    file->inode = theDirectory[j].inode;
    (*file).dev_id = file->inode->dev_id;
    (*file).iorb_count=0;
    file->inode->count--;
}



EXIT_CODE closef(OFILE *file)
{
    int i=0;
    INODE *node;
    if(file->iorb_count>0){
        return fail;
    }else{
    file->inode->count--;
    }
    if(file->inode->count = 0){
        for(i=0; i<MAX_OPENFILE; i++){
            node = theDirectory[i].inode;
            if(file->inode->inode_id == node->inode_id){
                delete_file(i);
            }else{
                i++;
            }
        }
    }
    return ok;
}



EXIT_CODE readf(OFILE *file, int position, int page_id, IORB  *iorb)
{
    int logBlk;
    int physBlk;
    PCB *pcb;
    PTBR->pcb = pcb;
    if(position >=0 && position < file->inode->filesize){
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
    }else{
    iorb->dev_id = -1;
    return fail;}

}



EXIT_CODE writef(OFILE *file, int position, int page_id, IORB *iorb)
{
    int pos, logBlk, lastBlk, physBlk;
    EXIT_CODE code;
    PCB *pcb;
    PTBR->pcb = pcb;
    if(position >= 0){
        logBlk = position / PAGE_SIZE;
        if(file->inode->filesize >0){
            lastBlk = (file->inode->filesize-1) / PAGE_SIZE;
        }else{
            lastBlk = -1;}
        if(logBlk > lastBlk){
            allocate_blocks(file->inode, file->inode->filesize);
            if(code = fail){
                iorb->dev_id = -1;
                return fail;}
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
        iorb->event->happened - false;
        Int_Vector.event = iorb->event;
        Int_Vector.iorb = iorb;
        Int_Vector.cause = iosvc;
        gen_int_handler();
        return ok;

    }else{
    iorb->dev_id = -1;
    return fail;}
}



void notify_files(IORB *iorb)
{
    iorb->file->iorb_count--;
}

EXIT_CODE allocate_blocks(INODE *inode, int numBlocksNeeded)
{
    int dev;
    int i=0;
    int j=0;
    int logBlock;
    dev = (*inode).dev_id;
    if(Dev_Tbl[dev].num_of_free_blocks < numBlocksNeeded){
        return fail;
        }
    if((*inode).filesize>0){
        logBlock = ((*inode).filesize-1) / (PAGE_SIZE+1);
        }else{logBlock = 0;}
    while(numBlocksNeeded != 0){
        if(Dev_Tbl[dev].free_blocks[i]=true){
            Dev_Tbl[dev].free_blocks[i]=false;
            Dev_Tbl[dev].num_of_free_blocks--;
            (*inode).allocated_blocks[logBlock]=i;
            logBlock++;
            numBlocksNeeded--;

        }
    }
}



int search_file(char *filename)
{
    int i,j,str;
    i=0;
    j=0;
    for (i=0; i<MAX_OPENFILE; i++){
        if(theDirectory[i].free = false){
                str=strcmp(theDirectory[i].filename, *filename);
        }else{i++;}
        if (str = 0){
            return i;
        }else{
        i++;}
    }
    if(i=MAX_OPENFILE-1 && str !=0){
        return -1;
    }

}



int new_file(char *filename)
{
    int i;
    int j;
    i=0;
    j=0;
    INODE *node;
    for (i=0; i<MAX_OPENFILE; i++){
            printf("I=%d\n", i);
        if(theDirectory[i].free = true){
            theDirectory[i].filename = &filename;
            theDirectory[i].free = false;
            node=theDirectory[i].inode;
            node->filesize = 0;
            node->count = 0;
        if(Dev_Tbl[0].num_of_free_blocks >= Dev_Tbl[1].num_of_free_blocks){
            node->dev_id = 0;
            }else{
            node->dev_id=1;
            }
        for(j=0; j<MAX_BLOCK; j++){
            node->allocated_blocks[j]=-1;
        }
        return i;
        }

    }
    printf("Broken, dude");
    return -1;

}




void delete_file(int dirNum)
{
    int i;
    for(i=0; i<MAX_BLOCK; i++){
        if(inodesTbl[dirNum].allocated_blocks[i] != -1){
            Dev_Tbl[inodesTbl[dirNum].dev_id].free_blocks[i] = true;
            Dev_Tbl[inodesTbl[dirNum].dev_id].num_of_free_blocks++;
            inodesTbl[dirNum].allocated_blocks[i] = -1;
        }else{
            i++;}
    theDirectory[dirNum].filename = NULL;
    theDirectory[dirNum].free = true;
    }
}



/* end of module */
