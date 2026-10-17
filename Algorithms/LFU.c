#include <stdio.h>
#include <stdlib.h>
#include "LFU.h"
#include "../DataStructs/MMU.h"

void insertionPageLFU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int * ptrDiskAddr, MMU * mmu) {
    if (Memory->cantidad < mmu->frameCount) {
        nodePage->M_Address = Memory->final;
        addOneMoreFreq(nodePage);
        enqueue(Memory, nodePage);
        mmu->elapsedTime += 5;
        return;
    }

    NodePage * victim = queueAt(Memory, 0);
    for (int k = 1; k < Memory->cantidad; k++) {
        NodePage * candidate = queueAt(Memory, k);
        if (candidate->freq < victim->freq) victim = candidate;
    }

    NodePage * oldestNode = deleteSpecificElementQueue(Memory, victim->id);

    putOnVirtualMemory(oldestNode, *ptrDiskAddr);
    addDiskTableElement(diskTable, *ptrDiskAddr, oldestNode);
    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->unloaded++;

    nodePage->M_Address = Memory->final;
    addOneMoreFreq(nodePage);
    enqueue(Memory, nodePage);
    mmu->elapsedTime += 5;
}

void bringPageToMemoryLFU(Queue * Memory, DiskTable * diskTable, int diskPos, int * ptrDiskAddr, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if (nodePage != NULL) {
        mmu->thrashTime += 5;
        mmu->unloaded--;
        bringFromDisk(nodePage);
        insertionPageLFU(Memory, nodePage, diskTable, ptrDiskAddr, mmu);
    }
}