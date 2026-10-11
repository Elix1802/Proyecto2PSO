#include <stdlib.h>
#include <stdio.h>
#include "NodePage.h"
#include "NodeProcess.h"
#include "DLinkedList.h"


struct NodeProcess {
    int id;
    struct DLinkedList * pages;
    int pageSize;
    int pagesNumber;
};

NodeProcess * createNodeProcess(int pid) {
    NodeProcess * nodeProcess = (NodeProcess*) malloc(sizeof(NodeProcess));
    if(nodeProcess==NULL) return NULL;
    nodeProcess->id = pid;
    nodeProcess->pageSize = 4;

    nodeProcess->pages = createDLinkedList();
    nodeProcess->pagesNumber = 0;

    return nodeProcess;
}

void destroyNodeProcess(NodeProcess * self) {
    if(self != NULL) {
        free(self);
    }
}

//gets
int getNumberPages(NodeProcess * self) {
    if(self != NULL) {
        return self->pagesNumber;
    }
}

NodePage ** getAllProcessPages(NodeProcess * self) {
    if(self != NULL) {
        int pagesNum = getSize(self->pages);
        if(pagesNum > 0) {
            NodePage ** pages = (NodePage**) malloc(pagesNum*sizeof(NodePage*));
            goToStart(self->pages);
            
            int i = 0;
            while(!atEnd(self->pages)) {
                pages[i] = getElement(self->pages);
                nextNodePage(self->pages);
                i++;
            }

            return pages;
        }

        return NULL;
    }

    return NULL;
}

/**
 * Retorna el numero de páginas asociados a una dirección lógica, se usa
 * antes de getAllLogicalAddressPages
 */
int getNumberLogicalAddressPages(NodeProcess * self, int L_Addr) {
    if(self!=NULL) {
        int pagesNum = getSize(self->pages);
        if(pagesNum > 0) {
            goToStart(self->pages);
            int i = 0;
            while(!atEnd(self->pages)) {
                NodePage * actualPage = getElement(self->pages);
                if(actualPage->L_Address == L_Addr) {
                    i++;
                }
                nextNodePage(self->pages);
            }
            return i;
        }
        return 0;
    }

    return -1;
}

/**
 * Función que retorna todas las paginas asociadas a un id logico
 * Siendo L_Addr la dirección logica a consultar y size la cantidad de paginas
 * asociadas a esa direccion logica
 */
NodePage ** getAllLogicalAddressPages(NodeProcess * self, int L_Addr, int size) {
    if(self!=NULL) {
        int pagesNum = getSize(self->pages);
        if(pagesNum > 0) {
            NodePage ** pages = (NodePage**) malloc(size*sizeof(NodePage*));
            goToStart(self->pages);
            int i = 0;
            while(!atEnd(self->pages)) {
                NodePage * actualPage = getElement(self->pages);
                if(actualPage->L_Address == L_Addr) {
                    pages[i] = getElement(self->pages);
                    
                }
                nextNodePage(self->pages);
                i++;
            }
            return pages;
        }
        return NULL;
    }

    return NULL;
}

//add
void addNodePageToProcess(NodeProcess * self, int pageId, int parentId, int loaded, int L_Addr, int M_Addr, int D_addr){
    if(self != NULL) {
        append(self->pages, pageId, parentId, loaded, L_Addr, M_Addr, D_addr);
        self->pagesNumber++;
    }

}








