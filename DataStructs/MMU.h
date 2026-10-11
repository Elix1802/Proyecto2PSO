#ifndef MMU_H
#define MMU_H
#include "NodePage.h"

enum AlgorithmID {
    OPT  = 1,
    FIFO = 2,
    SC   = 3,
    LRU  = 4,
    LFU  = 5
};

typedef struct MMU MMU;

MMU * createMMU(int algID, int totalMemory, int pageSize);
void destroyMMU(MMU * self);

int addProcess(MMU * self, int processId);
NodePage ** getPages(MMU * self, int processId, int * count);

#endif // MMU_H