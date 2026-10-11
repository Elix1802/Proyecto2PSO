#include <stdio.h>
#include <stdlib.h>
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"

void insertionPageFIFO(DLinkedList * linkedList, NodePage * nodePage, int diskLocation){
    NodePage* currentNode = getNextPtr(linkedList->head);

    if(linkedList->size<100) {
        appendNodeLinkedList(linkedList, nodePage);
        return;
    }

    int lowestId = currentNode->id;
    NodePage * oldestNode = currentNode;

    while (currentNode != linkedList->tail) {
        if(currentNode->id >= 0) {
            if(currentNode->id < lowestId) {
                lowestId = currentNode->id;
                oldestNode = currentNode;
            }
        }
        currentNode = getNextPtr(currentNode);
    }

    nodePage->pastPtr = oldestNode->pastPtr;

    nodePage->nextPtr = oldestNode->nextPtr;

    oldestNode->pastPtr->nextPtr = nodePage;

    oldestNode->nextPtr->pastPtr = nodePage;

    putOnVirtualMemory(oldestNode, diskLocation);

}