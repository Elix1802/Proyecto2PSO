#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#include "NodeProcess.h"


typedef struct ProcessTable ProcessTable;
typedef struct KeyValueP KeyValueP;

ProcessTable * createProcessTable();
void destroyProcessTable(ProcessTable * self);


int addProcessTableElement(ProcessTable * self, int key, NodeProcess * newNode);
NodeProcess * getProcessTableValue(ProcessTable * self, int key);

void printProcessTableValues(ProcessTable * self);

#endif