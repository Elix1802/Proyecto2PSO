#include <stdio.h>
#include <stdlib.h>
#include "DataStructs/NodePage.h"
#include "DataStructs/NodeProcess.h"
#include "DataStructs/DLinkedList.h"
#include "DataStructs/SymbolsTable.h"

int main() {
  DLinkedList * linkedList = createDLinkedList();
  NodePage * nodePage = createNodePage(1, 1, 1, 1, 1, 1, 5, NULL, NULL);
  NodeProcess * nodeProcess = createNodeProcess(1);

  append(linkedList, 2, 2, 2, 2, 2, 2, 5);
  //printNodePage(nodePage);
  //printDLinkedList(linkedList);

  int L_addr = 3;
  addNodePageToProcess(nodeProcess, 1,1,1,L_addr, 1,1, 5);
  addNodePageToProcess(nodeProcess, 2, 1, 1, L_addr, 2,-1, 5); //pageId, parentId, loaded, L_Addr, M_Addr, D_addr
  int logical = getNumberLogicalAddressPages(nodeProcess, L_addr);
  NodePage ** pages = getAllLogicalAddressPages(nodeProcess, L_addr, logical);

  for(int i=0; i<logical; i++) {
      printNodePage(pages[i]);

  }

  printf("¿Se eliminó? %d\n", deleteAllLogicalAddressPages(nodeProcess, L_addr));

  destroyNodePage(nodePage);
  destroyDLinkedList(linkedList);
  return 0;
}
