#include <stdio.h>
#include <stdlib.h>
#include "DataStructs/NodePage.h"
#include "DataStructs/NodeProcess.h"
#include "DataStructs/DLinkedList.h"
#include "DataStructs/SymbolsTable.h"
#include "DataStructs/ProcessTable.h"

int main() {
  DLinkedList * linkedList = createDLinkedList();
  DLinkedList * linkedList2 = createDLinkedList();

  NodePage * nodePage = createNodePage(1, 1, 1, 1, 1, 1, 5, NULL, NULL);
  NodeProcess * nodeProcess = createNodeProcess(1);
  NodeProcess * nodeProcess2 = createNodeProcess(2);

  ProcessTable * processTable = createProcessTable();

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
  addProcessTableElement(processTable, nodeProcess->id, nodeProcess);
  printProcessTableValues(processTable);

  //printf("\n¿Se eliminó? %d\n", deleteAllLogicalAddressPages(nodeProcess, L_addr));



    
  NodePage * page1 = createNodePage(101, 1, 1, 10, 20, 30, L_addr,NULL, NULL);
  NodePage * page2 = createNodePage(102, 1, 1, 40, 50, 60, L_addr, NULL, NULL);
  NodePage * page3 = createNodePage(103, 1, 1, 70, 80, 90, L_addr, NULL, NULL);
  addNodePageToProcess(nodeProcess2, 101, 1, 1, 10, 20, 30, L_addr); //pageId, parentId, loaded, L_Addr, M_Addr, D_addr
  DLinkedList * combined = combineDLinkedList(linkedList, linkedList2);
  printf("\nCombined:\n");
  printDLinkedList(combined);


  destroyNodePage(nodePage);
  destroyDLinkedList(linkedList);
  return 0;
}
