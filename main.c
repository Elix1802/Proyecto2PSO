#include <stdio.h>
#include <stdlib.h>
#include "DataStructs/NodePage.h"
#include "DataStructs/DLinkedList.h"

int main() {
  DLinkedList * linkedList = createDLinkedList();
  append(linkedList, 2, 2, 2, 2, 2, 2);
  NodePage * nodePage = createNodePage(1, 1, 1, 1, 1, 1, NULL, NULL);
  printNodePage(nodePage);
  destroyNodePage(nodePage);
  printf("Hola amigo!\n");
  return 0;
}
