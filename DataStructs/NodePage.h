#ifndef NODEPAGE_H
#define NODEPAGE_H

struct NodePage {
    int id;
    int parentId;
    int loaded; //1 if process is in RAM otherwise 0
    int L_Address;
    int M_Address;
    int D_Address;
    struct NodePage * nextPtr;
    struct NodePage * pastPtr;
};

typedef struct NodePage NodePage;

NodePage * createNodePage();

void destroyNodePage(NodePage * self);

//
NodePage * getNextPtr(NodePage * self);
NodePage * getPastPtr(NodePage * self);

//Puts
void putNextPtr(NodePage * self, NodePage * nextPtr);
void putPastPtr(NodePage * self, NodePage * pastPtr);

//print
void printNodePage(NodePage * self);
#endif