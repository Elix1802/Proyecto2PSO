#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

typedef struct DLinkedList DLinkedList;

DLinkedList* createDLinkedList();
void destroyDLinkedList(DLinkedList* list);
NodePage* getElement(DLinkedList* list);
void insert(DLinkedList* list, int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr);
void append(DLinkedList* list, int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr);
NodePage* removeNode(DLinkedList* list);
void goToStart(DLinkedList* list);
void goToEnd(DLinkedList* list);
void nextNodePage(DLinkedList* list);
int atStart(DLinkedList* list);
int atEnd(DLinkedList* list);
void previousNodePage(DLinkedList* list);
int getSize(DLinkedList* list);
void printDLinkedList(DLinkedList* list);
void clearDLinkedList(DLinkedList* list);

#endif

