#ifndef MMU_H
#define MMU_H
#include "DLinkedList.h"
#include "NodePage.h"
#include "NodeProcess.h"
#include "SymbolsTable.h"
#include "ProcessTable.h"
#include "DiskTable.h"
#include "Queue.h"

enum AlgorithmID {
    OPT  = 1,
    FIFO = 2,
    SC   = 3,
    LRU  = 4,
    LFU  = 5
};

struct MMU {
  int availableMemory;
  int totalMemory;
  int pageSize;
  int algID;
  int elapsedTime;
  DLinkedList * pageTable;
  NodePage ** frames;
  int frameCount;
  SymbolsTable * symbolsTable;
  ProcessTable * processTable;
  DiskTable * diskTable;

  Queue * MemoryRAM;

  NodePage ** futurePages;
  int actualTimeMoment;
  int sizeDefault;

  int nextPageId;
  int nextDiskId;

  double wastedMemory;

  int processes;
  double ramKB, ramPct;
  double vramKB, vramPct;
  int loaded, unloaded;
  double thrashTime, thrashPct;
  double fragmentationKB;
};

typedef struct MMU MMU;

MMU * createMMU(int algID, int totalMemory, int pageSize);
void destroyMMU(MMU * self);

int addProcess(MMU * self, int processId);
DLinkedList* getPages(MMU * self, int processId);

int newP(MMU * self, int pid, int size);
void useP(MMU * self, int ptr);
void deleteP(MMU * self, int ptr);
void killP(MMU * self, int pid);

int predictNew(MMU * self, int size);
int predictUse(MMU * self, int logicId);
void restoreIdPages(MMU * self);
void printPredicts(MMU * self);
void increaseFuturePredict(MMU * self);

void updateStats(MMU * self);
void mmuPlacePage(MMU * self, NodePage * page);
void mmuReplacePage(MMU * self, NodePage * victim, NodePage * incoming);
void mmuReleasePage(MMU * self, NodePage * page);
#endif