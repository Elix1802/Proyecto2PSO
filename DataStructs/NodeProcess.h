#ifndef NODEPROCESS_H
#define NODEPROCESS_H
#include "NodePage.h"
#include "DLinkedList.h"

typedef struct NodeProcess NodeProcess;

NodeProcess * createNodeProcess(int pid);

void destroyNodeProcess(NodeProcess * self);

//get
NodePage ** getAllProcessPages(NodeProcess * self);

int getNumberPages(NodeProcess * self);
int getNumberLogicalAddressPages(NodeProcess * self, int L_Addr);
NodePage ** getAllLogicalAddressPages(NodeProcess * self, int L_Addr, int size);


//add
void addNodePageToProcess(NodeProcess * self, int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr);

#endif