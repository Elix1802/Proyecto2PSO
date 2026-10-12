#ifndef SYMBOLSTABLE_H
#define SYMBOLSTABLE_H

#include "NodePage.h"


typedef struct SymbolsTable SymbolsTable;
typedef struct KeyValue KeyValue;

SymbolsTable * createSymbolsTable();
void destroySymbolsTable(SymbolsTable * self);


void addSymbolsTableElement(SymbolsTable * self, int key, NodePage * newNode);
NodePage * getSymbolsTableValue(SymbolsTable * self, int key);
NodePage * popSymbolsTableElement(SymbolsTable * self, int key);

void printSymbolsTableValues(SymbolsTable * self);

#endif