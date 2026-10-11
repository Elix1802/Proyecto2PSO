#ifndef FIFO_H
#define FIFO_H
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/DiskTable.h"


void insertionPageFIFO(DLinkedList * linkedList, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr);
void bringPageToMemoryFIFO(DLinkedList * Memory, DiskTable * diskTable, int memoryPos, int *  ptrDiskAddr);

#endif