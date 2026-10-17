#ifndef LFU_H
#define LFU_H
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/MMU.h"


void insertionPageLFU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, MMU * mmu);
void bringPageToMemoryLFU(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, MMU * mmu);

#endif