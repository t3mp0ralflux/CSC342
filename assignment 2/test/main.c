/****************************************************************************/
/*                                                                          */
/*           Module MEMORY                                  */
/*      External Declarations                               */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>


/* OSP constants */

#define MAX_PAGE       16                 /* max size of page tables        */
#define MAX_FRAME      32                 /* size of the physical memory    */
#define PAGE_SIZE      512                /* size of a page in bytes        */

#define   COST_OF_PAGE_TRANSFER      6  /* cost of reading page  from drum  */


/* external enumeration constants */

typedef enum {
    false, true                         /* the boolean data type            */
} BOOL;

typedef enum {
    read, write                         /* type of actions for I/O requests */
} IO_ACTION;

typedef enum {
    load, store                         /* types of memory reference        */
} REFER_ACTION;

typedef enum {
    running, ready, waiting, done       /* types of status                  */
} STATUS;

typedef enum {
    iosvc, devint,                      /* types of interrupt               */
    pagefault, startsvc,
    termsvc, killsvc,
    waitsvc, sigsvc, timeint
} INT_TYPE;



/* external type definitions */

typedef struct page_entry_node PAGE_ENTRY;
typedef struct page_tbl_node PAGE_TBL;
typedef struct event_node EVENT;
typedef struct ofile_node OFILE;
typedef struct pcb_node PCB;
typedef struct iorb_node IORB;
typedef struct int_vector_node INT_VECTOR;
typedef struct frame_node FRAME;



/* external data structures */

struct frame_node {
    BOOL   free;        /* = true, if free                                  */
    PCB    *pcb;        /* process to which the frame currently belongs     */
    int    page_id;     /* vitrual page id - an index to the PCB's page tbl */
    BOOL   dirty;       /* indicates if the frame has been modified         */
    int    lock_count;  /* number of locks set on page involved in an       */
                        /* active I/O                                       */
    int    *hook;       /* can hook up anything here                        */
};

struct page_entry_node {
    int    frame_id;    /* frame id holding this page                       */
    BOOL   valid;       /* page in main memory : valid = true; not : false  */
    BOOL   ref;         /* set to true every time page is referenced AD     */
    int    *hook;       /* can hook up anything here                        */
};

struct page_tbl_node {
    PCB    *pcb;        /* PCB of the process in question                   */
    PAGE_ENTRY page_entry[MAX_PAGE];
    int    *hook;       /* can hook up anything here                        */
};

struct pcb_node {
    int    pcb_id;         /* PCB id                                        */
    int    size;           /* process size in bytes; assigned by SIMCORE    */
    int    creation_time;  /* assigned by SIMCORE                           */
    int    last_dispatch;  /* last time the process was dispatched          */
    int    last_cpuburst;  /* length of the previous cpu burst              */
    int    accumulated_cpu;/* accumulated CPU time                          */
    PAGE_TBL *page_tbl;    /* page table associated with the PCB            */
    STATUS status;         /* status of process                             */
    EVENT  *event;         /* event upon which process may be suspended     */
    int    priority;       /* user-defined priority; used for scheduling    */
    PCB    *next;          /* next pcb in whatever queue                    */
    PCB    *prev;          /* previous pcb in whatever queue                */
    int    *hook;          /* can hook up anything here                     */
};

struct iorb_node {
    int    iorb_id;     /* iorb id                                          */
    int    dev_id;      /* associated device; index into the device table   */
    IO_ACTION action;   /* read/write                                       */
    int    block_id;    /* block involved in the I/O                        */
    int    page_id;     /* buffer page in the main memory                   */
    PCB    *pcb;        /* PCB of the process that issued the request       */
    EVENT  *event;      /* event used to synchronize processes with I/O     */
    OFILE  *file;       /* associated entry in the open files table         */
    IORB   *next;       /* next iorb in the device queue                    */
    IORB   *prev;       /* previous iorb in the device queue                */
    int    *hook;       /* can hook up anything here                        */
};

struct int_vector_node {
    INT_TYPE cause;           /* cause of interrupt                         */
    PCB    *pcb;              /* PCB to be started (if startsvc) or pcb that*/
            /* caused page fault (if fagefault interrupt) */
    int    page_id;           /* page causing pagefault                     */
    int    dev_id;            /* device causing devint                      */
    EVENT  *event;            /* event involved in waitsvc and sigsvc calls */
    IORB   *iorb;             /* IORB involved in iosvc call                */
};



