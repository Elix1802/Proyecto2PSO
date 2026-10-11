#include <stdlib.h>

#include "NodePage.h"
#include "DLinkedList.h"

struct DLinkedList {
    NodePage* head;
    NodePage* tail;
    NodePage* current;
    int size;
};

DLinkedList* createDLinkedList() {
    DLinkedList* list = (DLinkedList*)malloc(sizeof(DLinkedList));
    if (list == NULL) {
        return NULL; 
    }
    list->current = list->head =createNodePage(-1, 0, 0, 0, 0, 0, NULL, NULL);
    NodePage* lastNode = createNodePage(-2, 0, 0, 0, 0, 0, NULL, list->head);
    list->tail = lastNode;
    putNextPtr(list->head, lastNode);
    list->size = 0;
    return list;
}

void destroyDLinkedList(DLinkedList* list) {
    if (list != NULL) {
        clearDLinkedList(list);
        destroyNodePage(list->head);
        destroyNodePage(list->tail);
        free(list);
    }
}

NodePage* getElement(DLinkedList* list) {
    if (getNextPtr(list->current) == list->tail || list->size == 0) {
        return NULL; 
    }

    return getNextPtr(list->current);
}

void insert(DLinkedList* list, int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr) {
    NodePage* newNode = createNodePage(pageId, parentId, loaded, L_Addr, M_Addr, D_addr, getNextPtr(list->current), list->current);
    NodePage* nextNode = getNextPtr(list->current);
    putNextPtr(list->current, newNode);
    putPastPtr(nextNode, newNode);

    list->size++;
}

void append(DLinkedList* list, int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr) {
    NodePage* newNode = createNodePage(pageId, parentId, loaded, L_Addr, M_Addr, D_addr, list->tail, getPastPtr(list->tail));
    NodePage* lastNode = getPastPtr(list->tail);
    putNextPtr(lastNode, newNode);
    putPastPtr(list->tail, newNode);

    list->size++;
}

NodePage* removeNode(DLinkedList* list) {
    if (getNextPtr(list->current) == list->tail || list->size == 0) {
        return NULL; 
    }

    NodePage* nodeToRemove = getNextPtr(list->current);
    NodePage* nextNode = getNextPtr(nodeToRemove);
    putNextPtr(list->current, nextNode);
    putPastPtr(nextNode, list->current);

    list->size--;
    return nodeToRemove;
}

void goToStart(DLinkedList* list) {
    list->current = list->head;
}

void goToEnd(DLinkedList* list) {
    list->current = getPastPtr(list->tail);
}

void nextNodePage(DLinkedList* list) {
    if (getNextPtr(list->current) != list->tail) {
        list->current = getNextPtr(list->current);
    }
}

int atStart(DLinkedList* list) {
    return list->current == list->head;
}

int atEnd(DLinkedList* list) {
    return getNextPtr(list->current) == list->tail;
}

void previousNodePage(DLinkedList* list) {
    if (list->current != list->head) {
        list->current = getPastPtr(list->current);
    }
}

int getSize(DLinkedList* list) {
    return list->size;
}

void printDLinkedList(DLinkedList* list) {
    NodePage* currentNode = getNextPtr(list->head);
    while (currentNode != list->tail) {
        printNodePage(currentNode);
        currentNode = getNextPtr(currentNode);
    }
}

void clearDLinkedList(DLinkedList* list){
    while (getNextPtr(list->head) != list->tail) {
        putNextPtr(list->head, getNextPtr(getNextPtr(list->head)));
        destroyNodePage(getPastPtr(getNextPtr(list->head)));
    }
    list->current = list->head;
    putPastPtr(list->tail, list->head);
    list->size = 0;
}