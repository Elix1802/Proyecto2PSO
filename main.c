#include <stdio.h>
#include <stdlib.h>
#include "DataStructs/NodePage.h"

int main() {
  NodePage * nodePage = createNodePage(1, 1, 1, 1, 1, 1, NULL, NULL);
  printNodePage(nodePage);
  destroyNodePage(nodePage);
  printf("Hola amigo!\n");
  return 0;
}
