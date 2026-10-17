#include <stdio.h>
#include <stdlib.h>
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/MMU.h"


void insertionPageFIFO(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, MMU * mmu){

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
            mmu->elapsedTime++;    
            return;
        }
    }

    
    NodePage * oldestNode = dequeue(Memory);

    putOnVirtualMemory(oldestNode, *ptrDiskAddr);

    addDiskTableElement(diskTable, *ptrDiskAddr, oldestNode);

    nodePage->M_Address = Memory->final;
    enqueue(Memory, nodePage);

    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->elapsedTime+=5;
    mmu->vramKB += mmu->pageSize;
    mmu->vramPct = mmu->vramKB * 100 / 512.00;

    mmu->unloaded++;


}

void bringPageToMemoryFIFO(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, MMU*mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if(nodePage != NULL) {
        mmu->thrashTime+=5;
        mmu->thrashPct = mmu->thrashTime * 100 / (double) mmu->elapsedTime;  
        mmu->vramKB -= mmu->pageSize;
        mmu->vramPct = mmu->vramKB * 100 / 512.00;
        mmu->unloaded--;
        
        bringFromDisk(nodePage);
        insertionPageFIFO(Memory, nodePage, diskTable, ptrDiskAddr, mmu);

    }
}