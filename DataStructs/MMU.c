#include <stdlib.h>
#include "MMU.h"
#include "DLinkedList.h"
#include "NodePage.h"
#include "NodeProcess.h"
#include "SymbolsTable.h"
#include "ProcessTable.h"

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
  int frameCount;
  SymbolsTable * symbolsTable;
  ProcessTable * processTable;
  PtrEntry * ptrs;
  int nextPtr;
  int nextPageId;
};

MMU * createMMU(int algID, int totalMemory, int pageSize) {
    if (algID < OPT || algID > LFU) return NULL;
    if (pageSize <= 0 || totalMemory < pageSize) return NULL;
    if (totalMemory % pageSize != 0) return NULL;

    MMU * mmu = (MMU *) malloc(sizeof(MMU));
    if (mmu == NULL) return NULL;

    mmu->totalMemory = totalMemory;
    mmu->availableMemory = totalMemory;
    mmu->pageSize = pageSize;
    mmu->algID = algID;
    mmu->ptrs = NULL;
    mmu->nextPtr = 1;
    mmu->nextPageId = 1;
    mmu->frameCount = totalMemory / pageSize;

    mmu->frames = (NodePage **) calloc(mmu->frameCount, sizeof(NodePage *));
    if (mmu->frames == NULL) {
        free(mmu);
        return NULL;
    }

    mmu->pageTable = createDLinkedList();
    if (mmu->pageTable == NULL) {
        free(mmu->frames);
        free(mmu);
        return NULL;
    }

    mmu->processTable = createProcessTable();
    if (mmu->processTable == NULL) {
        destroyDLinkedList(mmu->pageTable);
        free(mmu->frames);
        free(mmu);
        return NULL;
    }

    mmu->symbolsTable = createSymbolsTable();
    if (mmu->symbolsTable == NULL) {
        destroyProcessTable(mmu->processTable);
        destroyDLinkedList(mmu->pageTable);
        free(mmu->frames);
        free(mmu);
        return NULL;
    }

    return mmu;
}


int addProcess(MMU * self, int processId) {
    return 1; 
}

NodePage ** getPages(MMU * self, int processId, int * count) {
    if (count != NULL) *count = 0;
    if (self == NULL) return NULL;
    if (findProcess(self->processTable, processId) == NULL) return NULL;

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

    destroyProcessTable(self->processTable);
    destroySymbolsTable(self->symbolsTable);

    PtrEntry * e = self->ptrs;
    while (e != NULL) {
        PtrEntry * next = e->next;
        free(e->pages);
        free(e);
        e = next;
    }

    destroyDLinkedList(self->pageTable);
    free(self->frames);
    free(self);
}

static int findFreeFrame(MMU * self) {
    for (int f = 0; f < self->frameCount; f++) {
        if (self->frames[f] == NULL) return f;
    }
    return -1;
}

static NodePage * appendPageInRam(MMU * self, int pid, int logicalAddress, int frame) {
    DLinkedList * list = self->pageTable;

    append(list, self->nextPageId, pid, 1, logicalAddress, frame, -1);
    goToEnd(list);
    previousNodePage(list);
    NodePage * page = getElement(list);
    goToStart(list);

    if (page == NULL) return NULL;

    self->nextPageId++;
    self->frames[frame] = page;
    self->availableMemory -= self->pageSize;
    return page;
}

int newP(MMU * self, int pid, int size) {
    if (self == NULL || size <= 0) return -1;

    int numPages = size / self->pageSize + (size % self->pageSize != 0);
    if (numPages > self->availableMemory / self->pageSize) return -1;

    if (getProcessTableValue(self->processTable, pid) == NULL && !addProcess(self, pid))
        return -1;

    PtrEntry * entry = (PtrEntry *) malloc(sizeof(PtrEntry));
    if (entry == NULL) return -1;

    entry->pages = (NodePage **) malloc(numPages * sizeof(NodePage *));
    if (entry->pages == NULL) {
        free(entry);
        return -1;
    }

    entry->ptr = self->nextPtr++;
    entry->pid = pid;
    entry->size = size;
    entry->nPages = 0;
    entry->next = self->ptrs;
    self->ptrs = entry;

    for (int i = 0; i < numPages; i++) {
        NodePage * page = appendPageInRam(self, pid, i * self->pageSize, findFreeFrame(self));
        if (page == NULL) return -1;
        entry->pages[entry->nPages++] = page;
    }

    return entry->ptr;
}

void useP(MMU * self, int ptr) {
}

void deleteP(MMU * self, int ptr) {
}

void killP(MMU * self, int pid) {
}