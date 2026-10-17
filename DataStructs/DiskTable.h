#ifndef DISKTABLE_H
#define DISKTABLE_H

#include "NodePage.h"

typedef struct KeyValueDisk KeyValueDisk;
typedef struct DiskTable DiskTable;


struct KeyValueDisk {
    int key; //Indice en memoria                     
    NodePage * value;
    struct KeyValueDisk * next;            
};



struct DiskTable {
    KeyValueDisk ** values;
};


DiskTable * createDiskTable();
void destroyDiskTable(DiskTable * self);


int addDiskTableElement(DiskTable * self, int key, NodePage * newNode);
NodePage * getDiskTableValue(DiskTable * self, int key);

void printDiskTableValues(DiskTable * self);

#endif


