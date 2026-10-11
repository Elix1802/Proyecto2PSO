#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "MMU.c"

int main(void) {
    MMU * m = createMMU(FIFO, 400, 4);
    assert(m != NULL);
    assert(m->totalMemory == 400);
    assert(m->availableMemory == 400);
    assert(m->pageSize == 4);
    assert(m->algID == FIFO);
    assert(m->processes == NULL);
    assert(m->pageTable != NULL);
    assert(getSize(m->pageTable) == 0);
    destroyMMU(m);

    assert(createMMU(0, 400, 4) == NULL);
    assert(createMMU(LFU + 1, 400, 4) == NULL);
    assert(createMMU(OPT, 400, 0) == NULL);
    assert(createMMU(OPT, 2, 4) == NULL);
    assert(createMMU(OPT, 402, 4) == NULL);

    m = createMMU(LRU, 400, 4);
    assert(addProcess(m, 1) == 1);
    assert(addProcess(m, 2) == 1);
    assert(addProcess(m, 3) == 1);
    assert(addProcess(m, 2) == 0);
    assert(addProcess(NULL, 5) == 0);

    int n = -1;
    assert(getPages(m, 2, &n) == NULL && n == 0);
    assert(getPages(m, 99, &n) == NULL && n == 0);
    assert(getPages(NULL, 1, &n) == NULL && n == 0);

    append(m->pageTable, 1, 1, 0, 1, 0, 0);
    append(m->pageTable, 2, 2, 0, 2, 0, 0);
    append(m->pageTable, 3, 1, 0, 1, 0, 0);
    append(m->pageTable, 4, 3, 0, 3, 0, 0);

    NodePage ** pg = getPages(m, 1, &n);
    assert(pg != NULL && n == 2);
    assert(pg[0]->id == 1 && pg[1]->id == 3);
    free(pg);

    pg = getPages(m, 2, &n);
    assert(pg != NULL && n == 1 && pg[0]->id == 2);
    free(pg);

    pg = getPages(m, 3, NULL);
    assert(pg != NULL && pg[0]->id == 4);
    free(pg);

    destroyMMU(m);
    destroyMMU(NULL);

    puts("TODAS LAS PRUEBAS PASARON");
    return 0;
}