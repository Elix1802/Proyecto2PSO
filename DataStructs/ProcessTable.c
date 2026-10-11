#include "ProcessTable.h"
#include "NodeProcess.h"
#include <stdio.h>
#include <stdlib.h>


struct KeyValueP {
    int key;                     
    NodeProcess * value;
    struct KeyValueP * next;            
};

struct ProcessTable {
    KeyValueP ** values;
};

int DIC_SIZE = 256;


int hashFunctionP(int key) {
    return (key % DIC_SIZE + DIC_SIZE) % DIC_SIZE;
}

ProcessTable * createProcessTable() {
    ProcessTable * processTable = (ProcessTable*) malloc(sizeof(ProcessTable));
    if(processTable == NULL) return NULL;
    
    processTable->values = (KeyValueP**) malloc(DIC_SIZE * sizeof(KeyValueP*));
    if(processTable->values == NULL) {
        free(processTable);
        return NULL;
    }

    for (int i = 0; i < DIC_SIZE; i++) {
        processTable->values[i] = NULL; 
    }

    return processTable;
}


void destroyProcessTable(ProcessTable * self) {
    if(self != NULL) {
        for (int i = 0; i < DIC_SIZE; i++) {
            KeyValueP * currentKv = self->values[i];
            while (currentKv != NULL) {
                KeyValueP * nextKv = currentKv->next;
                destroyNodeProcess(currentKv->value);
                free(currentKv);
                currentKv = nextKv;
            }
        }
        free(self->values);
        free(self);
    }
}


int addProcessTableElement(ProcessTable * self, int key, NodeProcess * newNode){
    if(self == NULL || newNode == NULL) return -1;
    
    int index = hashFunctionP(key);

    KeyValueP * currentKv = self->values[index];
    while(currentKv != NULL) {
        if(currentKv->key == key) {
            return 0;
        }
        currentKv = currentKv->next;
    }

    KeyValueP * newKeyValue = (KeyValueP*) malloc(sizeof(KeyValueP));
    if(newKeyValue == NULL) return -2;

    newKeyValue->key = key;
    newKeyValue->value = newNode;
    newKeyValue->next = self->values[index];
    self->values[index] = newKeyValue;
    return 1;
}



NodeProcess* getProcessTableValue(ProcessTable * self, int key) {
    if(self != NULL) {
        int index = hashFunctionP(key);
        KeyValueP * currentKv = self->values[index];
        
        while(currentKv != NULL) {
            if(currentKv->key == key) {
                return currentKv->value;
            }
            currentKv = currentKv->next;
        }
    }
    return NULL;
}


void printProcessTableValues(ProcessTable * self) {
    if(self != NULL) {
        for(int i = 0; i < DIC_SIZE; i++) {
            KeyValueP * currentKv = self->values[i];
            if(currentKv != NULL) {
               // printf("idProcess: %d ; pagesNumber: %d\n", currentKv->key, currentKv->value->pagesNumber);
            }
        }
    }
}