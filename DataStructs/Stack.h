#ifndef STACK_H
#define STACK_H

typedef struct {
    int * pila;
    int size;
    int actualIndex;
    int defaultSize;
} Stack;

Stack * createStack();
void destroyStack(Stack * self);

void pushStack(Stack * self, int id);
int popStack(Stack * self);

#endif