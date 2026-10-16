#include <stdlib.h>
#include "Simulation.h"

#define TOTAL_MEMORY (400 * 1024)
#define PAGE_SIZE    (4 * 1024)

static void predictFuture(MMU *opt, const InstructionList *list, int *addr) {
    int ptrCount = 0;
    for (int i = 0; i < list->count; i++) {
        const Instruction *ins = &list->instructions[i];
        if (ins->operation == NEW)      addr[++ptrCount] = predictNew(opt, ins->size);
        else if (ins->operation == USE) predictUse(opt, addr[ins->ptr]);
    }
    restoreIdPages(opt);      
}

Simulation *simCreate(const InstructionList *list, int algID) {
    if (list == NULL || algID < FIFO || algID > LFU) return NULL;

    Simulation *sim = calloc(1, sizeof(Simulation));
    if (sim == NULL) return NULL;

    sim->list = list;
    sim->maxPtr = list->count + 1;
    sim->optAddr = calloc(sim->maxPtr, sizeof(int));
    sim->algAddr = calloc(sim->maxPtr, sizeof(int));
    sim->opt = createMMU(OPT, TOTAL_MEMORY, PAGE_SIZE);
    sim->alg = createMMU(algID, TOTAL_MEMORY, PAGE_SIZE);

    if (!sim->optAddr || !sim->algAddr || !sim->opt || !sim->alg) {
        simDestroy(sim);
        return NULL;
    }

    predictFuture(sim->opt, list, sim->optAddr);
    for (int i = 0; i < sim->maxPtr; i++) sim->optAddr[i] = 0;   
    return sim;
}

int simStep(Simulation *sim) {
    if (sim == NULL || sim->step >= sim->list->count) return 0;

    const Instruction *ins = &sim->list->instructions[sim->step++];

    switch (ins->operation) {
        case NEW:
            sim->ptrCount++;
            sim->optAddr[sim->ptrCount] = newP(sim->opt, ins->pid, ins->size);
            sim->algAddr[sim->ptrCount] = newP(sim->alg, ins->pid, ins->size);
            break;
        case USE:
            useP(sim->opt, sim->optAddr[ins->ptr]);
            useP(sim->alg, sim->algAddr[ins->ptr]);
            break;
        case DELETE:
            deleteP(sim->opt, sim->optAddr[ins->ptr]);
            deleteP(sim->alg, sim->algAddr[ins->ptr]);
            break;
        case KILL:
            killP(sim->opt, ins->pid);
            killP(sim->alg, ins->pid);
            break;
    }
    return 1;
}

void simDestroy(Simulation *sim) {
    if (sim == NULL) return;
    destroyMMU(sim->opt);
    destroyMMU(sim->alg);
    free(sim->optAddr);
    free(sim->algAddr);
    free(sim);
}