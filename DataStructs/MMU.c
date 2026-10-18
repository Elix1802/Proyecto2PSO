#include <stdlib.h>
#include "MMU.h"
#include "DLinkedList.h"
#include "NodePage.h"
#include "NodeProcess.h"
#include "SymbolsTable.h"
#include "ProcessTable.h"
#include "DiskTable.h"
#include "Queue.h"
#include "../Algorithms/FIFO.h"
#include "../Algorithms/SecondChance.h"
#include "../Algorithms/LRU.h"
#include "../Algorithms/LFU.h"
#include "../Algorithms/OPT.h"

MMU * createMMU(int algID, int totalMemory, int pageSize) {
    if (algID < OPT || algID > LFU) return NULL;
    if (pageSize <= 0 || totalMemory < pageSize) return NULL;
    if (totalMemory % pageSize != 0) return NULL;
    if (totalMemory / pageSize > 100) return NULL;

    MMU * mmu = (MMU *) malloc(sizeof(MMU));
    if (mmu == NULL) return NULL;

    mmu->totalMemory = totalMemory;
    mmu->availableMemory = totalMemory;
    mmu->pageSize = pageSize;
    mmu->algID = algID;
    mmu->elapsedTime = 0;
    mmu->nextPageId = 1;
    mmu->nextDiskId = 100;
    mmu->frameCount = totalMemory / pageSize;

    mmu->wastedMemory = 0.0;
    mmu->processes = 0;
    mmu->ramKB = 0;
    mmu->ramPct = 0;
    mmu->vramKB = 0;
    mmu->vramPct = 0;
    mmu->loaded = 0; 
    mmu->unloaded = 0;
    mmu->thrashTime = 0;
    mmu->thrashPct = 0;
    mmu->fragmentationKB = 0;

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

    mmu->diskTable = createDiskTable();
    if (mmu->diskTable == NULL) {
        destroySymbolsTable(mmu->symbolsTable);
        destroyProcessTable(mmu->processTable);
        destroyDLinkedList(mmu->pageTable);
        free(mmu->frames);
        free(mmu);
        return NULL;
    }

    mmu->actualTimeMoment = 0;
    mmu->sizeDefault = 0;
    mmu->futurePages = NULL;

    if (algID == OPT) {
        mmu->sizeDefault = 255;
        mmu->futurePages = (NodePage **) calloc(mmu->sizeDefault, sizeof(NodePage *));
    }

    Queue * queue = (Queue*) malloc(sizeof(Queue));
    inicializar(queue);
    mmu->MemoryRAM =queue; 

    return mmu;
}

int addProcess(MMU * self, int processId) {
    if (self == NULL) return 0;
    if (getProcessTableValue(self->processTable, processId) != NULL) return 0;

    NodeProcess * newProcess = createNodeProcess(processId);
    if (newProcess == NULL) return 0;

    if(addProcessTableElement(self->processTable, processId, newProcess) == 0) {
        destroyNodeProcess(newProcess);
        return 0;
    }
    return 1; 
}

DLinkedList * getPages(MMU * self, int processId) {
    if (self == NULL) return NULL;
    if (getProcessTableValue(self->processTable, processId) == NULL) return NULL;

    NodeProcess * process = getProcessTableValue(self->processTable, processId);
    return getAllProcessPages(process);
}

void destroyMMU(MMU * self) {
    if (self == NULL) return;

    destroySymbolsTable(self->symbolsTable);
    destroyDiskTable(self->diskTable);
    destroyProcessTable(self->processTable);
    destroyDLinkedList(self->pageTable);
    free(self->MemoryRAM);

    if (self->futurePages != NULL) {
        for (int i = 0; i < self->sizeDefault; i++) {
            destroyNodePage(self->futurePages[i]);
        }
        free(self->futurePages);
    }

    free(self->frames);
    free(self);
}

void updateStats(MMU * self) {
    if (self == NULL) return;
    self->loaded = self->MemoryRAM->cantidad;
    self->availableMemory = self->totalMemory - self->loaded * self->pageSize;
    self->ramKB = self->loaded * self->pageSize / 1024.0;
    self->ramPct = self->loaded * 100.0 / self->frameCount;
    self->vramKB = self->unloaded * self->pageSize / 1024.0;
    self->vramPct = self->unloaded * 100.0 / self->frameCount;
    self->thrashPct = self->elapsedTime > 0 ? self->thrashTime * 100.0 / self->elapsedTime : 0.0;
}

