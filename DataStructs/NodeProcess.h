#ifndef NODEPROCESS_H
#define NODEPROCESS_H

typedef struct NodeProcess NodeProcess;

NodeProcess * createNodePage(int pid);

void destroyNodePage(NodeProcess * self);


#endif