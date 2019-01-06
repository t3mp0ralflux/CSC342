#include <stdio.h>

/****************************************************************************/
/*                                                                          */
/* 			     Module CPU                                     */
/* 			External Declarations 				    */
/*                                                                          */
/****************************************************************************/


/* OSP constant      */

#define   MAX_PAGE                   16 /* max size of page tables          */

/* OSP enumeration constants */

typedef enum {
    false, true                         /* the boolean data type            */
} BOOL;

typedef enum {
    running, ready, waiting, done       /* types of status                  */
} STATUS;



/* external type definitions */

typedef struct page_entry_node PAGE_ENTRY;
typedef struct page_tbl_node PAGE_TBL;
typedef struct event_node EVENT;
typedef struct pcb_node PCB;

/* external data structures */

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


/* external variables */

extern PAGE_TBL *PTBR;		/* page table base register */

extern int    Quantum;		/* global time quantum; contains the value
				   entered at the beginning or changed
				   at snapshot. Has no effect on timer
				   interrupts, unless passed to set_timer() */



/* external routines */

extern prepage(/* pcb */);
extern int start_cost(/* pcb */);
/*  PCB    *pcb; */

extern set_timer(/* time_quantum */);
/*  int    time_quantum; */

extern int get_clock();





/****************************************************************************/
/*                                                                          */
/*				Module CPU				    */
/*			     Internal Routines				    */
/*                                                                          */
/****************************************************************************/

PCB *head, *tail;

void cpu_init()
{
  head = NULL;
  tail = NULL;
}

PCB* pop()
{
  if (head == NULL) {
    // Nothing to pop
    return NULL;
  } else if (head->prev == NULL) {
    // Only one element in queue
    // Pop and make the queue empty
    PCB *popped = head;

    head = NULL;
    tail = NULL;
    return popped;
  } else {
    // At least two elements in queue
    PCB *popped = head;
    PCB *new_head = popped->prev;

    // Remove popped element's link to queue
    popped->prev = NULL;

    // Remove previous element's link to popped element
    new_head->next = NULL;

    head = new_head;

    return popped;
  }
}

void insert_ready(pcb)
PCB *pcb;
{
  PCB *p;

  p = tail;
  while (p != NULL) {
    if (p == pcb) {
      return;
    } else {
      p = p->next;
    }
  }

  // Add to tail
  PCB *new_tail = pcb;
  if (tail == NULL) {
    // Empty queue
    head = new_tail;
    tail = new_tail;
    new_tail->prev = NULL;
    new_tail->next = NULL;
  } else {
    // Non-empty queue
    PCB *old_tail = tail;
    new_tail->next = old_tail;
    old_tail->prev = new_tail;
    tail = new_tail;
  }

  pcb->status = ready;

  /*
  fprintf(stderr, "queue when inserting\n");
  fprintf(stderr,"head:%p\ttail:%p\n", head, tail);
  fprintf(stderr,"Processes in the ready queue:\n");

  p = tail;
  while (p != NULL) {
	  fprintf(stderr,"Pid:%d\tself:%p\tprev:%p\tnext:%p\n",
			  p->pcb_id, p, p->prev, p->next);
    p = p->next;
  }
  fprintf(stderr, "\n");
  */
}

void dispatch()
{
  PCB* current_pcb = NULL;
  if (PTBR != NULL) {
    current_pcb = PTBR->pcb;
  }

  if( current_pcb != NULL &&
      current_pcb->status == running )
  {
    insert_ready(current_pcb);
  }

  current_pcb = pop();

  if( current_pcb != NULL )
  {
    PTBR = current_pcb->page_tbl;
    current_pcb->status = running;
    current_pcb->last_dispatch = get_clock();
    set_timer(Quantum);
  }
  else
  {
    PTBR = NULL;
  }

  /*
  fprintf(stderr, "queue when dispatching\n");
  fprintf(stderr,"head:%p\ttail:%p\n", head, tail);
  fprintf(stderr,"Processes in the ready queue:\n");

  PCB *p = tail;
  while (p != NULL) {
	  fprintf(stderr,"Pid:%d\tself:%p\tprev:%p\tnext:%p\n",
			  p->pcb_id, p, p->prev, p->next);
    p = p->next;
  }
  fprintf(stderr, "\n");
  */
}

/* end of module */
