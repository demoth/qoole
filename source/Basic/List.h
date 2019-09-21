/*
Copyright (C) 1996-1997 GX Media, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/*
 * list.h     Template for a LinkList structure.
 */

#ifndef _LIST_H_
#define _LIST_H_

#include <stdio.h>
//#include "LCommon.h"

//==================== ElmNode ====================

class ElmNode {
 public:
  inline ElmNode(void) { prevNode = nextNode = NULL; };
  inline ElmNode(const ElmNode &src) { *this = src; };
  // ~ElmNode(void);
  // ElmNode &operator=(const ElmNode &src);
  
  inline ElmNode *SetPrevNode(ElmNode *prev) { prevNode = prev; return this; };
  inline ElmNode *SetNextNode(ElmNode *next) { nextNode = next; return this; };
  inline ElmNode *GetPrevNode(void) const { return prevNode; };
  inline ElmNode *GetNextNode(void) const { return nextNode; };

 private:
  ElmNode *nextNode, *prevNode;
};

//==================== LinkList ====================

template <class NodeType> class IterLinkList;

template <class NodeType>
class LinkList {
  friend class IterLinkList<NodeType>;

 public:
  LinkList(void);
  LinkList(const LinkList &src);
  virtual ~LinkList(void);
  LinkList &operator=(const LinkList &src);

  int NumOfElm(void) const;
  int FindNodeIndex(const ElmNode *recPtr) const;

  int DeleteAllNodes(void);

  NodeType &InsertNode(ElmNode &rec, int index);
  NodeType &ConcatNode(ElmNode &rec);
  NodeType &AppendNode(ElmNode &rec);
  NodeType &RemoveNode(ElmNode &rec);
  NodeType &RemoveNode(int index);
  NodeType &FindNodeByIndex(int index) const;
  NodeType &operator[](int index) const { return FindNodeByIndex(index); };

  // Merge...
  int OrderMerge(LinkList &mList,
                 int (*CmpFunc)(const NodeType &n1, const NodeType &n2));

 private:
  int listSize;
  ElmNode *head, *tail;
};

//==================== IterLinkList ====================

template <class NodeType>
class IterLinkList {
 public:
  IterLinkList(const LinkList<NodeType> &list);
  IterLinkList(const IterLinkList<NodeType> &src) { *this = src; };
  // ~IterLinkList(void);

  void Reset(void);
  bool IsDone(void) const;
  NodeType *GetNext(void);
  NodeType *FindNextByFunc(bool (*FindFunc)(const NodeType &node));
  int Apply2All(bool (*ApplyFunc)(NodeType &node));

 private:
  const LinkList<NodeType> *linkList;
  ElmNode *iterPtr;
};

//==================== LinkList ====================

template <class NodeType>
LinkList<NodeType>::LinkList(void) {
  listSize = 0;
  head = tail = NULL;
}

template <class NodeType>
LinkList<NodeType>::LinkList(const LinkList<NodeType> &src) {
  listSize = 0;
  head = tail = NULL;
  *this = src;
}

template <class NodeType>
LinkList<NodeType>::~LinkList(void) {
  // Can't destroy rest of the ElmNodes in list.
  if (head != NULL) {
    // Warning.  Leaking Mem.
//    LFatal("Leaking Mem...\n");
  }
}

template <class NodeType>
int
LinkList<NodeType>::NumOfElm(void) const {
  // PreConditions.
  ASSERT(listSize >= 0);

  return listSize;
}

template <class NodeType>
int
LinkList<NodeType>::DeleteAllNodes(void) {
  int rtnVal = NumOfElm();

  while (NumOfElm() > 0)
	  delete &(RemoveNode(0));

  return rtnVal;
}