void mmuPlacePage(MMU * self, NodePage * page) {
    for (int f = 0; f < self->frameCount; f++) {
        if (self->frames[f] == NULL) {
            self->frames[f] = page;
            page->M_Address = f;
            return;
        }
    }
}

void mmuReplacePage(MMU * self, NodePage * victim, NodePage * incoming) {
    int f = victim->M_Address;
    victim->M_Address = -1;
    if (f >= 0 && f < self->frameCount) {
        self->frames[f] = incoming;
        incoming->M_Address = f;
    }
}

void mmuReleasePage(MMU * self, NodePage * page) {
    if (page->M_Address >= 0 && page->M_Address < self->frameCount) {
        self->frames[page->M_Address] = NULL;
    }
    page->M_Address = -1;
}

int predictNew(MMU * self, int size) {
    if(self != NULL) {
        int numPages = size / self->pageSize + (size % self->pageSize != 0);
        int l_addr = self->nextPageId; 
        for(int i = 0; i<numPages; i++) {
            self->futurePages[self->actualTimeMoment++] = createNodePage(self->nextPageId++, -10, -10, l_addr, -10, -10, -10, NULL, NULL);
            increaseFuturePredict(self); 
        }

        return l_addr;
    }

    return -1;
}

int predictUse(MMU * self, int logicId) {
    if(self == NULL) return -1;

    int lastActualTimeMoment = self->actualTimeMoment;

    for(int i = 0; i < lastActualTimeMoment; i++) {
        if(self->futurePages[i]->L_Address == logicId && self->futurePages[i]->Mark == 0) {
            NodePage * newPage = createNodePage(self->futurePages[i]->id, -10, -10, logicId, -10, -10, -10, NULL, NULL);
            newPage->Mark = 1;
            self->futurePages[self->actualTimeMoment++] = newPage;
            increaseFuturePredict(self);
        }
    }
    return 0;
}

void restoreIdPages(MMU * self) {
    if(self!=NULL) {
        self->nextPageId = 1;
        self->actualTimeMoment = 0;
    }
}

void printPredicts(MMU * self) {
    if(self != NULL) {
        int i = 0;
        int lastActualTimeMoment = self->actualTimeMoment;
        for(i = 0; i<lastActualTimeMoment; i++) {
            printNodePage(self->futurePages[i]);
        }
    }
}

void increaseFuturePredict(MMU * self) {
    if(self->actualTimeMoment >= self->sizeDefault) {
        int lastTime = self->sizeDefault;
        self->sizeDefault *= 2;
        NodePage ** temp = (NodePage**) malloc(self->sizeDefault * sizeof(NodePage*));
        for(int i = 0; i < self->sizeDefault; i++) {
            if(i < lastTime) temp[i] = self->futurePages[i];
            else temp[i] = NULL;
        }

        free(self->futurePages);
        self->futurePages = temp;
    }
}

int newP(MMU * self, int pid, int size) {
    if (self == NULL || size <= 0) return -1;

    int numPages = size / self->pageSize + (size % self->pageSize != 0);
    double fragmentation = (numPages * self->pageSize - size) / 1024.0;
    self->fragmentationKB += fragmentation;

    NodeProcess * newProcess = getProcessTableValue(self->processTable, pid);
    if (newProcess == NULL) {
        newProcess = createNodeProcess(pid);
        if (newProcess == NULL) return -1;
        addProcessTableElement(self->processTable, pid, newProcess);
        self->processes++;
        combineDLinkedList(self->pageTable, newProcess->pages);
    }

    int l_addr = self->nextPageId;
    for (int i = 0; i < numPages; i++) {

        NodePage * newNode = createNodePage(self->nextPageId++, pid, 1, l_addr, -1, -1, 0, NULL, NULL);
        if (i == numPages - 1) {
            newNode->frag = fragmentation;
        }

        addNodePageToProcess(newProcess, newNode);
        self->pageTable->size++;
        addSymbolsTableElement(self->symbolsTable, l_addr, newNode);

        if (self->algID == 1) {
            insertionPageOPT(self->MemoryRAM, newNode, self->diskTable, &self->nextDiskId, self->futurePages, self);
        }
        else if (self->algID == 2) {
            insertionPageFIFO(self->MemoryRAM, newNode, self->diskTable, &self->nextDiskId, self);
        }
        else if (self->algID == 3) {
            insertionPageSecondChance(self->MemoryRAM, newNode, self->diskTable, &self->nextDiskId, self);
        }
        else if (self->algID == 4) {
            insertionPageLRU(self->MemoryRAM, newNode, self->diskTable, &self->nextDiskId, self);
        }
        else if (self->algID == 5) {
            insertionPageLFU(self->MemoryRAM, newNode, self->diskTable, &self->nextDiskId, self);
        }

        self->actualTimeMoment++;
    }

    updateStats(self);
    return l_addr;
}

