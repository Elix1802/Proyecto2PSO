#include <stdio.h>
#include <stdlib.h>
#include "SecondChance.h"
#include "../DataStructs/MMU.h"

void insertionPageSecondChance(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int * ptrDiskAddr, MMU * mmu) {
    if (Memory->cantidad < mmu->frameCount) {
        enqueue(Memory, nodePage);
        mmuPlacePage(mmu, nodePage);
        mmu->elapsedTime += 5;
        nodePage->loaded_T = mmu->elapsedTime;
        return;
    }

    while (1) {
        NodePage * candidateNode = dequeue(Memory);
        if (candidateNode->Mark == 1) {
            candidateNode->Mark = 0;
            enqueue(Memory, candidateNode);
        } else {
            putOnVirtualMemory(candidateNode, *ptrDiskAddr);
            addDiskTableElement(diskTable, *ptrDiskAddr, candidateNode);
            *ptrDiskAddr = (*ptrDiskAddr) + 1;
            mmu->unloaded++;

            mmuReplacePage(mmu, candidateNode, nodePage);
            enqueue(Memory, nodePage);
            mmu->elapsedTime += 5;
            nodePage->loaded_T = mmu->elapsedTime;
            return;
        }
    }
}

void bringPageToMemorySecondChance(Queue * Memory, DiskTable * diskTable, int diskPos, int * ptrDiskAddr, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if (nodePage != NULL) {
        mmu->thrashTime += 5;
        mmu->unloaded--;
        bringFromDisk(nodePage);
        insertionPageSecondChance(Memory, nodePage, diskTable, ptrDiskAddr, mmu);
    }
}