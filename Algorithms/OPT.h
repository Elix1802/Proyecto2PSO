#ifndef OPT_H
#define OPT_H
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/DiskTable.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/MMU.h"

void insertionPageOPT(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, NodePage ** futurePredicts, MMU * mmu);
void bringPageToMemoryOPT(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, NodePage ** futurePredicts, MMU * mmu);

#endif