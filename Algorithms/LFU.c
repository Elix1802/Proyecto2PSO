#include <stdio.h>
#include <stdlib.h>
#include "LFU.h"
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/MMU.h"



void insertionPageLFU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, MMU * mmu){

    if(Memory->cantidad < 100) {
        nodePage->M_Address = Memory->final;
        addOneMoreFreq(nodePage);
        enqueue(Memory, nodePage);
        mmu->elapsedTime+=5;
        mmu->ramKB += mmu->pageSize;
        mmu->ramPct = mmu->ramKB * 100 / (double) mmu->totalMemory;
        mmu->loaded++;
        return;
    }

    for(int i = Memory->frente; i<Memory->final;i++) {
        if(Memory->datos[i]->id == nodePage->id) {
            addOneMoreFreq(nodePage);
            mmu->elapsedTime++;
            return;
        }
    }

    int idLFU = Memory->datos[Memory->frente]->id;
    int freq = Memory->datos[Memory->frente]->freq;
    for(int i = Memory->frente + 1; i<Memory->final;i++) {
        if(Memory->datos[i]->freq < freq) {
            idLFU = Memory->datos[i]->id;
            freq = Memory->datos[i]->freq;
        }
    }  

    NodePage * oldestNode = deleteSpecificElementQueue(Memory,idLFU);

    putOnVirtualMemory(oldestNode, *ptrDiskAddr);

    addDiskTableElement(diskTable, *ptrDiskAddr, oldestNode);

    //printNodePage(nodePage);
    nodePage->M_Address = Memory->final;
    addOneMoreFreq(nodePage);
    enqueue(Memory, nodePage);

    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->elapsedTime+=5;
    mmu->vramKB += mmu->pageSize;
    mmu->vramPct = mmu->vramKB * 100 / 512.00;

    mmu->unloaded++;

}

void bringPageToMemoryLFU(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, MMU * mmu) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if(nodePage != NULL) {
        mmu->thrashTime+=5;
        mmu->thrashPct = mmu->thrashTime * 100 / (double) mmu->elapsedTime;  
        mmu->vramKB -= mmu->pageSize;
        mmu->vramPct = mmu->vramKB * 100 / 512.00;
        mmu->unloaded--;
        bringFromDisk(nodePage);
        insertionPageLFU(Memory, nodePage, diskTable, ptrDiskAddr, mmu);

    }
}