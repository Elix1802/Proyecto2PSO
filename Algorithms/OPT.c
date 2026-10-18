#include <stdio.h>
#include <stdlib.h>
#include "OPT.h"
#include "../DataStructs/NodePage.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"

static int nextUse(NodePage ** futurePredicts, int from, int limit, int pageId) {
    for (int j = from; j < limit; j++) {
        if (futurePredicts[j] == NULL) break;
        if (futurePredicts[j]->id == pageId) return j;
    }
    return -1;
}

void insertionPageOPT(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int * ptrDiskAddr, NodePage ** futurePredicts, MMU * mmu) {
    if (Memory->cantidad < mmu->frameCount) {
        enqueue(Memory, nodePage);
        mmuPlacePage(mmu, nodePage);
        mmu->elapsedTime += 5;
        nodePage->loaded_T = mmu->elapsedTime;
        return;
    }

    NodePage * victim = NULL;
    int farthest = -1;
    for (int k = 0; k < Memory->cantidad; k++) {
        NodePage * candidate = queueAt(Memory, k);
        int use = nextUse(futurePredicts, mmu->actualTimeMoment, mmu->sizeDefault, candidate->id);
        if (use == -1) {
            victim = candidate;
            break;
        }
        if (use > farthest) {
            farthest = use;
            victim = candidate;
        }
    }

    NodePage * oldestNode = deleteSpecificElementQueue(Memory, victim->id);

    putOnVirtualMemory(oldestNode, *ptrDiskAddr);
    addDiskTableElement(diskTable, *ptrDiskAddr, oldestNode);
    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->unloaded++;

    mmuReplacePage(mmu, oldestNode, nodePage);
    enqueue(Memory, nodePage);
    mmu->elapsedTime += 5;
    nodePage->loaded_T = mmu->elapsedTime;
}

void bringPageToMemoryOPT(Queue * Memory, DiskTable * diskTable, int diskPos, int * ptrDiskAddr, NodePage ** futurePredicts, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if (nodePage != NULL) {
        mmu->thrashTime += 5;
        mmu->unloaded--;
        bringFromDisk(nodePage);
        insertionPageOPT(Memory, nodePage, diskTable, ptrDiskAddr, futurePredicts, mmu);
    }
}