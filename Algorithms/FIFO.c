#include <stdio.h>
#include <stdlib.h>
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"


void insertionPageFIFO(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr){

    if(Memory->cantidad < 5) {
        nodePage->M_Address = Memory->final;
        enqueue(Memory, nodePage);
        return;
    }

    for(int i = Memory->frente; i<Memory->final;i++) {
        if(Memory->datos[i]->id == nodePage->id) return;
    }

    
    NodePage * oldestNode = dequeue(Memory);

    putOnVirtualMemory(oldestNode, *ptrDiskAddr);

    addDiskTableElement(diskTable, *ptrDiskAddr, oldestNode);

    //printNodePage(nodePage);
    nodePage->M_Address = Memory->final;
    enqueue(Memory, nodePage);

    *ptrDiskAddr = (*ptrDiskAddr) + 1;
}

void bringPageToMemoryFIFO(Queue * Memory, DiskTable * diskTable, int memoryPos, int *  ptrDiskAddr) {
    NodePage * nodePage = getDiskTableValue(diskTable, memoryPos);
    if(nodePage != NULL) {
        bringFromDisk(nodePage);
        insertionPageFIFO(Memory, nodePage, diskTable, ptrDiskAddr);

    }
}