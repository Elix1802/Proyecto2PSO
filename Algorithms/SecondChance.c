#include <stdio.h>
#include <stdlib.h>
#include "../DataStructs/MMU.h"
#include "SecondChance.h"


void insertionPageSecondChance(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, MMU * mmu){
    
    if(Memory->cantidad < 100) {
        enqueue(Memory, nodePage);
        mmu->elapsedTime+=5;
        mmu->ramKB += mmu->pageSize;
        mmu->ramPct = mmu->ramKB * 100 / (double) mmu->totalMemory;
        mmu->loaded++;
        return;
    }

    for (int i = Memory->frente; i < Memory->final; i++) {
        if (Memory->datos[i]->id == nodePage->id)  {
            Memory->datos[i]->Mark = 1;
            mmu->elapsedTime++;
            return;
        }
        
    }
    
    while (1) {
        NodePage* candidateNode = dequeue(Memory);
        if (candidateNode->Mark == 1) {
            candidateNode->Mark = 0;
            enqueue(Memory, candidateNode);
        }
        else {
            enqueue(Memory, nodePage);
            putOnVirtualMemory(candidateNode, *ptrDiskAddr);
            addDiskTableElement(diskTable, *ptrDiskAddr, candidateNode);
            *ptrDiskAddr = (*ptrDiskAddr) + 1;
            mmu->elapsedTime+=5;
            return;
        }
        
    }
}

void bringPageToMemorySecondChance(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if(nodePage != NULL) {
        mmu->thrashTime+=5;
        mmu->thrashPct = mmu->thrashTime * 100 / (double) mmu->elapsedTime;  
        mmu->vramKB -= mmu->pageSize;
        mmu->vramPct = mmu->vramKB * 100 / 512.00;
        mmu->unloaded--;
        bringFromDisk(nodePage);
        insertionPageSecondChance(Memory, nodePage, diskTable, ptrDiskAddr, mmu);

    }
}