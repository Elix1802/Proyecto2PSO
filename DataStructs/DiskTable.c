#include "DiskTable.h"
#include "NodePage.h"
#include <stdio.h>
#include <stdlib.h>




int DIC_SIZE_DISK = 256;


int hashFunctionDisk(int key) {
    return (key % DIC_SIZE_DISK + DIC_SIZE_DISK) % DIC_SIZE_DISK;
}

DiskTable * createDiskTable() {
    DiskTable * diskTable = (DiskTable*) malloc(sizeof(DiskTable));
    if(diskTable == NULL) return NULL;
    
    diskTable->values = (KeyValueDisk**) malloc(DIC_SIZE_DISK * sizeof(KeyValueDisk*));
    if(diskTable->values == NULL) {
        free(diskTable);
        return NULL;
    }

    for (int i = 0; i < DIC_SIZE_DISK; i++) {
        diskTable->values[i] = NULL; 
    }

    return diskTable;
}


void destroyDiskTable(DiskTable * self) {
    if(self != NULL) {
        for (int i = 0; i < DIC_SIZE_DISK; i++) {
            KeyValueDisk * currentKv = self->values[i];
            if (currentKv != NULL && currentKv->value != NULL) {
                destroyNodePage(currentKv->value);
            }
        }
        free(self->values);
        free(self);
    }
}


int addDiskTableElement(DiskTable * self, int key, NodePage * newNode){
    if(self == NULL || newNode == NULL) return -1;
    
    int index = hashFunctionDisk(key);

    KeyValueDisk * currentKv = self->values[index];

    int i = index;
    
    while(currentKv != NULL) {
        if(currentKv->key == key) {
            return 0;
        }

        if(i >= DIC_SIZE_DISK) {
            int lastSize = DIC_SIZE_DISK;
            DIC_SIZE_DISK*=2;
            KeyValueDisk ** temp = (KeyValueDisk**) malloc(DIC_SIZE_DISK * sizeof(KeyValueDisk*));
            for(int j = 0; j<DIC_SIZE_DISK; j++) {   
                temp[j] = NULL;
            }

            for(int j = 0; j<lastSize; j++) {
                int newIndex = hashFunctionDisk(self->values[j]->key);
                temp[newIndex] =  *&self->values[j];
            }
            
            free(self->values);
            self->values = temp;
            
            //Agregar nuevo key
            KeyValueDisk * newKeyValue = (KeyValueDisk*) malloc(sizeof(KeyValueDisk));
                
            if(newKeyValue == NULL) return -2;

            int newIndex = hashFunctionDisk(key);
            
            newKeyValue->key = key;
            newKeyValue->value = newNode;

            newKeyValue->next = self->values[(newIndex + 1 >= DIC_SIZE_DISK) ? newIndex : newIndex + 1];
            self->values[newIndex] = newKeyValue;

            return 2;

        }
        i++;
        currentKv = currentKv->next;
    }
    
    index = i;
    KeyValueDisk * newKeyValue = (KeyValueDisk*) malloc(sizeof(KeyValueDisk));
    
    if(newKeyValue == NULL) return -2;
    
    newKeyValue->key = key;
    newKeyValue->value = newNode;

    newKeyValue->next = self->values[(index + 1 >= DIC_SIZE_DISK) ? index : index + 1];
    self->values[index] = newKeyValue;
    return 1;
}


NodePage* getDiskTableValue(DiskTable * self, int key) {
    if(self != NULL) {
        int index = hashFunctionDisk(key);
        KeyValueDisk * currentKv = self->values[index];
        int i =  index;
        while(currentKv != NULL) {
            if(currentKv->key == key) {
                self->values[i] = NULL;
                return currentKv->value;
            }
            currentKv = currentKv->next;
            i++;
        }
    }
    return NULL;
}



void printDiskTableValues(DiskTable * self) {
    if(self != NULL) {
        for(int i = 0; i < DIC_SIZE_DISK; i++) {
            KeyValueDisk * currentKv = self->values[i];
            if(currentKv != NULL) {
                printNodePage(currentKv->value);
            }
        }
    }
}