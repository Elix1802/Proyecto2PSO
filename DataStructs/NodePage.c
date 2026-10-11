#include <stdio.h>
#include <stdlib.h>
#include "NodePage.h"


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

void destroyNodePage(NodePage * self) {
    if(self != NULL) free(self);
}
 
//gets
NodePage * getNextPtr(NodePage * self) {
    if(self != NULL) {
        return self->nextPtr;
    }
}
NodePage * getPastPtr(NodePage * self) {
    if(self != NULL) {
        return self->pastPtr;
    }
}

//Puts
void putNextPtr(NodePage * self, NodePage * nextPtr) {
    if(self != NULL) {
        self->nextPtr = nextPtr;
    }
}

void putPastPtr(NodePage * self, NodePage * pastPtr) {
    if(self != NULL) {
        self->pastPtr = pastPtr;
    }
}


//print
void printNodePage(NodePage * self) {
    if(self != NULL) {
        int pastNodePageId = (self->pastPtr != NULL) ? self->pastPtr->id : -1;
        int nextNodePageId = (self->nextPtr != NULL) ? self->nextPtr->id : -1;
        printf("\npageId: %d ; parentId: %d ; loaded: %d ; L_Address: %d ; M_Address: %d ; D_Address: %d ; pastNodePageId : %d ; nextNodePageId : %d \n", self->id, self->parentId, self->loaded, self->L_Address, self->M_Address, self->D_Address, pastNodePageId, nextNodePageId);
        return;
    }
    
    printf("No page found");
    
}
