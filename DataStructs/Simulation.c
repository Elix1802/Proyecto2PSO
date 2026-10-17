#include <stdlib.h>
#include "Simulation.h"

static const int totalMemory = 400 * 1024;
static const int pageSize = 4 * 1024;
static const int maxAllowedPid = 1000000;

int simValidate(const InstructionList *list) {
    if (list == NULL || list->count <= 0) return -1;

    int newCount = 0;
    int maxPid = 0;
    for (int i = 0; i < list->count; i++) {
        const Instruction *ins = &list->instructions[i];
        if (ins->operation == NEW) newCount++;
        if ((ins->operation == NEW || ins->operation == KILL) && ins->pid > maxPid) maxPid = ins->pid;
    }
    if (maxPid > maxAllowedPid) return -1;

    int *owner = calloc(newCount + 2, sizeof(int));
    int *state = calloc(maxPid + 2, sizeof(int));
    if (owner == NULL || state == NULL) {
        free(owner);
        free(state);
        return -1;
    }

    int nextPtr = 1;
    int bad = 0;

    for (int i = 0; i < list->count && bad == 0; i++) {
        const Instruction *ins = &list->instructions[i];

        switch (ins->operation) {
            case NEW:
                if (ins->pid < 1 || ins->size <= 0 || state[ins->pid] == 2) {
                    bad = i + 1;
                    break;
                }
                state[ins->pid] = 1;
                owner[nextPtr++] = ins->pid;
                break;

            case USE:
            case DELETE:
                if (ins->ptr < 1 || ins->ptr >= nextPtr || owner[ins->ptr] == 0) {
                    bad = i + 1;
                    break;
                }
                if (ins->operation == DELETE) owner[ins->ptr] = 0;
                break;

            case KILL:
                if (ins->pid < 1 || state[ins->pid] != 1) {
                    bad = i + 1;
                    break;
                }
                state[ins->pid] = 2;
                for (int p = 1; p < nextPtr; p++) {
                    if (owner[p] == ins->pid) owner[p] = 0;
                }
                break;
        }
    }

    free(owner);
    free(state);
    return bad;
}

static int predictFuture(MMU *opt, const InstructionList *list) {
    int *addr = calloc(list->count + 1, sizeof(int));
    if (addr == NULL) return -1;

    int ptrCount = 0;
    for (int i = 0; i < list->count; i++) {
        const Instruction *ins = &list->instructions[i];
        if (ins->operation == NEW) {
            addr[++ptrCount] = predictNew(opt, ins->size);
        } else if (ins->operation == USE) {
            predictUse(opt, addr[ins->ptr]);
        }
    }

    restoreIdPages(opt);
    free(addr);
    return 0;
}

Simulation *simCreate(const InstructionList *list, int algID) {
    if (algID < FIFO || algID > LFU) return NULL;
    if (simValidate(list) != 0) return NULL;

    Simulation *sim = calloc(1, sizeof(Simulation));
    if (sim == NULL) return NULL;

    sim->list = list;
    sim->maxPtr = list->count + 1;
    sim->optAddr = calloc(sim->maxPtr, sizeof(int));
    sim->algAddr = calloc(sim->maxPtr, sizeof(int));
    sim->opt = createMMU(OPT, totalMemory, pageSize);
    sim->alg = createMMU(algID, totalMemory, pageSize);

    if (!sim->optAddr || !sim->algAddr || !sim->opt || !sim->alg || predictFuture(sim->opt, list) != 0) {
        simDestroy(sim);
        return NULL;
    }

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