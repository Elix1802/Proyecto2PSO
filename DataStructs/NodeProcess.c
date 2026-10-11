#include <stdlib.h>
#include <stdio.h>
#include "NodePage.h"
#include "NodeProcess.h"
#define ARRAY_SIZE 1024

struct NodeProcess {
    int id;
    struct NodePage ** pages;
    int pageSize;
};

NodeProcess * createNodeProcess(int pid) {
    NodeProcess * nodeProcess = (NodeProcess*) malloc(sizeof(NodeProcess));
    if(nodeProcess==NULL) return NULL;
    nodeProcess->id = pid;
    nodeProcess->pageSize = 4;

    nodeProcess->pages = (NodePage**) malloc(ARRAY_SIZE*sizeof(NodePage*));
    if(nodeProcess->pages == NULL) return NULL;

    for(int i=0; i<ARRAY_SIZE; i++) {
        nodeProcess->pages[i] = NULL;
    }

    return nodeProcess;
}

void destroyNodeProcess(NodeProcess * self) {
    if(self != NULL) {
        
    }
}