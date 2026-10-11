#include <stdlib.h>
#include <stdio.h>
#include "NodePage.h"
#include "NodeProcess.h"
#include "DLinkedList.h"


struct NodeProcess {
    int id;
    struct DLinkedList * pages;
    int pageSize;
};

NodeProcess * createNodeProcess(int pid) {
    NodeProcess * nodeProcess = (NodeProcess*) malloc(sizeof(NodeProcess));
    if(nodeProcess==NULL) return NULL;
    nodeProcess->id = pid;
    nodeProcess->pageSize = 4;

    nodeProcess->pages = createDLinkedList();

    return nodeProcess;
}

//add
void addNodePageToProcess(NodeProcess * self, int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr){
    if(self != NULL) {
        append(self->pages, pageId, parentId, loaded, L_Addr, M_Addr, D_addr);
    }
}

void destroyNodeProcess(NodeProcess * self) {
    if(self != NULL) {
        free(self);
    }
}


