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
  DLinkedList * linkedListRAM; 
  SymbolsTable * symbolsTable; 
  ProcessTable * processTable; 
  PtrEntry * ptrs; 
};  

MMU * createMMU(int algID, int totalMemory, int pageSize) {
    if (algID < 0) return NULL;
    if (pageSize <= 0 || totalMemory < pageSize) return NULL;
    if (totalMemory % pageSize != 0) return NULL;

    MMU * mmu = (MMU *) malloc(sizeof(MMU));
    if (mmu == NULL) return NULL;

    mmu->totalMemory = totalMemory;
    mmu->availableMemory = totalMemory;
    mmu->pageSize = pageSize;
    mmu->algID = algID;
    mmu->ptrs = NULL;
    mmu->linkedListRAM = NULL;

    mmu->pageTable = createDLinkedList();
    if (mmu->pageTable == NULL) {
        free(mmu);
        return NULL;
    }

    mmu->processTable = createProcessTable();
    if (mmu->processTable == NULL) {
        destroyDLinkedList(mmu->pageTable);
        free(mmu);
        return NULL;
    }

    mmu->symbolsTable = createSymbolsTable();
    if (mmu->symbolsTable == NULL) {
        destroyDLinkedList(mmu->pageTable);
        destroyProcessTable(mmu->processTable);
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
    free(self);
}

int newP(MMU *mmu, int pid, int size) {
    if (mmu == NULL || size <= 0) {
        return -1;
    }

    int num_pages = (size + mmu->pageSize - 1) / mmu->pageSize;

    static int global_ptr_counter = 1;
    int assigned_ptr = global_ptr_counter++;

    DLinkedList *page_list = createDLinkedList();
    if (page_list == NULL) {
        return -1;
    }

    for (int i = 0; i < num_pages; i++) {
        static int global_page_id_counter = 1;
        int page_id = global_page_id_counter++;

        int total_frames = mmu->totalMemory / mmu->pageSize;
        int physical_segment = (page_id - 1) % total_frames;

        NodePage *new_page = createNodePage(
            page_id,
            pid,
            1,
            i * mmu->pageSize,
            physical_segment,
            -1,
            NULL,
            NULL
        );

        if (new_page == NULL) {
            destroyDLinkedList(page_list);
            return -1;
        }

        d_linked_list_add(page_list, new_page);
        d_linked_list_add(mmu->pageTable, new_page);
    }

    int registered = symbols_table_insert(mmu->symbolsTable, pid, assigned_ptr, page_list);
    if (!registered) {
        destroyDLinkedList(page_list);
        return -1;
    }

    return assigned_ptr;
}

void useP(MMU * self, int ptr) {
}

void deleteP(MMU * self, int ptr) {
}

void killP(MMU * self, int pid) {
}