#ifndef NODEPROCESS_H
#define NODEPROCESS_H
#include "NodePage.h"
#include "DLinkedList.h"

struct NodeProcess {
    int id;
    struct DLinkedList * pages;
    int pageSize;
    int pagesNumber;
};

typedef struct NodeProcess NodeProcess;

NodeProcess * createNodeProcess(int pid);

void destroyNodeProcess(NodeProcess * self);

//get
DLinkedList * getAllProcessPages(NodeProcess * self);

int getNumberPages(NodeProcess * self);
int getNumberLogicalAddressPages(NodeProcess * self, int L_Addr);
NodePage ** getAllLogicalAddressPages(NodeProcess * self, int L_Addr, int size);
int deleteAllLogicalAddressPages(NodeProcess * self, int L_Addr);

//add
void addNodePageToProcess(NodeProcess * self, NodePage * newPage);

#endif