template <class NodeType>
NodeType &
LinkList<NodeType>::InsertNode(ElmNode &rec, int index) {
  // PreCondition.
  ASSERT(listSize >= 0);
  ASSERT(index >= 0 && index <= NumOfElm());

  if (listSize == 0) {
    // Empty list.
    ASSERT(index == 0);

    rec.SetPrevNode(NULL);
    rec.SetNextNode(NULL);
    head = tail = &rec;
  }
  else if (index == 0) {
    // Insert at head of list.
    rec.SetPrevNode(NULL);
    rec.SetNextNode(head);
    head->SetPrevNode(&rec);
    head = &rec;
  }
  else if (index == NumOfElm()) {
    // Append at end of list.
    rec.SetNextNode(NULL);
    rec.SetPrevNode(tail);
    tail->SetNextNode(&rec);
    tail = &rec;
  }
  else {
    // Insert in the middle of list.
    ElmNode *prev, *next;

    next = &(FindNodeByIndex(index));
    prev = next->GetPrevNode();
    prev->SetNextNode(&rec);
    next->SetPrevNode(&rec);
    rec.SetPrevNode(prev);
    rec.SetNextNode(next);
  }

  listSize++;
  return (NodeType &) rec;
}

template <class NodeType>
NodeType &
LinkList<NodeType>::ConcatNode(ElmNode &rec) {
  return (NodeType &) InsertNode(rec, 0);
}

template <class NodeType>
NodeType &
LinkList<NodeType>::AppendNode(ElmNode &rec) {
  return (NodeType &) InsertNode(rec, NumOfElm());
}

template <class NodeType>
NodeType &
LinkList<NodeType>::RemoveNode(ElmNode &rec) {
  // PreConditions.
  ASSERT(FindNodeIndex(&rec) != -1);
  ASSERT(listSize > 0);

  if (listSize == 1) {
    // Only 1 Node.
    ASSERT(head == &rec);
    head = tail = NULL;
  }
  else if (rec.GetPrevNode() == NULL) {
    // rec is at head of list.
    head = rec.GetNextNode();
    head->SetPrevNode(NULL);
  }
  else if (rec.GetNextNode() == NULL) {
    // rec is at end of list.
    tail = rec.GetPrevNode();
    tail->SetNextNode(NULL);
  }
  else {
    // rec is in the middle.
    (rec.GetPrevNode())->SetNextNode(rec.GetNextNode());
    (rec.GetNextNode())->SetPrevNode(rec.GetPrevNode());
  }

  rec.SetPrevNode(NULL);
  rec.SetNextNode(NULL);
  listSize--;

  return (NodeType &) rec;
}

template <class NodeType>
NodeType &
LinkList<NodeType>::RemoveNode(int index) {
  // PreCondition.

  return RemoveNode(FindNodeByIndex(index));
}

template <class NodeType>
NodeType &
LinkList<NodeType>::FindNodeByIndex(int index) const {
  int i;
  ElmNode *currPtr;

  // PreConditions.
  ASSERT(index >= 0 && index < NumOfElm());

  currPtr = head;
  for(i = 0; i < index; i++)
    currPtr = currPtr->GetNextNode();

  return (NodeType &) *currPtr;
}

template <class NodeType>
int
LinkList<NodeType>::FindNodeIndex(const ElmNode *recPtr) const {
  // PreCondition.
  ASSERT(recPtr != NULL);

  int i;
  ElmNode *currPtr;

  currPtr = head;
  for(i = 0; i < NumOfElm(); i++) {
    if (recPtr == currPtr)
      return i;
    currPtr = currPtr->GetNextNode();
  }

  return -1;
}

template <class NodeType>
LinkList<NodeType> &
LinkList<NodeType>::operator=(const LinkList<NodeType> &src) {
  // Sanity Check.  Warn if list isn't empty.
  ASSERT(listSize == 0);
  ASSERT(head == NULL && tail == NULL);

  int i;

  if (&src == this)
    return *this;

  for(i = 0; i < src.NumOfElm(); i++)
    AppendNode(*(new NodeType(src[i])));

  return *this;
}

