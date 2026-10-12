#include <stdio.h>
#include <stdlib.h>

#include "SecondChance.h"


void insertionPageSecondChance(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr){
    
    if(Memory->cantidad < 100) {
        enqueue(Memory, nodePage);
        return;
    }

    
    while (1) {
        NodePage* candidateNode = dequeue(Memory);
        if (candidateNode->Mark == 1) {
            candidateNode->Mark = 0;
            enqueue(Memory, candidateNode);
        }
        else {
            enqueue(Memory, nodePage);
            putOnVirtualMemory(candidateNode, *ptrDiskAddr);
            addDiskTableElement(diskTable, *ptrDiskAddr, candidateNode);
            *ptrDiskAddr = (*ptrDiskAddr) + 1;
            return;
        }
        
    }
}

void bringPageToMemorySecondChance(Queue * Memory, DiskTable * diskTable, int memoryPos, int *  ptrDiskAddr) {
    NodePage * nodePage = getDiskTableValue(diskTable, memoryPos);
    if(nodePage != NULL) {
        bringFromDisk(nodePage);
        insertionPageSecondChance(Memory, nodePage, diskTable, ptrDiskAddr);

    }
}