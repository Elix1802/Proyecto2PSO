#ifndef NODEPROCESS_H
#define NODEPROCESS_H
#include "NodePage.h"

typedef struct NodeProcess NodeProcess;

NodeProcess * createNodeProcess(int pid);

void destroyNodeProcess(NodeProcess * self);

//add
void addNodePageToProcess(NodeProcess * self, int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr);

#endif