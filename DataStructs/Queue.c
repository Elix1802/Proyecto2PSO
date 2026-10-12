#define TAM_MAX 100
#include "Queue.h"
#include "../DataStructs/DiskTable.h"


void inicializar(Queue *cola) {
  cola->frente = 0;
  cola->final = 0;
  cola->cantidad = 0;
}

int enqueue(Queue *cola, NodePage* valor) {
  if (cola->cantidad == TAM_MAX) {
    return 0;
  }

  cola->datos[cola->final] = valor;
  cola->final = (cola->final + 1) % TAM_MAX;
  cola->cantidad++;
  return 1;
}

NodePage* dequeue(Queue *cola) {
    if (cola->cantidad == 0) return NULL;
    NodePage *valor = cola->datos[cola->frente];
    cola->frente = (cola->frente + 1) % TAM_MAX;
    cola->cantidad--;
    return valor;
}

NodePage* peek(const Queue *cola) {
    return cola->cantidad == 0 ? NULL : cola->datos[cola->frente];
}

void deleteSpecificElementQueue(Queue * cola, DiskTable * diskTable,int idPage, int * ptrDiskAddr){
    int i;
    for(i = 0; i<cola->cantidad; i++) {
      if(cola->datos[i]->id == idPage) {
        addDiskTableElement(diskTable, *ptrDiskAddr, cola->datos[i]);
        *ptrDiskAddr = (*ptrDiskAddr)+ 1;
        break;
      }
    }
    
    for(int j = i; j<cola->cantidad - 1; j++) {
        cola->datos[j] = cola->datos[j+1]; 
    }

    cola->cantidad--;
    cola->final--;
}

void printQueue(Queue * self) {
  if(self != NULL) {
    for(int i = self->frente; i<self->final; i++) {
      printNodePage(self->datos[i]);
    }
  }
}