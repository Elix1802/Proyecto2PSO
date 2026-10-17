#ifndef FIFO_H
#define FIFO_H
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/MMU.h"


void insertionPageFIFO(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, MMU * mmu);
void bringPageToMemoryFIFO(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, MMU * mmu);

#endif

