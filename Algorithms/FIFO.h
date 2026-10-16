#ifndef FIFO_H
#define FIFO_H
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/Queue.h"


void insertionPageFIFO(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr);
void bringPageToMemoryFIFO(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr);

#endif