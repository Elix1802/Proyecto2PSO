#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"

Stack * createStack() {
    Stack * stack = (Stack*) malloc(sizeof(Stack));
    stack->defaultSize = 256;
    stack->actualIndex = 0;
    stack->size = 0;
    stack->pila = (int*) calloc(stack->defaultSize, sizeof(Stack));
}
void destroyStack(Stack * self){
    free(self->pila);
    free(self);
}

void pushStack(Stack * self, int id) {
    if(self!=NULL) {
        for(int i = 0; i < self->size; i++) {
            if(self->pila[i] == id) return;
        }

        self->pila[self->actualIndex] = id;
        self->actualIndex++;
        self->size++;
    }
    
}

int popStack(Stack * self) {
    if(self!=NULL) {
        int id = self->pila[--self->actualIndex];
        self->size--;
        return id;
    }
}