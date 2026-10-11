#ifndef DISKTABLE_H
#define DISKTABLE_H

#include "NodePage.h"


typedef struct DiskTable DiskTable;
typedef struct KeyValueDisk KeyValueDisk;

DiskTable * createDiskTable();
void destroyDiskTable(DiskTable * self);


int addDiskTableElement(DiskTable * self, int key, NodePage * newNode);
NodePage * getDiskTableValue(DiskTable * self, int key);

void printDiskTableValues(DiskTable * self);

#endif