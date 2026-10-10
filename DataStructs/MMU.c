#include "NodeProcess.h"
#include "NodeProcess.c"
#include "NodePage.h"
#include "NodePage.c"

struct MMU {
  int availableMemory;
  int totalMemory;
  int pageSize;
  int algID;
  struct nodeProcess * processes;
};

struct NodePage* getPages(struct MMU * self){


};


