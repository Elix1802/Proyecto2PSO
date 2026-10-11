#include <stdio.h>
#include <stdlib.h>
#include "DataStructs/NodePage.h"
#include "DataStructs/NodeProcess.h"
#include "DataStructs/DLinkedList.h"

int main() {
  DLinkedList * linkedList = createDLinkedList();
  NodePage * nodePage = createNodePage(1, 1, 1, 1, 1, 1, NULL, NULL);
  NodeProcess * nodeProcess = createNodeProcess(1);

  append(linkedList, 2, 2, 2, 2, 2, 2);
  //printNodePage(nodePage);
  //printDLinkedList(linkedList);

  addNodePageToProcess(nodeProcess, 1,1,1,1,1,1);
  addNodePageToProcess(nodeProcess, 2, 1, 1, 2,2,-1); //pageId, parentId, loaded, L_Addr, M_Addr, D_addr
  int logical = getNumberLogicalAddressPages(nodeProcess, 1);
  NodePage ** pages = getAllLogicalAddressPages(nodeProcess,1, logical);

  for(int i=0; i<logical; i++) {
      printNodePage(pages[i]);

  }


  destroyNodePage(nodePage);
  destroyDLinkedList(linkedList);
  return 0;
}
