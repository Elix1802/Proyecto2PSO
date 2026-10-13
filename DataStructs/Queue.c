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

NodePage * deleteSpecificElementQueue(Queue *cola, int idPage) {
    if (cola->cantidad == 0) return NULL;

    int i = cola->frente;
    int elementosProcesados = 0;
    int cantidadOriginal = cola->cantidad;
    NodePage *founded = NULL;
    while (elementosProcesados < cantidadOriginal) {
        NodePage * nodoActual = cola->datos[i];

        if (nodoActual->id == idPage) {
            int actual = i;
            for (int step = 0; step < cantidadOriginal - elementosProcesados - 1; step++) {
                int siguiente = (actual + 1) % TAM_MAX;
                cola->datos[actual] = cola->datos[siguiente];
                actual = siguiente;
            }
            
            cola->final = (cola->final - 1 + TAM_MAX) % TAM_MAX;
            cola->cantidad--;
            
            cantidadOriginal--;
            if(founded == NULL) founded = nodoActual;

        } else {
            i = (i + 1) % TAM_MAX;
            elementosProcesados++;
        }
    }

    return founded;
}


void printQueue(Queue * self) {
  if(self != NULL) {
    for(int i = self->frente; i<self->final; i++) {
      printNodePage(self->datos[i]);
    }
  }
}