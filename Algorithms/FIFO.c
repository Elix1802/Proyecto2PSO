#include <stdio.h>
#include <stdlib.h>
#include "FIFO.h"
#include "../DataStructs/NodePage.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/MMU.h"

void insertionPageFIFO(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int * ptrDiskAddr, MMU * mmu) {
    if (Memory->cantidad < mmu->frameCount) {
        nodePage->M_Address = Memory->final;
        enqueue(Memory, nodePage);
        mmu->elapsedTime += 5;
        return;
    }

    NodePage * oldestNode = dequeue(Memory);

    putOnVirtualMemory(oldestNode, *ptrDiskAddr);
    addDiskTableElement(diskTable, *ptrDiskAddr, oldestNode);
    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->unloaded++;

    nodePage->M_Address = Memory->final;
    enqueue(Memory, nodePage);
    mmu->elapsedTime += 5;
}

void bringPageToMemoryFIFO(Queue * Memory, DiskTable * diskTable, int diskPos, int * ptrDiskAddr, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if (nodePage != NULL) {
        mmu->thrashTime += 5;
        mmu->unloaded--;
        bringFromDisk(nodePage);
        insertionPageFIFO(Memory, nodePage, diskTable, ptrDiskAddr, mmu);
    }
}
