#include <stdio.h>
#include <stdlib.h>
#include "LRU.h"
#include "../DataStructs/MMU.h"

void insertionPageLRU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, MMU * mmu){
    if(Memory->cantidad < 100) {
        nodePage->M_Address = Memory->final;
        enqueue(Memory, nodePage);
        mmu->elapsedTime+=5;
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

    //printNodePage(nodePage);
    nodePage->M_Address = Memory->final;
    enqueue(Memory, nodePage);

    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->elapsedTime+=5;

}

void bringPageToMemoryLRU(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if(nodePage != NULL) {
        mmu->trashingTime+=5;
        bringFromDisk(nodePage);
        insertionPageLRU(Memory, nodePage, diskTable, ptrDiskAddr, mmu);

    }
}