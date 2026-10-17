#ifndef SecondChance_H
#define SecondChance_H
#include "../DataStructs/NodePage.h"
#include "../DataStructs/DLinkedList.h"
#include "../DataStructs/Queue.h"
#include "../DataStructs/DiskTable.h"

struct MMU;

void insertionPageSecondChance(Queue * Memory, NodePage * nodePage, DiskTable * diskTable, int *  ptrDiskAddr, struct MMU * mmu);
void bringPageToMemorySecondChance(Queue * Memory, DiskTable * diskTable, int diskPos, int *  ptrDiskAddr, struct MMU * mmu);
#endif