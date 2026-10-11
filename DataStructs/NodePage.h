#ifndef NODEPAGE_H
#define NODEPAGE_H

typedef struct NodePage NodePage;

NodePage * createNodePage();

void destroyNodePage(NodePage * self);

//
NodePage * getNextPtr(NodePage * self);
NodePage * getPastPtr(NodePage * self);

//Puts
void putNextPtr(NodePage * self, NodePage * nextPtr);
void putPastPtr(NodePage * self, NodePage * pastPtr);



#endif