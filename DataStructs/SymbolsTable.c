#include "SymbolsTable.h"
#include "NodePage.h"
#include <stdio.h>
#include <stdlib.h>

#define DIC_SIZE 256

struct KeyValue {
    int key;                     
    NodePage * value;            
    struct KeyValue * next;      
};

struct SymbolsTable {
    KeyValue ** values;
};


int hashFunctionD(int key) {
    return (key % DIC_SIZE + DIC_SIZE) % DIC_SIZE;
}

SymbolsTable * createSymbolsTable() {
    SymbolsTable * symbolsTable = (SymbolsTable*) malloc(sizeof(SymbolsTable));
    if(symbolsTable == NULL) return NULL;
    
    symbolsTable->values = (KeyValue**) malloc(DIC_SIZE * sizeof(KeyValue*));
    if(symbolsTable->values == NULL) {
        free(symbolsTable);
        return NULL;
    }

    for (int i = 0; i < DIC_SIZE; i++) {
        symbolsTable->values[i] = NULL; 
    }

    return symbolsTable;
}


void destroySymbolsTable(SymbolsTable * self) {
    if(self != NULL) {
        for (int i = 0; i < DIC_SIZE; i++) {
            KeyValue * currentKv = self->values[i];
            while(currentKv != NULL) {
                KeyValue * nextKv = currentKv->next;
                
                NodePage * currentPg = currentKv->value;
                while(currentPg != NULL) {
                    NodePage * nextPg = getNextPtr(currentPg);
                    destroyNodePage(currentPg);
                    currentPg = nextPg;
                }
                
                free(currentKv);
                currentKv = nextKv;
            }
        }
        free(self->values);
        free(self);
    }
}


void addSymbolsTableElement(SymbolsTable * self, int key, NodePage * newNode){
    if(self == NULL || newNode == NULL) return;
    
    int index = hashFunctionD(key);

    KeyValue * currentKv = self->values[index];
    while(currentKv != NULL) {
        if(currentKv->key == key) {
            return;
        }
        currentKv = currentKv->next;
    }

    KeyValue * newKeyValue = (KeyValue*) malloc(sizeof(KeyValue));
    if(newKeyValue == NULL) return;
    
    newKeyValue->key = key;
    newKeyValue->value = newNode;

    
    newKeyValue->next = self->values[index];
    self->values[index] = newKeyValue;


}


NodePage* getSymbolsTableValue(SymbolsTable * self, int key) {
    if(self != NULL) {
        int index = hashFunctionD(key);
        KeyValue * currentKv = self->values[index];
        
        while(currentKv != NULL) {
            if(currentKv->key == key) {
                return currentKv->value;
            }
            currentKv = currentKv->next;
        }
    }
    return NULL;
}

NodePage* popSymbolsTableElement(SymbolsTable * self, int key) {
    if(self != NULL) {
        int index = hashFunctionD(key);
        KeyValue * currentKv = self->values[index];
        
        while(currentKv != NULL) {
            if(currentKv->key == key) {
                self->values[index] = NULL;
                return currentKv->value;
            }
            currentKv = currentKv->next;
        }
    }
    return NULL;
}


void printSymbolsTableValues(SymbolsTable * self) {
    if(self != NULL) {
        for(int i = 0; i < DIC_SIZE; i++) {
            KeyValue * currentKv = self->values[i];
            while(currentKv != NULL) {
                printf("key: %d idPage: %d\n", currentKv->key, currentKv->value->id);
                
                NodePage * current = currentKv->value;
                while(current != NULL) {
                    printNodePage(current);
                    current = getNextPtr(current);
                }
                printf("\n");
                
                currentKv = currentKv->next;
            }
        }
    }
}