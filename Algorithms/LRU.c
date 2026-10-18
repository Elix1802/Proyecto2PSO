#include <stdio.h>
#include <stdlib.h>
#include "LRU.h"
#include "../DataStructs/MMU.h"

void insertionPageLRU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int * ptrDiskAddr, MMU * mmu) {
    if (Memory->cantidad < mmu->frameCount) {
        enqueue(Memory, nodePage);
        mmuPlacePage(mmu, nodePage);
        mmu->elapsedTime += 5;
        nodePage->loaded_T = mmu->elapsedTime;
        return;
    }

    NodePage * lastRecentlyUsedPage = dequeue(Memory);

    putOnVirtualMemory(lastRecentlyUsedPage, *ptrDiskAddr);
    addDiskTableElement(diskTable, *ptrDiskAddr, lastRecentlyUsedPage);
    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->unloaded++;

    mmuReplacePage(mmu, lastRecentlyUsedPage, nodePage);
    enqueue(Memory, nodePage);
    mmu->elapsedTime += 5;
    nodePage->loaded_T = mmu->elapsedTime;
}

void bringPageToMemoryLRU(Queue * Memory, DiskTable * diskTable, int diskPos, int * ptrDiskAddr, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if (nodePage != NULL) {
        mmu->thrashTime += 5;
        mmu->unloaded--;
        bringFromDisk(nodePage);
        insertionPageLRU(Memory, nodePage, diskTable, ptrDiskAddr, mmu);
    }
}