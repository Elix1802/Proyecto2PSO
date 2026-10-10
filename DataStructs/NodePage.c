#include <stdio.h>
#include <stdlib.h>
#include "NodePage.h"

struct NodePage {
    int id;
    int parentId;
    int loaded;
    int L_Address;
    int M_Address;
    int D_Address;
    struct NodePage * nextPtr;
    struct NodePage * pastPtr;
};


NodePage * createNodePage() {

}