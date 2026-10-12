#ifndef QUEUE_H
#define QUEUE_H
#include "NodePage.h"
#include "../DataStructs/DiskTable.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  NodePage* datos[100];
  int frente;
  int final;
  int cantidad;

} Queue;

void inicializar(Queue *cola);
int enqueue(Queue *cola, NodePage* valor);
NodePage* dequeue(Queue *cola);
NodePage* peek(const Queue *cola);
void deleteSpecificElementQueue(Queue * cola, int idPage);
void printQueue(Queue * self);

#endif