/* extern variables */

extern INT_VECTOR Int_Vector;           /* interrupt vector                */
extern PAGE_TBL *PTBR;                  /* page table base register        */
extern FRAME Frame_Tbl[MAX_FRAME];      /* frame table                     */
extern int Prepage_Degree;    /* global degree of prepaging (0-10) */



/* external routines */

extern siodrum(/* action, pcb, page_id, frame_id */);
/*  IO_ACTION   action;
    PCB         *pcb;
    int         page_id, frame_id;  */
extern int get_clock();
extern gen_int_handler();


/****************************************************************************/
/*                                                                          */
/*           Module MEMORY                                  */
/*      Internal Declarations                               */
/*                                                                          */
/****************************************************************************/

#define   PRIVATE         static
#define   PUBLIC
#define   TRUE            1
#define   FALSE           0
//#define   NULL            0      /*  NULL pointer   */
#define   UNLOCKED        0
#define   MAX_SIZE      MAX_PAGE*PAGE_SIZE /* max size of a job allowed     */


#define get_page_tbl(pcb)   pcb->page_tbl

#define lock_frame(frame_id)    Frame_Tbl[frame_id].lock_count++
#define unlock_frame(frame_id)    Frame_Tbl[frame_id].lock_count--
#define set_frame_dirty(frame_id) Frame_Tbl[frame_id].dirty = true


/* external variables */

static int trace = FALSE; /* Internal trace flag */
static int clock_hand = 0;



#define MIN_FREE 4
#define LOTS_FREE 1

typedef struct list_item_struct LIST_ITEM;
struct list_item_struct {
  PAGE_ENTRY *page;
  LIST_ITEM *next;
  LIST_ITEM *prev;
};

LIST_ITEM *current;

PRIVATE void list_insert(PAGE_ENTRY*);
PRIVATE void list_remove();
PRIVATE void put_into_frame(PCB *pcb, PAGE_ENTRY *page, int page_id);
PRIVATE void page_demon();

/**************************************************************************/
/*                    */
/*      memory_init()           */
/*                    */
/*   Description  : initialize the data structure in memory module        */
/*                    */
/*   called by    : SIMCORE module            */
/*                    */
/**************************************************************************/
PUBLIC
memory_init()
{
  current = NULL;
}


/**************************************************************************/
/*                    */
/*      get_page            */
/*                    */
/*  Description : To  transfer a page from drum to main memory        */
/*                        */
/*      Called by   : PAGEINT module            */
/*                    */
/*                    */
/**************************************************************************/
PUBLIC
get_page(pcb,page_id)
PCB   *pcb;
int    page_id;

{
  int free_frames = MAX_FRAME;
  if (current != NULL) {
    LIST_ITEM *p = current;
    do {
      --free_frames;
      p = p->next;
    } while (p != current);
    fprintf(stderr,"freefream %d\n",free_frames);
  }

  if (free_frames < MIN_FREE) {
    page_demon();
  }

  PAGE_ENTRY *page = &(pcb->page_tbl->page_entry[page_id]);
  list_insert(page);

  put_into_frame(pcb, page, page_id);
}

PRIVATE
void put_into_frame(PCB *pcb, PAGE_ENTRY *page, int page_id) {
  int free_frame = -1;
  int i = 0;

  for (i = 0; i < MAX_FRAME; ++i) {
    FRAME *frame = &(Frame_Tbl[i]);
    if (frame->free) {
      free_frame = i;
      frame->free = false;
      frame->pcb = pcb;
      frame->page_id = page_id;
      break;
    }
  }

  siodrum(read, pcb, page_id, free_frame);
  page->frame_id = free_frame;
  page->valid = true;
  page->ref = true;
}

PRIVATE
void page_demon() {
fprintf(stderr,"PaGE d\n");
  // Do nothing with an empty list.
  if (current == NULL) {
    return;
  }

  int i = LOTS_FREE;
  int removed = 0;

  LIST_ITEM *p = current;
  while (removed < LOTS_FREE) {
    PAGE_ENTRY *page = p->page;

    if (page->ref) {
      page->ref = false;
      current = current->next;
    } else {
      FRAME *frame = &(Frame_Tbl[page->frame_id]);
      if (frame->lock_count == 0) {
        // Copy frame to memory.
fprintf(stderr,"Before:%p\n", frame);
        if (frame->dirty) {
          siodrum(write, frame->pcb, frame->page_id, 0);
          frame->dirty = false;
        }
fprintf(stderr,"After:%p\n", frame);

        // Make frame free for other processes.
        page->valid = false;
        frame->free = true;
        frame->pcb = NULL;
        frame->page_id = -1;

        ++removed;
        list_remove();
      }
    }

    p = p->next;
  }
  print_page_tbl();
}

