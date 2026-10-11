#include "NodeProcess.h"
#include "NodePage.h"

struct MMU {
  int availableMemory;
  int totalMemory;
  int pageSize;
  int algID;
  struct nodeProcess * processes;
};

struct NodePage* getPages(struct MMU * self){


};


