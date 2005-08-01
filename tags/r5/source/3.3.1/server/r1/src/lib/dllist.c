/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * dllist.c- Routines for doubly linked list implementation
 *
 */

#include "dllist.h"

/* dlNewList - create a new dllist header
 *
 * Input - None
 *
 * Output - The newly created Dllist header
 */
 
Dllist*
dlNewList()
{
  Dllist* l;

    l = malloc(sizeof(Dllist));
    l->dll_head = 0;
    l->dll_tail = 0;

    return l;
}

/* dlFreeList - Free up a Dllist header and all the elements 
 * associated with it.
 *
 * Input - Dllist* l - The Dllist header
 *
 * Output - None
 *
 */
void
dlFreeList(Dllist* l)
{
    Dlelem* curr;

    while ( (curr = dlRemHead(l)) != 0)
      	free(curr);

    free(l);
}

/* dlNewElem - Create a new element
 * 
 * Input : void* val - The value of the element 
 *
 * Output : Returns the newly created Dlelem*
 */

Dlelem* 
dlNewElem(void* val)
{
 Dlelem* e;
    e = malloc(sizeof(Dlelem));
    e->dle_next = 0;
    e->dle_prev = 0;
    e->dle_val = val;
    e->dle_list = 0;
    return e;
}

/* dlFreeElem - Free an element
 *
 * Input : Dlelem* e - The element to be freed.
 *
 * Output - None
 */

void
dlFreeElem(Dlelem* e)
{
    if (e->dle_val != NULL)
	/* XXXXX - fix a memory leak problem */
	free (e->dle_val);
    free(e);
}

/* dlGetHead - Get the element at the head of a Dllist
 *
 * Input - Dllist* l - The Dllist header
 *
 * Output : Returns the elemnet at the head of the Dllist
 */

Dlelem*
dlGetHead(Dllist* l)
{
    return (l ? l->dll_head : 0);
}

/* dlGetHeadVal - get the value stored in the first element 
 *
 * Input : Dllist* l - The Dllist header
 *
 * Output : The pointer to the  value stored in the first element.
 */

void*
dlGetHeadVal(Dllist* l)
{
    Dlelem* e = dlGetHead(l);
    return (e ? e->dle_val : 0);
}

/* dlGetTail - Get the element at the tail of the Dllist
 *
 * Input : Dllist* l - The Dllist header
 *
 * Output - The tail element
 */

Dlelem* 
dlGetTail(Dllist* l)
{
    return (l ? l->dll_tail : 0);
}

/* dlGetTailVal - Get the value stored in the tail element 
 *
 * Input - Dllist* l - The Dllist header
 * 
 * Output - The pointer to the value stored in the tail element
 */

void*
dlGetTailVal(Dllist* l)
{
    Dlelem* e = dlGetTail(l);
  
    return (e ? e->dle_val : 0);
}

/* dlGetPred - Get the previous element in the chain
 *
 * Input : Dlelem* e - The current element
 *
 * Output - Returns the previous element
 */

Dlelem* 
dlGetPred(Dlelem* e) /* get predecessor */
{
    return (e ? e->dle_prev : 0);
}

/* dlGetSucc - Get the next element in the chain
 *
 * Input : Dlelem* e - The current element
 *
 * Output - Returns the next element
 */

Dlelem* 
dlGetSucc(Dlelem* e) /* get successor */
{
    return (e ? e->dle_next : 0);
}

/* dlRemove - Remove an element from its Dllist
 *
 * Input : Dlelem* e - The element to be removed
 *
 * Output - None
 */
 
void
dlRemove(Dlelem* e)
{
    Dllist* l;

    if (e->dle_prev)
    	e->dle_prev->dle_next = e->dle_next;
    if (e->dle_next)
    	e->dle_next->dle_prev = e->dle_prev;

    l = e->dle_list;
    if (e == l->dll_head)
    	dlRemHead(l);
    if (e == l->dll_tail)
    	dlRemTail(l);
}

/* dlAddHead - Add an element to the head of a Dllist
 * 
 * Input - Dllist* l - The Dllist header
 *	   Dlelem* e - The element to be added
 *
 * Output - None
 */

void    
dlAddHead(Dllist* l, Dlelem* e)
{
    e->dle_list = l;

    if (l->dll_head)  {
    	l->dll_head->dle_prev = e;
    	e->dle_next = l->dll_head;
    }
    e->dle_prev = 0;
    l->dll_head = e;

    if (l->dll_tail == 0)
    	l->dll_tail = l->dll_head;
}

/* dlAddTail - Add an element to the tail of a Dllist
 *
 * Input - Dllist* l - The Dllist header
 *         Dlelem* e - The element to be added
 *
 * Output - None
 */

void
dlAddTail(Dllist* l, Dlelem* e)
{
    e->dle_list = l;

    if (l->dll_tail)   {
    	l->dll_tail->dle_next = e;
    	e->dle_prev = l->dll_tail;
    }
    e->dle_next = 0;
    l->dll_tail = e;

    if (l->dll_head == 0) /* if this is first element added */
    	l->dll_head = l->dll_tail;
}

/* dlRemHead - Remove and returns the head element
 *
 * Input : Dllist* l - The Dllist header
 *
 * Output : Returns the head element
 */

Dlelem* 
dlRemHead(Dllist* l) 
{
    Dlelem* result;

    if (l->dll_head == 0)
    	return 0;

    result = l->dll_head;
    if (l->dll_head->dle_next) {
    	l->dll_head->dle_next->dle_prev = 0;
    }

    l->dll_head = l->dll_head->dle_next;

    result->dle_next = 0;
    result->dle_list = 0;
    
    if (result == l->dll_tail) /* if the head is also the tail */
    	l->dll_tail = 0;

    return result;
}

/* dlRemTail - Remove and returns the tail element
 *
 * Input : Dllist* l - The Dllist header
 *
 * Output : Returns the tail element
 */

Dlelem* 
dlRemTail(Dllist* l)
{
    Dlelem* result;

    if (l->dll_tail == 0 )
    	return 0;

    result = l->dll_tail;
    if (l->dll_tail->dle_prev) {
    	l->dll_tail->dle_prev->dle_next = 0;
    }
    l->dll_tail = l->dll_tail->dle_prev;

    result->dle_prev = 0;
    result->dle_list = 0;

    if (result == l->dll_head) /* if the tail is also the head */
    	l->dll_head = 0;

    return result;
}

