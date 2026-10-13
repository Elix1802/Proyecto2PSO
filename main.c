#include <stdio.h>
#include <stdlib.h>
#include "DataStructs/NodePage.h"
#include "DataStructs/NodeProcess.h"
#include "DataStructs/DLinkedList.h"
#include "DataStructs/SymbolsTable.h"
#include "DataStructs/ProcessTable.h"
#include "DataStructs/DiskTable.h"  
#include "DataStructs/Queue.h"
#include "DataStructs/MMU.h"
#include "Algorithms/FIFO.h"

/** 
int main() {
  DLinkedList * linkedList = createDLinkedList();
  DLinkedList * linkedList2 = createDLinkedList();

  NodePage * nodePage = createNodePage(1, 1, 1, 1, 1, 1, 5, NULL, NULL);
  NodeProcess * nodeProcess = createNodeProcess(1);
  NodeProcess * nodeProcess2 = createNodeProcess(2);

  ProcessTable * processTable = createProcessTable();
  DiskTable * diskTable = createDiskTable();

  int diskAddr = 100;

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



  append(linkedList2, 3, 1, 1, 10, 20, 30, L_addr); //pageId, parentId, loaded, L_Addr, M_Addr, D_addr

  NodePage * page1 = createNodePage(4, 1, 1, 10, 20, 30, L_addr,NULL, NULL);
  NodePage * page2 = createNodePage(5, 1, 1, 40, 50, 60, L_addr, NULL, NULL);
  NodePage * page3 = createNodePage(6, 1, 1, 70, 80, 90, L_addr, NULL, NULL);
  DLinkedList * combined = combineDLinkedList(linkedList, linkedList2);
  printf("\nCombined:\n");
  printDLinkedList(combined);
  insertionPageFIFO(combined, page1, diskTable, &diskAddr);
  printf("\nDespues de fifo:\n");
  printDLinkedList(combined);
  printf("\nDespues de fifo cambio:\n");
  insertionPageFIFO(combined, page2, diskTable, &diskAddr);
  printDLinkedList(combined);

  insertionPageFIFO(combined, page3, diskTable, &diskAddr);



  printf("\nTabla Disco\n");
  printDLinkedList(combined);
  printDiskTableValues(diskTable);

  bringPageToMemoryFIFO(combined,diskTable, 100, &diskAddr);

  printf("\nTabla Disco trayendo de disco\n");
  printDLinkedList(combined);
  printDiskTableValues(diskTable);







  destroyNodePage(nodePage);
  destroyDLinkedList(linkedList);
  //destroyDiskTable(diskTable);

  return 0;
}
*/

/** 
int main() {
    DLinkedList * linkedList = createDLinkedList();
    Queue * cola = (Queue*) malloc(sizeof(Queue)); //RAM
    DiskTable * diskTable = createDiskTable();
    int diskAddr = 100;

    int L_addr = 3;
    inicializar(cola);
    NodePage * page1 = createNodePage(4, 1, 1, 10, 20, 30, L_addr,NULL, NULL);
    NodePage * page2 = createNodePage(5, 1, 1, 40, 50, 60, L_addr, NULL, NULL);
    NodePage * page3 = createNodePage(6, 1, 1, 70, 80, 90, L_addr, NULL, NULL);

    appendNodeLinkedList(linkedList, page1);
    appendNodeLinkedList(linkedList, page2);

    appendNodeLinkedList(linkedList, page3);

    insertionPageFIFO(cola,page1, diskTable, &diskAddr);
    insertionPageFIFO(cola,page2, diskTable, &diskAddr);
    printQueue(cola);

    printf("\nSe inserta uno mas que el tamaño\n");
    insertionPageFIFO(cola,page3, diskTable, &diskAddr);
    printDiskTableValues(diskTable);
    printQueue(cola);
    bringPageToMemoryFIFO(cola, diskTable, 100, &diskAddr);
    printf("\nQuedó\n");

    printQueue(cola);
    printDiskTableValues(diskTable);
}
*/

int main() {
    int memory = 1024 * 400;
    int pageSize = 1024 * 4;
    MMU * mmu = createMMU(5, memory, pageSize);
    int l_addr = newP(mmu, 1, 4096);
    int l_addr2 = newP(mmu, 2, 4096);
    int l_addr3 = newP(mmu, 3, 4096);
    int l_addr4 = newP(mmu, 4, 4096);

    //int l_addr3 = newP(mmu, 2, 9000);
    //int l_addr4 = newP(mmu, 1, 9000);

    printf("\n----------------------------------\n");
    printf("Memoria RAM y disco antes del use");
    printQueue(mmu->MemoryRAM);
    printf("\n----------------------------------\n");
    printDiskTableValues(mmu->diskTable);
    printf("\n----------------------------------\n");

    useP(mmu, l_addr);
    //useP(mmu, l_addr);
    
    printf("\nDespués\n");
    printf("\n----------------------------------\n");
    printf("Memoria RAM y disco después del use");
    printQueue(mmu->MemoryRAM);
    printf("\n----------------------------------\n");
    printDiskTableValues(mmu->diskTable);
    printf("\n----------------------------------\n");

    //deleteP(mmu, l_addr);
    printf("\n------------------Use 2-------------------\n");
    useP(mmu, l_addr);
    printQueue(mmu->MemoryRAM);
    printDiskTableValues(mmu->diskTable);


    printf("\n------------------Use 3-------------------\n");
    useP(mmu, l_addr2);
    printQueue(mmu->MemoryRAM);
    printDiskTableValues(mmu->diskTable);
    //printQueue(mmu->MemoryFIFOSC);
    //printDiskTableValues(mmu->diskTable);
    printf("\n------------------Kill 3-------------------\n");

    killP(mmu, 1);
    printQueue(mmu->MemoryRAM);
    printDiskTableValues(mmu->diskTable);



    



    return 0;
}