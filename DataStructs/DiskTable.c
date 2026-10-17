#include "DiskTable.h"
#include "NodePage.h"
#include <stdio.h>
#include <stdlib.h>

#define DIC_SIZE_DISK 1024


static int hashFunctionDisk(int key) {
    return (key % DIC_SIZE_DISK + DIC_SIZE_DISK) % DIC_SIZE_DISK;
}

DiskTable * createDiskTable() {
    DiskTable * diskTable = (DiskTable*) malloc(sizeof(DiskTable));
    if(diskTable == NULL) return NULL;

    diskTable->values = (KeyValueDisk**) calloc(DIC_SIZE_DISK, sizeof(KeyValueDisk*));
    if(diskTable->values == NULL) {
        free(diskTable);
        return NULL;
    }

    return diskTable;
}

void destroyDiskTable(DiskTable * self) {
    if(self != NULL) {
        for (int i = 0; i < DIC_SIZE_DISK; i++) {
            KeyValueDisk * currentKv = self->values[i];
            while(currentKv != NULL) {
                KeyValueDisk * nextKv = currentKv->next;
                free(currentKv);
                currentKv = nextKv;
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
    while(currentKv != NULL) {
        if(currentKv->key == key) return 0;
        currentKv = currentKv->next;
    }

    KeyValueDisk * newKeyValue = (KeyValueDisk*) malloc(sizeof(KeyValueDisk));
    if(newKeyValue == NULL) return -2;

    newKeyValue->key = key;
    newKeyValue->value = newNode;
    newKeyValue->next = self->values[index];
    self->values[index] = newKeyValue;
    return 1;
}

NodePage* getDiskTableValue(DiskTable * self, int key) {
    if(self != NULL) {
        int index = hashFunctionDisk(key);
        KeyValueDisk * prev = NULL;
        KeyValueDisk * currentKv = self->values[index];

        while(currentKv != NULL) {
            if(currentKv->key == key) {
                if(prev == NULL) self->values[index] = currentKv->next;
                else prev->next = currentKv->next;
                NodePage * value = currentKv->value;
                free(currentKv);
                return value;
            }
            prev = currentKv;
            currentKv = currentKv->next;
        }
    }
    return NULL;
}

void printDiskTableValues(DiskTable * self) {
    if(self != NULL) {
        for(int i = 0; i < DIC_SIZE_DISK; i++) {
            KeyValueDisk * currentKv = self->values[i];
            while(currentKv != NULL) {
                printNodePage(currentKv->value);
                currentKv = currentKv->next;
            }
        }
    }
}
