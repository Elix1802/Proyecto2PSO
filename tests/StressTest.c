#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../DataStructs/MMU.h"
#include "../Files/FileManager.h"

static int check(MMU *m, int step) {
    int ld = 0, un = 0;
    NodePage *p = m->pageTable->head;
    int guard = 0;
    while (p != NULL && guard++ < 1000000) {
        if (p->id >= 0) { if (p->loaded) ld++; else un++; }
        if (p == m->pageTable->tail) break;
        p = p->nextPtr;
    }
    int inq = 0;
    for (int k = 0; k < m->MemoryRAM->cantidad; k++) { NodePage *q = queueAt(m->MemoryRAM, k); if (!q->loaded) { printf("step %d: non-loaded page in queue id=%d\n", step, q->id); return 1; } inq++; }
    if (ld != m->loaded || un != m->unloaded || inq != ld || ld > 100) {
        printf("step %d: MISMATCH chainLoaded=%d chainUnloaded=%d queue=%d mmu.loaded=%d mmu.unloaded=%d\n", step, ld, un, inq, m->loaded, m->unloaded);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    unsigned seed = argc > 1 ? atoi(argv[1]) : 1;
    int P = argc > 2 ? atoi(argv[2]) : 5;
    int N = argc > 3 ? atoi(argv[3]) : 200;
    InstructionList list;
    if (argc > 4) {
        srand(seed);
        initInstructionList(&list);
        int active[4096]; int owner[4096]; int na = 0; int nextPtr = 1;
        for (int i = 0; i < N; i++) {
            int r = rand() % 100;
            Instruction in; in.line = i + 1; in.size = 0; in.ptr = 0; in.pid = 1 + rand() % P;
            if (na == 0 || r < 30) { in.operation = NEW; in.size = 4096 + rand() % (150 * 1024); in.ptr = nextPtr; active[na] = nextPtr; owner[na] = in.pid; na++; nextPtr++; }
            else if (r < 85) { int k = rand() % na; in.operation = USE; in.ptr = active[k]; in.pid = owner[k]; }
            else { int k = rand() % na; in.operation = DELETE; in.ptr = active[k]; in.pid = owner[k]; active[k] = active[na-1]; owner[k] = owner[na-1]; na--; }
            appendInstruction(&list, in.operation, in.pid, in.size, in.ptr);
        }
        for (int p = 1; p <= P; p++) { Instruction in; in.operation = KILL; in.pid = p; in.size = 0; in.ptr = 0; in.line = 0; appendInstruction(&list, in.operation, in.pid, in.size, in.ptr); }
    } else if (generateInstructions(seed, P, N, &list) != 0) { printf("gen fail\n"); return 1; }
    int bad = 0;
    for (int alg = 1; alg <= 5; alg++) {
        MMU *m = createMMU(alg, 400 * 1024, 4096);
        int maxPtr = 0;
        for (int i = 0; i < list.count; i++) if (list.instructions[i].ptr > maxPtr) maxPtr = list.instructions[i].ptr;
        int *map = calloc(maxPtr + 2, sizeof(int));
        if (alg == 1) {
            for (int i = 0; i < list.count; i++) {
                Instruction in = list.instructions[i];
                if (in.operation == NEW) map[in.ptr] = predictNew(m, in.size);
                else if (in.operation == USE) predictUse(m, map[in.ptr]);
            }
            restoreIdPages(m);
            memset(map, 0, (maxPtr + 2) * sizeof(int));
        }
        int shown = 0;
        for (int i = 0; i < list.count && !bad; i++) {
            Instruction in = list.instructions[i];
            if (in.operation == NEW) { int r = newP(m, in.pid, in.size); if (r < 0) bad = 1; map[in.ptr] = r; }
            else if (in.operation == USE) useP(m, map[in.ptr]);
            else if (in.operation == DELETE) deleteP(m, map[in.ptr]);
            else {
                if (!shown) { shown = 1; printf("  pre-kill alg=%d t=%ds thrash=%.1f%% ram=%.0fKB(%.0f%%) vram=%.0fKB(%.0f%%) loaded=%d unloaded=%d proc=%d frag=%.2fKB\n", alg, m->elapsedTime, m->thrashPct, m->ramKB, m->ramPct, m->vramKB, m->vramPct, m->loaded, m->unloaded, m->processes, m->fragmentationKB); }
                killP(m, in.pid);
            }
            if (check(m, i)) bad = 1;
        }
        if (argc > 4) { }
        printf("alg=%d t=%d thrash=%.1f%% (%.0fs) ram=%.0fKB %.0f%% vram=%.0fKB %.0f%% loaded=%d unloaded=%d proc=%d frag=%.2fKB\n",
               alg, m->elapsedTime, m->thrashPct, m->thrashTime, m->ramKB, m->ramPct, m->vramKB, m->vramPct, m->loaded, m->unloaded, m->processes, m->fragmentationKB);
        free(map);
        destroyMMU(m);
    }
    freeInstructionList(&list);
    return bad;
}
