#include <stdio.h>
#include <stdlib.h>
#include "LRU.h"
#include "../DataStructs/MMU.h"

void insertionPageLRU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, MMU * mmu){
    if(Memory->cantidad < 100) {
        nodePage->M_Address = Memory->final;
        enqueue(Memory, nodePage);
        mmu->elapsedTime+=5;
        mmu->ramKB += mmu->pageSize;
        mmu->ramPct = mmu->ramKB * 100 / (double) mmu->totalMemory;
        mmu->loaded++;
        return;
    }

    for(int i = Memory->frente; i<Memory->final;i++) {
        if(Memory->datos[i]->id == nodePage->id) {
            deleteSpecificElementQueue(Memory, nodePage->id);
            enqueue(Memory, nodePage);
            mmu->elapsedTime++;
            return;
        }
    }

    NodePage * lastRecentlyUsedPage = dequeue(Memory);

    putOnVirtualMemory(lastRecentlyUsedPage, *ptrDiskAddr);

    addDiskTableElement(diskTable, *ptrDiskAddr, lastRecentlyUsedPage);

    nodePage->M_Address = Memory->final;
    enqueue(Memory, nodePage);

    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->elapsedTime+=5;
    mmu->vramKB += mmu->pageSize;
    mmu->vramPct = mmu->vramKB * 100 / 512.00;

    mmu->unloaded++;

}

void bringPageToMemoryLRU(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if(nodePage != NULL) {
        mmu->thrashTime+=5;
        mmu->thrashPct = mmu->thrashTime * 100 / (double) mmu->elapsedTime;  
        mmu->vramKB -= mmu->pageSize;
        mmu->vramPct = mmu->vramKB * 100 / 512.00;
        mmu->unloaded--;
        bringFromDisk(nodePage);
        insertionPageLRU(Memory, nodePage, diskTable, ptrDiskAddr, mmu);

    }
}