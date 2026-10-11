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
            destroyNodeProcess(currentKv->value);
        }
        free(self->values);
        free(self);
    }
}


int addProcessTableElement(ProcessTable * self, int key, NodeProcess * newNode){
    if(self == NULL || newNode == NULL) return -1;
    
    int index = hashFunctionP(key);

    KeyValueP * currentKv = self->values[index];

    int i = index;
    
    while(currentKv != NULL) {
        if(currentKv->key == key) {
            return 0;
        }

        if(i >= DIC_SIZE) {
            int lastSize = DIC_SIZE;
            DIC_SIZE*=2;
            KeyValueP ** temp = (KeyValueP**) malloc(DIC_SIZE * sizeof(KeyValueP*));
            for(int j = 0; j<DIC_SIZE; j++) {   
                temp[j] = NULL;
            }

            for(int j = 0; j<lastSize; j++) {
                int newIndex = hashFunctionP(self->values[j]->key);
                temp[newIndex] =  *&self->values[j];
            }
            
            free(self->values);
            self->values = temp;
            
            //Agregar nuevo key
            KeyValueP * newKeyValue = (KeyValueP*) malloc(sizeof(KeyValueP));
                
            if(newKeyValue == NULL) return -2;

            int newIndex = hashFunctionP(key);
            
            newKeyValue->key = key;
            newKeyValue->value = newNode;

            newKeyValue->next = self->values[(newIndex + 1 >= DIC_SIZE) ? newIndex : newIndex + 1];
            self->values[newIndex] = newKeyValue;

            return 2;

        }
        i++;
        currentKv = currentKv->next;
    }
    
    index = i;
    KeyValueP * newKeyValue = (KeyValueP*) malloc(sizeof(KeyValueP));
    
    if(newKeyValue == NULL) return -2;
    
    newKeyValue->key = key;
    newKeyValue->value = newNode;

    newKeyValue->next = self->values[(index + 1 >= DIC_SIZE) ? index : index + 1];
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