// Used for merge sort.
// Both list should be pre-sorted in reverse order.

template <class NodeType>
int
LinkList<NodeType>::OrderMerge(LinkList &mList,
                               int (*CmpFunc)(const NodeType &n1,
                                              const NodeType &n2)) {
  int nListSize = listSize + mList.listSize;
  ElmNode *nHead, *nTail;
  ElmNode *ptr1, *ptr2, *ptr;

  if (listSize > 0 && mList.listSize > 0) {
    ptr1 = &(FindNodeByIndex(0));
    ptr2 = &(mList.FindNodeByIndex(0));
    if ((*CmpFunc)(*((NodeType *) ptr1), *((NodeType *) ptr2)) < 0)
      nHead = &(mList.RemoveNode(*ptr2));
    else
      nHead = &(RemoveNode(*ptr1));
  }
  else if (listSize > 0) {
    nHead = &(RemoveNode(0));
  }
  else if (mList.listSize > 0) {
    nHead = &(mList.RemoveNode(0));
  }
  else {
    nHead = NULL;
  }

  nTail = nHead;

  while (listSize > 0 && mList.listSize > 0) {
    ptr1 = &(FindNodeByIndex(0));
    ptr2 = &(mList.FindNodeByIndex(0));
    if ((*CmpFunc)(*((NodeType *) ptr1), *((NodeType *) ptr2)) < 0)
      ptr = &(mList.RemoveNode(*ptr2));
    else
      ptr = &(RemoveNode(*ptr1));
    nTail->SetNextNode(ptr);
    ptr->SetPrevNode(nTail);
    nTail = ptr;
  }

  if (listSize > 0) {
    nTail->SetNextNode(head);
    head->SetPrevNode(nTail);
    nTail = tail;
    head = tail = NULL;
    listSize = 0;
  }
  else if (mList.listSize > 0) {
    nTail->SetNextNode(mList.head);
    mList.head->SetPrevNode(nTail);
    nTail = mList.tail;
    mList.head = mList.tail = NULL;
    mList.listSize = 0;
  }

  listSize = nListSize;
  head = nHead;
  tail = nTail;

  // Post Condition.
  ASSERT(mList.NumOfElm() == 0);
  return listSize;
}

//==================== IterLinkList ====================

template <class NodeType>
IterLinkList<NodeType>::IterLinkList(const LinkList<NodeType> &list) {
  linkList = &list;
  iterPtr = NULL;
}

template <class NodeType>
void
IterLinkList<NodeType>::Reset(void) {
  iterPtr = linkList->head;
}

template <class NodeType>
bool
IterLinkList<NodeType>::IsDone(void) const {
  return (iterPtr == NULL ? true : false);
}

template <class NodeType>
NodeType *
IterLinkList<NodeType>::GetNext(void) {
  ElmNode *rtnNode;

  if (IsDone()) return NULL;
  rtnNode = iterPtr;
  iterPtr = iterPtr->GetNextNode();

  return (NodeType *) rtnNode;
}

template <class NodeType>
NodeType *
IterLinkList<NodeType>::
FindNextByFunc(bool (*FindFunc)(const NodeType &node)) {
  ElmNode *rtnNode;

  while (!IsDone()) {
    rtnNode = iterPtr;
    iterPtr = iterPtr->GetNextNode();

    if ((*FindFunc)(*((NodeType *) rtnNode)))
      return (NodeType *) rtnNode;
  }
  return NULL;
}

template <class NodeType>
int
IterLinkList<NodeType>::Apply2All(bool (*ApplyFunc)(NodeType &node)) {
  // PreCondition.
  ASSERT(ApplyFunc != NULL);

  int rtnVal = 0;

  Reset();
  while(!IsDone()) {
    if ((*ApplyFunc)(*GetNext()))
      rtnVal++;
  }
  return rtnVal;
}

#endif // _LIST_H_
