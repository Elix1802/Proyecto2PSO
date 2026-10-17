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

struct MMU {
  int availableMemory;
  int totalMemory;
  int pageSize;
  int algID;
  int elapsedTime;
  int trashingTime;
  DLinkedList * pageTable;
  NodePage ** frames;
  int frameCount; //Cantidad de marcos
  SymbolsTable * symbolsTable;
  ProcessTable * processTable;
  DiskTable * diskTable;

  Queue * MemoryRAM;

  //Predicción del futuro
  NodePage ** futurePages; //Nodos peones que se usarán a futuro
  int actualTimeMoment;
  int sizeDefault; //Tamaño por default del future pages

  int nextPageId; //id de cada página 
  int nextDiskId;//Id de cada elemento en la memoria virtual

  double wastedMemory;

  //Estadísticas
  

};

typedef struct MMU MMU;

MMU * createMMU(int algID, int totalMemory, int pageSize);
void destroyMMU(MMU * self);

int addProcess(MMU * self, int processId);
DLinkedList* getPages(MMU * self, int processId);

// Instrucciones
int newP(MMU * self, int pid, int size);
void useP(MMU * self, int ptr);
void deleteP(MMU * self, int ptr);
void killP(MMU * self, int pid);

//Predicts:
int predictNew(MMU * self, int size);
int predictUse(MMU * self, int logicId);
void restoreIdPages(MMU * self);
void printPredicts(MMU * self);
void increaseFuturePredict(MMU * self);
#endif // MMU_H

