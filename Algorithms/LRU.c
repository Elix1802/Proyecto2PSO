#include <stdio.h>
#include <stdlib.h>
#include "LRU.h"

void insertionPageLRU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr){
    if(Memory->cantidad < 6) {
        nodePage->M_Address = Memory->final;
        enqueue(Memory, nodePage);
        return;
    }

    for(int i = Memory->frente; i<Memory->final;i++) {
        if(Memory->datos[i]->id == nodePage->id) {
            deleteSpecificElementQueue(Memory, nodePage->id);
            enqueue(Memory, nodePage);
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
}

void bringPageToMemoryLRU(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if(nodePage != NULL) {
        bringFromDisk(nodePage);
        insertionPageLRU(Memory, nodePage, diskTable, ptrDiskAddr);

    }
}