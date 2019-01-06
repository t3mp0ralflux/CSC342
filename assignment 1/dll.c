/****************************************************************************/
/* File: dll.c
 */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              Simple Queue ADT                                            */
/*                                                                          */
/*              Implementation                                              */
/*                                                                          */
/****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "dll.h"


// Initializer for Doubly linked list
void initDLL(DoublyLinkedList *self)
{
        if(self == NULL) return;
        self -> head = self->tail = NULL;
}

// Add a new node to the end
void appendDLL(DoublyLinkedList *self, data_t *data)
{
        // does it exist?
        if(self == NULL) return;

        // Make a new node
        ListNode *newNode;
        newNode = (ListNode *) calloc(1,sizeof(ListNode));
        newNode->data = data;
        newNode->next = newNode->prev = NULL;


        if(self->head == NULL)
        {
                self->head = self->tail = newNode;
        }
        else
        {
                self->tail->next = newNode;
                newNode->prev = self->tail;
                self->tail = newNode;
        }
}

// Add a new node to the front
void prependDLL(DoublyLinkedList *self, data_t *data)
{

        if(self==NULL)return;


        ListNode *newNode;
        newNode = (ListNode *)calloc(1,sizeof(ListNode));
        newNode->data = data;
        newNode->next = newNode->prev = NULL;


        if(self->head ==NULL)
        {
                self->head = self->tail = newNode;
        }

        else
        {
                newNode->next = self->head;
                self->head->prev = newNode;
                self->head = newNode;
        }
}

// Insert a new node at specific spot
void insertBeforeDLL(DoublyLinkedList *self, data_t *data, ListNode *ln)
{

        if(self == NULL) return;


        if(ln == NULL)
        {
                appendDLL(self,data);
        }


        else if (ln==self->head)
        {
                prependDLL(self,data);
        }


        else
        {
                ListNode *new;
                new = (ListNode *)calloc(1,sizeof(ListNode));
                new->data = data;
                new->next = ln;
                new->prev = ln->prev;
                ln->prev->next = new;
                ln->prev = new;
        }
}

// Value without removal
data_t *frontValue(DoublyLinkedList *self)
{
        if(self==NULL)return;
        if(self->head != NULL)
        {
                return self->head->data;
        }
        else
        {
                return NULL;
        }
}

// First node without removal
ListNode *frontNode(DoublyLinkedList *self)
{
        if(self==NULL)return;
        if(self->head!=NULL)
        {
                return self->head;
        }
        else
        {
                return NULL;
        }
}


// Remove the node at ln
void removeNode(DoublyLinkedList *self, ListNode *ln)
{
        if(self==NULL)return;

        // List only has one node
        if(ln==self->head && ln==self->tail)
        {
                self->head = self->tail = NULL;
                free(ln);
        }

        // The node is at the front of the list.
        else if(ln==self->head)
        {
                self->head = ln->next;
                ln->next->prev = NULL;
                free(ln);
        }

        // The node is at the end of the list.
        else if(ln==self->tail)
        {
                self->tail = ln->prev;
                ln->prev->next = NULL;
                free(ln);
        }

        // All other nodes in the list
        else if(ln!=NULL)
        {
                ln->prev->next = ln->next;
                ln->next->prev = ln->prev;
                free(ln);
        }
}


// Find a node in the list and return a pointer to it.
ListNode *findNode(DoublyLinkedList *self, data_t *data)
{
        if(self==NULL)return;
        ListNode *ln;
        ln = self->head;
        while(ln!=NULL)
        {
                // It matches
                if(ln->data->key==data->key)
                {
                        return ln;
                }

                // No match, move on
                else
                {
                        ln = ln->next;
                }
        }

        // 404 node not found
        return NULL;
}


// Find a node in the list containing the specified data
data_t *findValue(DoublyLinkedList *self, data_t *data)
{
        if(self==NULL)return;
        ListNode *ln;
        ln = self->head;
        while(ln!=NULL)
        {
                // match made
                if(ln->data->key == data->key)
                {
                        return ln->data;
                }

                // try again
                else
                {
                        ln = ln->next;
                }
        }

        // 404: not found
        return NULL;
}

// Print the list with string
void printDLL(DoublyLinkedList *self, char *label)
{
        if(self==NULL)return;
        ListNode *ln;
        ln = self->head;

        // Print the label first
        printf("%s",label);


        if (ln==NULL)
        {
                printf("Nothing in list, try putting something in first. \n");
        }
        else
        {
                // while there are nodes remaining in the list print
                while (ln != NULL)
                {
                        printf("%s",toString(ln->data));
                        ln = ln->next;
                }
        }
}


// Print the list backwards
void printDLLBackwards(DoublyLinkedList *self, char *label)
{
        if(self==NULL)return;
        ListNode *ln;
        ln= self->tail;

        // Print the label first
        printf("%s",label);

        // The list is empty
        if(ln==NULL)
        {
                printf("Nothing to see here.\n");
        }
        else
        {
                // while there are nodes remaining in the list, print.
                while (ln != NULL)
                {
                        printf(" %s ",toString(ln->data));
                        ln = ln->prev;
                }
        }
        printf("\n");
}


char *toString(data_t *d)
{
    static char result[BUFSIZ];

    if (d == NULL)
    {
        sprintf (result,"NULL");
    }
    else
    {
        sprintf (result,"key=%d(data=%d) ", d->key, d->value);
    }
    return result;
}


int main(int argc, char *argv[])
{
    DoublyLinkedList myDLL;
    int a,i, j;
    data_t data[11], *d;
    ListNode *m = NULL;
    long r;


    initDLL(&myDLL);
    printDLL(&myDLL, "before insertBefore myDLL: ");
    printf("findValue = %s\n", toString(findValue(&myDLL, &data[0])));

    for (i = 0; i < 10; i++) {
        data[i].key = i;
        data[i].value = 10*i;

        r = rand();
        j = r % (i + 1);
        printf("\n________________________________LOOP %i___________________________\n",i);
        printf("\nprepending\n");
        prependDLL(&myDLL, &data[i]);
        printDLL(&myDLL, "after prependDLL\n");
        printf("\nappending\n");
        appendDLL(&myDLL, &data[i]);
        printDLL(&myDLL, "after appendDLL\n");
        printf("\nInsert with findNode\n");
        printf("\nLooking for data[%i]",j);
        printf("\nj = %i\n",j);
        printf(" Key = %s\n",toString(&data[j]));
        insertBeforeDLL(&myDLL, &data[i], findNode(&myDLL, &data[j]));
        printf("\ninsert NULL Before \n");
        insertBeforeDLL(&myDLL, &data[i], NULL);
        printf("\ninsert before the front node\n");
        insertBeforeDLL(&myDLL, &data[i], frontNode(&myDLL));
        printf("\nFinding value\n");
        printf("findValue = %s\n", toString(findValue(&myDLL, &data[i])));
    }

    printf("First NODE : %s",toString(frontValue(&myDLL)));
    printDLL(&myDLL, "after insertBefore myDLL: ");
    printf("\n");
    printDLLBackwards(&myDLL, "after insertBefore myDLL backwards: ");

    data[10].key = 11;
    data[10].value = 10*11;


    printf("\n\nbefore insertBefore %s: ", toString(&data[2]));
    printDLL(&myDLL, "");
    printf("\n End of Program\n");

    insertBeforeDLL(&myDLL, &data[i], findNode(&myDLL, &data[2]));
    printf("after insertBefore %s: ", toString(&data[2]));
    printDLL(&myDLL, "");


    printf("\nAt removal for loop\n");
    for ( a = 0; a < 10; a++)
    {
        printf("\n______________________REMOVAL LOOP %i______________________\n",a);
        removeNode(&myDLL, findNode(&myDLL, &data[a]));
        printf("after removing Node %s: ", toString(&data[a]));
        printDLL(&myDLL, "\n");
    }

}

