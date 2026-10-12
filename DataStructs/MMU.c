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
    mmu->nextDiskId = 100;
    mmu->frameCount = totalMemory / pageSize;
    mmu->wastedMemory = 0.0;

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

    //Selección de RAM
    if(algID == 2 || algID == 3) {
        Queue * queue = (Queue*) malloc(sizeof(Queue));
        inicializar(queue);
        mmu->MemoryFIFOSC =queue; 
    }

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

    DLinkedList * list = self->pageTable;
    NodeProcess * process = getProcessTableValue(self->processTable, processId);

    DLinkedList * pages = getAllProcessPages(process);

    
    return pages;
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

    append(list, self->nextPageId, pid, 1, logicalAddress, frame, -1,1);
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
    self->wastedMemory += numPages*self->pageSize - size;

    NodeProcess * newProcess = createNodeProcess(pid);
    addProcessTableElement(self->processTable, pid, newProcess); //Tabla de procesos

    int l_addr = self->nextPageId; 
    for(int i = 0; i<numPages; i++) {

        NodePage * newNode = createNodePage(self->nextPageId++, pid, 1, l_addr, -1, -1, 0,NULL, NULL);
        addNodePageToProcess(newProcess, newNode); //Se añade la pagina al proceso
        addSymbolsTableElement(self->symbolsTable, l_addr, newNode); //SE añade la pagina a la tabla de simbolos


        if(self->algID == 2) { //2 FIFO
            insertionPageFIFO(self->MemoryFIFOSC, newNode, self->diskTable, &self->nextDiskId); //Se añade la página a ram
        }

        else if(self->algID == 3) {
            insertionPageSecondChance(self->MemoryFIFOSC, newNode, self->diskTable, &self->nextDiskId);
        }


    }
    self->pageTable = combineDLinkedList(self->pageTable, newProcess->pages);
    return l_addr;
}

void useP(MMU * self, int ptr) {
    if(self != NULL) {

        NodePage * nodePage = getSymbolsTableValue(self->symbolsTable, ptr);
        //printDLinkedList(self->pageTable);

        if(nodePage != NULL) {
            while(nodePage->id != -2) {
                printf("\nNodito\n");
                printNodePage(nodePage);

                if(self->algID == 3) {
                    nodePage->Mark = 1;
                } 

                if(nodePage->loaded == 0) {
                    switch(self->algID){
                        case 2:
                            bringPageToMemoryFIFO(self->MemoryFIFOSC, self->diskTable, nodePage->D_Address, &self->nextDiskId);
                            break;
                        case 3:
                            bringPageToMemorySecondChance(self->MemoryFIFOSC, self->diskTable, nodePage->D_Address, &self->nextDiskId);
                            break;
                        default:
                            break;
                    }   
                }

                nodePage = getNextPtr(nodePage);
            }
        }
        return;
    }
}

void deleteP(MMU * self, int ptr) {
}

void killP(MMU * self, int pid) {
}