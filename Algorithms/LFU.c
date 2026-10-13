#include <stdio.h>
#include <stdlib.h>
#include "LFU.h"
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"


void insertionPageLFU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr){

    if(Memory->cantidad < 3) {
        nodePage->M_Address = Memory->final;
        addOneMoreFreq(nodePage);
        enqueue(Memory, nodePage);
        return;
    }

    for(int i = Memory->frente; i<Memory->final;i++) {
        if(Memory->datos[i]->id == nodePage->id) {
            addOneMoreFreq(nodePage);
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
}

void bringPageToMemoryLFU(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if(nodePage != NULL) {
        bringFromDisk(nodePage);
        insertionPageLFU(Memory, nodePage, diskTable, ptrDiskAddr);

    }
}