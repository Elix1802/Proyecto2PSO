#include <stdio.h>
#include <stdlib.h>
#include "NodePage.h"

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


NodePage * createNodePage(int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr, NodePage * nextPtr, NodePage * pastPtr) {
    NodePage * nodePage = (NodePage*) malloc(sizeof(NodePage));
    if (nodePage == NULL) return NULL;
    
    nodePage->id = pageId;
    nodePage->parentId = parentId;
    nodePage->loaded = loaded;
    nodePage->L_Address = L_Addr;
    nodePage->M_Address = M_Addr;
    nodePage->D_Address = D_addr;
    nodePage->nextPtr = nextPtr;
    nodePage->pastPtr = pastPtr;

    return nodePage;
}