#ifndef MMU_H
#define MMU_H
#include "DLinkedList.h"
#include "NodePage.h"
#include "NodeProcess.h"
#include "SymbolsTable.h"
#include "ProcessTable.h"
#include "DiskTable.h"
#include "Queue.h"
#include "../Algorithms/FIFO.h"
#include "../Algorithms/SecondChance.h"

enum AlgorithmID {
    OPT  = 1,
    FIFO = 2,
    SC   = 3,
    LRU  = 4,
    LFU  = 5
};

typedef struct PtrEntry {
    int ptr, pid, size, nPages;   
    NodePage ** pages;
    struct PtrEntry * next;
} PtrEntry;

struct MMU {
  int availableMemory;
  int totalMemory;
  int pageSize;
  int algID;
  DLinkedList * pageTable;
  NodePage ** frames;
  int frameCount; //Cantidad de marcos
  SymbolsTable * symbolsTable;
  ProcessTable * processTable;
  DiskTable * diskTable;
  PtrEntry * ptrs;

  Queue * MemoryFIFOSC;

  int nextPtr;
  int nextPageId;
  int nextDiskId;//Id de cada elemento en la memoria virtual

  double wastedMemory;

};

typedef struct MMU MMU;

MMU * createMMU(int algID, int totalMemory, int pageSize);
void destroyMMU(MMU * self);

int addProcess(MMU * self, int processId);
DLinkedList* getPages(MMU * self, int processId);

//Posibles Instrucciones
int newP(MMU * self, int pid, int size);
void useP(MMU * self, int ptr);
void deleteP(MMU * self, int ptr);
void killP(MMU * self, int pid);

#endif // MMU_H