PRIVATE
void list_insert(PAGE_ENTRY *page) {
  LIST_ITEM *item = (LIST_ITEM*) malloc(sizeof(LIST_ITEM));
  item->page = page;
  item->next = NULL;
  item->prev = NULL;

  if (current == NULL) {
    item->next = item;
    item->prev = item;
    current = item;
  } else {
    LIST_ITEM *ahead = current->next;
    LIST_ITEM *behind = current;

    item->next = ahead;
    item->prev = behind;

    ahead->prev = item;
    behind->next = item;

    current = item;
  }
}


PRIVATE
void list_remove() {
  if (current == NULL) {
    return;
  } else if (current->next == current) {
    free(current);
    current = NULL;
  } else {
    LIST_ITEM *ahead = current->next;
    LIST_ITEM *behind = current->prev;

    behind->next = ahead;
    ahead->prev = behind;
    free(current);

    current = ahead;
  }
}

/**************************************************************************/
/*                    */
/*                  deallocate              */
/*                    */
/*   Description : The job is history now so free the memory frames       */
/*                 occupied by the process.                     */
/*                 Set the pcb to NULL                                    */
/*                            */
/*   called by   : PROCSVC module                                         */
/*                    */
/**************************************************************************/
PUBLIC
deallocate(pcb)
PCB *pcb;
{
  if( current == NULL ){
    return;
  }

  int i;
  for( i=0; i < MAX_PAGE; ++i )
  {
    PAGE_ENTRY* page = &(pcb->page_tbl->page_entry[i]);
    if( !page->valid ){
      continue;
    }
    page->valid = false;
    page->ref = false;

    FRAME *frame = &(Frame_Tbl[page->frame_id]);
    frame->free = true;
    frame->pcb = NULL;
    frame->page_id = -1;
    frame->dirty = false;

    // Removing page form the list
    LIST_ITEM* p = current;
    if (current->next == current) {
      free(current);
      current = NULL;
      return;
    } else {
      do {
        if (p->page == page) {
          LIST_ITEM *ahead = p->next;
          LIST_ITEM *behind = p->prev;

          behind->next = ahead;
          ahead->prev = behind;
          free(p);

          if (p == current) {
            current = ahead;
          }
        }
        p = p->next;
      } while ( p != current );

    }
  }
}


/************************************************************************/
/*                  */
/*       prepage            */
/*                  */
/*    Description : Swap the process specified in the argument from     */
/*                  drum/disk to main memory.                           */
/*              Will  use the prepaging policy.                     */
/*                    */
/*   called by    : CPU module            */
/*                  */
/************************************************************************/
PUBLIC
prepage(pcb)
PCB *pcb;
{
  /* Not part of lab. Leave empty  */
}

/************************************************************************/
/*                  */
/*       start_cost           */
/*                  */
/*   called by    : CPU module            */
/*                  */
/************************************************************************/
PUBLIC
int start_cost(pcb)
PCB *pcb;
{
  /* Not part of lab. Leave empty  */
}


