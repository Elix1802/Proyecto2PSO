#ifndef SIMULATION_H
#define SIMULAATION_H
#include "../Files/FileManager.h"
#include "MMU.h"

typedef struct {
    MMU *opt;
    MMU *alg;
    const InstructionList *list;
    int step;
    int *optAddr;
    int *algAddr;
    int ptrCount;
    int maxPtr;
} Simulation;

Simulation *simCReate(const InstructionList *list, int algID);
int simStep(Simulation *sim);
void simDestroy(Simulation *sim);


#endif