/****************************************************************************/
/* File: dll.h
 */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              Doubly Linked List ADT                                      */
/*                                                                          */
/*              Declarations                                                */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>


//extern int ______trace_switch;


typedef struct {
    int key;
    int value;
} data_t;

typedef struct listNode {
    struct listNode *next, *prev;
    data_t *data;
} ListNode;

typedef struct listType {
    ListNode *head;
    ListNode *tail;
} DoublyLinkedList;



void initDLL(DoublyLinkedList *self);
void appendDLL(DoublyLinkedList *self, data_t *data);
void prependDLL(DoublyLinkedList *self, data_t *data);
void insertBeforeDLL(DoublyLinkedList *self, data_t *data, ListNode *p);
data_t *frontValue(DoublyLinkedList *self);
ListNode *frontNode(DoublyLinkedList *self);
void removeNode(DoublyLinkedList *self, ListNode *p);
ListNode *findNode(DoublyLinkedList *self, data_t *data);
data_t *findValue(DoublyLinkedList *self, data_t *data);
void printDLL(DoublyLinkedList *self, char *label);
void printDLLBackwards(DoublyLinkedList *self, char *label);
char *toString(data_t *d);