/***************************************************************************/
/*                     */
/*        refer                    */
/*                     */
/*  Description : Called by SIMCORE module to simulate memory          */
/*          referencing by processes.                            */
/*                     */
/*      Called by   : SIMCORE module             */
/*                     */
/*  Call        : gen_int_handler()                  */
/*                    */
/*   You are not expected to change this routine                            */
/*                     */
/***************************************************************************/
PUBLIC
refer(logic_addr,action)
int          logic_addr;   /* logical address                              */
REFER_ACTION action;       /* a store operation will change memory content */
{
  int            job_size,
                 page_no,
                 frame_id;
  PAGE_TBL       *page_tbl_ptr;
  PCB            *pcb;

  if (PTBR != NULL)
    check_page_table(PTBR,1,"MEMORY.refer","","upon entering routine");

  pcb = PTBR->pcb;

  if (trace)
    printf("Hello refer(pcb: %d,logic_addr: %d,action: %d)\n",
          pcb->pcb_id,logic_addr,action);


  job_size = pcb->size;
  page_tbl_ptr = get_page_tbl(pcb); /* macro */

  if (logic_addr < MAX_SIZE && logic_addr < job_size && logic_addr >= 0){

       page_no = logic_addr / PAGE_SIZE;

       if (page_tbl_ptr->page_entry[page_no].valid == false) {
          /* page fault                                             */
          /* set interrupt vector Int_Vector to indicate page fault */
          /* call interrupt handler                                 */

          Int_Vector.pcb = pcb;
          Int_Vector.page_id = page_no;
          Int_Vector.cause   = pagefault;
          gen_int_handler();
       }

       page_tbl_ptr->page_entry[page_no].ref = true;

       if (( page_tbl_ptr->page_entry[page_no].valid == true) &&
                 (action == store)) {

         frame_id = page_tbl_ptr->page_entry[page_no].frame_id;
         set_frame_dirty(frame_id); /* macro */

       }

  }
  else {
    printf("CLOCK> %6d#*** ERROR: MEMORY.refer>\n\t\tPCB %d: Invalid address passed to refer(%d, ...)\n\n\n",
        get_clock(),pcb->pcb_id,logic_addr);
    print_sim_frame_tbl();
    osp_abort();
  }

}


/*************************************************************************/
/*                   */
/*      lock_page          */
/*                   */
/*  Description:   To lock the chunk of memory mentioned in iorb     */
/*           to protect it from being swapped out.             */
/*                   */
/*  Called by  :   DEVICES module          */
/*                                                                       */
/*      call       :   gen_int_handler in INTER module                   */
/*                     lock_frame                                        */
/*                    */
/*   You are not expected to change this routine                            */
/*                   */
/*************************************************************************/
PUBLIC
lock_page(iorb)
IORB *iorb;
{
   int           page_id ;
   int           frame_id;
   PAGE_TBL      *page_tbl_ptr;

    if (trace)
        printf("Hello lock_page(iorb). The pcb is %d\n",iorb->pcb->pcb_id);

    check_iorb(iorb,1,"MEMORY.lock_page","","upon entering routine");

    page_tbl_ptr = get_page_tbl(iorb->pcb); /* macro */
    page_id      =  iorb->page_id;

    if (page_tbl_ptr->page_entry[page_id].valid == false) {

          Int_Vector.pcb = iorb->pcb;
          Int_Vector.page_id = page_id;
          Int_Vector.cause   = pagefault;
          gen_int_handler();
    }

    frame_id = page_tbl_ptr->page_entry[page_id].frame_id;


    if (iorb->action == read)
        set_frame_dirty(frame_id);  /* macro */
    lock_frame(frame_id); /* macro */
 }


/*************************************************************************/
/*                                                                       */
/*              unlock_page()                                            */
/*                                                                       */
/*          description : Unlocked the page which has finished I/O       */
/*                                                                       */
/*          Called by   : DEVICES module                                 */
/*                                                                       */
/*          Call        : unlock_frame                                   */
/*                    */
/*   You are not expected to change this routine                            */
/*                                                                       */
/*************************************************************************/
PUBLIC
unlock_page(iorb)
IORB *iorb;
{
   int              page_id ;
   int              frame_id;
   PAGE_TBL        *page_tbl_ptr;

   if (trace)
       printf("Hello unlock_page(iorb). The pcb is %d\n",iorb->pcb->pcb_id);

   check_iorb(iorb,1,"MEMORY.unlock_page","","upon entering routine");

   page_tbl_ptr = get_page_tbl(iorb->pcb);  /* macro */
   page_id      = iorb->page_id;
   frame_id = page_tbl_ptr->page_entry[page_id].frame_id;


   unlock_frame(frame_id);  /* macro */

}

/****************************************************************************/
/*                      */
/*      print_page_tbl              */
/*                      */
/*  Description : Print the page table of a process.        */
/*                    For debugging purpose                                 */
/*                      */
/****************************************************************************/


print_page_tbl(page_tbl_ptr)
PAGE_TBL   *page_tbl_ptr;
{
  int i;

  printf("\n\n");
  for (i=0; i<MAX_PAGE; i++)
        printf("pg=%d    valid=%d  ref=%d  frame=%d\n",
                i, page_tbl_ptr->page_entry[i].valid,
                page_tbl_ptr->page_entry[i].ref,
                page_tbl_ptr->page_entry[i].frame_id);

  printf("\n\n");
}
