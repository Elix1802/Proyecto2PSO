#include <stdio.h>
#include <stdlib.h>

#include "SecondChance.h"


void insertionPageSecondChance(DLinkedList *linkedList, NodePage *nodePage, int diskLocation) {
    if (linkedList->size < 100) {
        appendNodeLinkedList(linkedList, nodePage);
        return;
    }
    Queue queue;
    inicializar(&queue);
    NodePage* currentNode = getNextPtr(linkedList->head);
    while (queue.cantidad < linkedList->size) {
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
        enqueue(&queue, oldestNode);
    }

    int notRemplaced = 1;
    while (notRemplaced) {
        NodePage* candidateNode = dequeue(&queue);
        if (candidateNode->Mark == 0) {
            candidateNode->Mark = 1;
            notRemplaced = 0;
        }
        else {
            enqueue(&queue, candidateNode);
        }
    }
}