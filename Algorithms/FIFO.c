#include <stdio.h>
#include <stdlib.h>
#include "FIFO.h"
#include "../DataStructs/NodePage.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/MMU.h"

void insertionPageFIFO(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int * ptrDiskAddr, MMU * mmu) {
    if (Memory->cantidad < mmu->frameCount) {
        enqueue(Memory, nodePage);
        mmuPlacePage(mmu, nodePage);
        mmu->elapsedTime += 5;
        nodePage->loaded_T = mmu->elapsedTime;
        return;
    }

    NodePage * oldestNode = dequeue(Memory);

    putOnVirtualMemory(oldestNode, *ptrDiskAddr);
    addDiskTableElement(diskTable, *ptrDiskAddr, oldestNode);
    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->unloaded++;

    mmuReplacePage(mmu, oldestNode, nodePage);
    enqueue(Memory, nodePage);
    mmu->elapsedTime += 5;
    nodePage->loaded_T = mmu->elapsedTime;
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