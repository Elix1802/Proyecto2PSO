#ifndef SecondChance_H
#define SecondChance_H
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"


void insertionPageSecondChance(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr);
void bringPageToMemorySecondChance(Queue * Memory, DiskTable * diskTable, int memoryPos, int *  ptrDiskAddr);
#endif