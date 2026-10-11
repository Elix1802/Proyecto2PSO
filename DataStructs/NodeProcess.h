#ifndef NODEPROCESS_H
#define NODEPROCESS_H

typedef struct NodeProcess NodeProcess;

NodeProcess * createNodeProcess(int pid);

void destroyNodeProcess(NodeProcess * self);


#endif