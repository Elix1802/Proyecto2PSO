#include <stdio.h>
#include <stdlib.h>
#include "OPT.h"
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/Stack.h"


void insertionPageOPT(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, NodePage ** futurePredicts, MMU * mmu){

    Stack * stack = createStack();
    if(Memory->cantidad < 100) {
        nodePage->M_Address = Memory->final;
        enqueue(Memory, nodePage);
        mmu->elapsedTime+=5;
        return;
    }

    for(int i = Memory->frente; i<Memory->final;i++) {
        if(Memory->datos[i]->id == nodePage->id) {
            mmu->elapsedTime++;
            return;}
    }

    int found = 0;
    int idOldest = -1;
    for(int i = Memory->frente; i<Memory->final;i++) {
        for(int j = mmu->actualTimeMoment; j<mmu->sizeDefault; j++) {
            if(futurePredicts[j] == NULL) break;
            else if(Memory->datos[i]->id == futurePredicts[j]->id) {
                pushStack(stack, Memory->datos[i]->id);
                found = 1;
                break;
            }
        }
        if(found == 0) {
            idOldest = Memory->datos[i]->id;
            break;
        }

        found = 0;
    }

    NodePage * oldestNode = NULL;
    if(stack->size > 0 && idOldest == -1) {
        int lastId = popStack(stack);
        oldestNode = deleteSpecificElementQueue(Memory,lastId);

    } else {
        oldestNode = deleteSpecificElementQueue(Memory, idOldest);
    }

    putOnVirtualMemory(oldestNode, *ptrDiskAddr);

    addDiskTableElement(diskTable, *ptrDiskAddr, oldestNode);
    

    //printNodePage(nodePage);
    nodePage->M_Address = Memory->final;
    enqueue(Memory, nodePage);

    *ptrDiskAddr = (*ptrDiskAddr) + 1;
    mmu->elapsedTime+=5;

}

void bringPageToMemoryOPT(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr,NodePage ** futurePredicts, MMU * mmu ) {
    NodePage * nodePage = getDiskTableValue(diskTable, diskPos);
    if(nodePage != NULL) {
        mmu->trashingTime+=5;
        bringFromDisk(nodePage);
        insertionPageOPT(Memory, nodePage, diskTable, ptrDiskAddr, futurePredicts, mmu);

    }
}