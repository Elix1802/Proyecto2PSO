#include <stdlib.h>
#include <stdio.h>
#include "NodePage.h"
#include "NodeProcess.h"

struct NodeProcess {
    int id;
    struct NodePage ** pages;
    int pageSize;
};