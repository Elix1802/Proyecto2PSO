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

    int highestTime = -10;
    NodePage * oldestNode;

    while (currentNode != linkedList->tail) {
        if(currentNode->id >= 0) {
            if(currentNode->loaded_T > highestTime) {
                highestTime = currentNode->loaded_T;
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