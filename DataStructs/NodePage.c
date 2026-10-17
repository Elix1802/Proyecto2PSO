#include <stdio.h>
#include <stdlib.h>
#include "NodePage.h"


NodePage * createNodePage(int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr, int loaded_T, NodePage * nextPtr, NodePage * pastPtr) {
    NodePage * nodePage = (NodePage*) malloc(sizeof(NodePage));
    if (nodePage == NULL) return NULL;
    
    nodePage->id = pageId;
    nodePage->parentId = parentId;
    nodePage->loaded = loaded;
    nodePage->L_Address = L_Addr;
    nodePage->M_Address = M_Addr; //-1 Todavia no tiene asignado espacio
    nodePage->D_Address = D_addr; //-1 Todavia no tiene asignado espacio
    nodePage->loaded_T = loaded_T;
    nodePage->Mark = 0;
    nodePage->nextPtr = nextPtr;
    nodePage->pastPtr = pastPtr;

    nodePage->freq = 0;
    nodePage->frag = 0;

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
    return NULL;
}
NodePage * getPastPtr(NodePage * self) {
    if(self != NULL) {
        return self->pastPtr;
    }
    return NULL;
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

void putOnVirtualMemory(NodePage * self, int D_addr) {
    if(self != NULL) {
        self->D_Address = D_addr;
        self->loaded = 0;
        self->freq = 0;
    }
}

void bringFromDisk(NodePage * self) {
    if(self != NULL) {
        self->D_Address = -1;
        self->loaded = 1;
    }
}

void addTimeOnNodePage(NodePage * self, int time) {
    if(self!=NULL) {
        self->loaded_T++;
    }
}

void addOneMoreFreq(NodePage * self) {
    if(self!=NULL) {
        self->freq ++;
    }
}
//print
void printNodePage(NodePage * self) {
    if(self != NULL) {
        int pastNodePageId = (self->pastPtr != NULL) ? self->pastPtr->id : -1;
        int nextNodePageId = (self->nextPtr != NULL) ? self->nextPtr->id : -1;
        printf("\npageId: %d ; parentId: %d ; loaded: %d ; L_Address: %d ; M_Address: %d ; D_Address: %d; loaded_T: %d ; frequency : %d; pastNodePageId : %d ; nextNodePageId : %d \n", self->id, self->parentId, self->loaded, self->L_Address, self->M_Address, self->D_Address, self->loaded_T, self->freq ,pastNodePageId, nextNodePageId);
        return;
    }
    
    printf("No page found");
    
}