void useP(MMU * self, int ptr) {
    if (self == NULL) return;

    NodePage * nodePage = getSymbolsTableValue(self->symbolsTable, ptr);

    while (nodePage != NULL && nodePage->id != -2 && nodePage->L_Address == ptr) {
        self->actualTimeMoment++;
        if (self->algID == 3) {
            nodePage->Mark = 1;
        }

        if (nodePage->loaded == 0) {
            switch (self->algID) {
                case 1:
                    bringPageToMemoryOPT(self->MemoryRAM, self->diskTable, nodePage->D_Address, &self->nextDiskId, self->futurePages, self);
                    break;
                case 2:
                    bringPageToMemoryFIFO(self->MemoryRAM, self->diskTable, nodePage->D_Address, &self->nextDiskId, self);
                    break;
                case 3:
                    bringPageToMemorySecondChance(self->MemoryRAM, self->diskTable, nodePage->D_Address, &self->nextDiskId, self);
                    break;
                case 4:
                    bringPageToMemoryLRU(self->MemoryRAM, self->diskTable, nodePage->D_Address, &self->nextDiskId, self);
                    break;
                case 5:
                    bringPageToMemoryLFU(self->MemoryRAM, self->diskTable, nodePage->D_Address, &self->nextDiskId, self);
                    break;
                default:
                    break;
            }
        } else {
            self->elapsedTime += 1;
            addOneMoreFreq(nodePage);
            if (self->algID == 4) {
                deleteSpecificElementQueue(self->MemoryRAM, nodePage->id);
                enqueue(self->MemoryRAM, nodePage);
            }
        }

        nodePage = getNextPtr(nodePage);
    }

    updateStats(self);
}

static void removePagesFromTable(MMU * self, int byProcess, int key) {
    goToStart(self->pageTable);

    while (!atEnd(self->pageTable)) {
        NodePage * nodoActual = getElement(self->pageTable);
        int match = 0;

        if (nodoActual != NULL && nodoActual->id >= 0) {
            match = byProcess ? (nodoActual->parentId == key) : (nodoActual->L_Address == key);
        }

        if (match) {
            NodePage * eliminado = removeNode(self->pageTable);
            if (eliminado != NULL) {
                self->fragmentationKB -= eliminado->frag;
                NodeProcess * owner = getProcessTableValue(self->processTable, eliminado->parentId);
                if (owner != NULL) {
                    owner->pages->size--;
                    owner->pagesNumber--;
                }
                if (eliminado->loaded == 0) {
                    getDiskTableValue(self->diskTable, eliminado->D_Address);
                    self->unloaded--;
                }
                destroyNodePage(eliminado);
            }
        } else {
            nextNodePage(self->pageTable);
        }
    }
}

void deleteP(MMU * self, int ptr) {
    if (self == NULL) return;

    NodePage * nodePage = popSymbolsTableElement(self->symbolsTable, ptr);
    if (nodePage == NULL) return;

    while (nodePage != NULL && nodePage->id != -2 && nodePage->L_Address == ptr) {
        if (nodePage->loaded == 1) {
            deleteSpecificElementQueue(self->MemoryRAM, nodePage->id);
            mmuReleasePage(self, nodePage);
        }
        nodePage = getNextPtr(nodePage);
    }

    removePagesFromTable(self, 0, ptr);
    updateStats(self);
}

void killP(MMU * self, int pid) {
    if (self == NULL) return;

    NodeProcess * process = getProcessTableValue(self->processTable, pid);
    if (process == NULL) return;

    DLinkedList * pages = getAllProcessPages(process);
    if (pages != NULL) {
        goToStart(pages);
        NodePage * page = getElement(pages);
        while (page != NULL && page->id != -2) {
            if (page->loaded == 1) {
                deleteSpecificElementQueue(self->MemoryRAM, page->id);
                mmuReleasePage(self, page);
            }
            if (page->L_Address == page->id) {
                NodePage * symbol = popSymbolsTableElement(self->symbolsTable, page->L_Address);
                (void) symbol;
            }
            page = getNextPtr(page);
        }
    }

    removePagesFromTable(self, 1, pid);
    self->processes--;
    updateStats(self);
}