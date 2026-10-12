#ifndef LRU_H
#define LRU_H
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/Queue.h"

void insertionPageLRU(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr);
void bringPageToMemoryLRU(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr);

#endif