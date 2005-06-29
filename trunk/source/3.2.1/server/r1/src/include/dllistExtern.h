/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* dllistExtern.h - Prototype for dllist.c
 *
 *   Example of using Dllist :
 *
 *   lst = dlNewList();                -- make a new dllist
 *   dlAddHead(lst, dlNewElem(inList)); -- add a new element to the list
 *   elt = dlGetHead(lst);             -- retrieve the head element
 *   outList = (void*)DLE_VAL(elt);    -- get the value of an element
 *   dlRemove(elt);                    -- removes the element from its list
 *   dlFreeElem(elt);                  -- free the element
 */

#ifndef DLLIST_EXTERN_H
#define DLLIST_EXTERN_H

struct Dllist;
struct Dlelem;
 
typedef struct Dlelem {
  struct Dlelem *dle_next;  /* next element */
  struct Dlelem *dle_prev;  /* previous element */
  void   *dle_val; /* value of the element */
  struct Dllist *dle_list;  /* what list this element is in */
} Dlelem;
 
typedef struct Dllist {
  Dlelem *dll_head;
  Dlelem *dll_tail;
} Dllist;
 
extern Dllist* dlNewList();
extern void    dlFreeList(Dllist* l);
extern Dlelem* dlNewElem(void* val);
extern void    dlFreeElem(Dlelem* elem);
extern Dlelem* dlGetHead(Dllist* l);
extern Dlelem* dlGetTail(Dllist* l);
extern void*   dlGetHeadVal(Dllist* l);
extern void*   dlGetTailVal(Dllist* l);
extern Dlelem* dlGetPred(Dlelem* elem);
extern Dlelem* dlGetSucc(Dlelem* elem);
extern void    dlRemove(Dlelem* elem);
extern void    dlAddHead(Dllist* list, Dlelem* node);
extern void    dlAddTail(Dllist* list, Dlelem* node);
extern Dlelem* dlRemHead(Dllist* list);
extern Dlelem* dlRemTail(Dllist* list);
 
#define DLE_VAL(x)  (x->dle_val)

#endif	/* DLLIST_EXTERN_H */
