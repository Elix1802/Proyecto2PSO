#include <stdlib.h>
#include "MMU.h"
#include "DLinkedList.h"

typedef struct ProcEntry {
    int pid;
    struct ProcEntry * next;
} ProcEntry;

struct MMU {
  int availableMemory;
  int totalMemory;
  int pageSize;
  int algID;
  ProcEntry * processes;
  DLinkedList * pageTable;
};

MMU * createMMU(int algID, int totalMemory, int pageSize) {
    if (algID < OPT || algID > LFU) return NULL;
    if (pageSize <= 0 || totalMemory < pageSize) return NULL;
    if (totalMemory % pageSize != 0) return NULL;

    MMU * mmu = (MMU *) malloc(sizeof(MMU));
    if (mmu == NULL) return NULL;

    mmu->pageTable = createDLinkedList();
    if (mmu->pageTable == NULL) {
        free(mmu);
        return NULL;
    }

    mmu->totalMemory = totalMemory;
    mmu->availableMemory = totalMemory;
    mmu->pageSize = pageSize;
    mmu->algID = algID;
    mmu->processes = NULL;
    return mmu;
}

static ProcEntry * findProcess(MMU * self, int processId) {
    ProcEntry * p = self->processes;
    while (p != NULL) {
        if (p->pid == processId) return p;
        p = p->next;
    }
    return NULL;
}

int addProcess(MMU * self, int processId) {
    if (self == NULL) return 0;
    if (findProcess(self, processId) != NULL) return 0;

    ProcEntry * p = (ProcEntry *) malloc(sizeof(ProcEntry));
    if (p == NULL) return 0;

    p->pid = processId;
    p->next = self->processes;
    self->processes = p;
    return 1;
}

NodePage ** getPages(MMU * self, int processId, int * count) {
    if (count != NULL) *count = 0;
    if (self == NULL) return NULL;
    if (findProcess(self, processId) == NULL) return NULL;

    DLinkedList * list = self->pageTable;
    NodePage * page;

    int total = 0;
    goToStart(list);
    while ((page = getElement(list)) != NULL) {
        if (page->parentId == processId) total++;
        nextNodePage(list);
    }
    if (total == 0) return NULL;

    NodePage ** result = (NodePage **) malloc(total * sizeof(NodePage *));
    if (result == NULL) return NULL;

    int i = 0;
    goToStart(list);
    while ((page = getElement(list)) != NULL) {
        if (page->parentId == processId) result[i++] = page;
        nextNodePage(list);
    }

    if (count != NULL) *count = total;
    return result;
}

void destroyMMU(MMU * self) {
    if (self == NULL) return;

    ProcEntry * p = self->processes;
    while (p != NULL) {
        ProcEntry * next = p->next;
        free(p);
        p = next;
    }
    destroyDLinkedList(self->pageTable);
    free(self);
}