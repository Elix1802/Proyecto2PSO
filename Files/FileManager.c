#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "FileManager.h"


//Structs
typedef struct {
    int ptr;
    int pid;
} ActivePtr;

typedef struct {
    int P;                 
    int nextPtr;                

    ActivePtr* activePtrs;       // ptr activos y su pid
    int activePtrsCount;        

    int* ptrCountByPid;          // ptr activos de cada proceso
    int* lastPtrByPid;           

    int* activeProcesses;        // pids que no han hecho kill
    int activeProcessesCount;
} GenState;

//Semilla y random
static void rngSeed(unsigned int seed) {
    srand(seed);
}

static int rngRand(int min, int max) {
    if(max <= min){
        return min;
    }
    return min + rand() % (max - min + 1);
}


//Random
void initInstructionList(InstructionList* instructionList) {
    instructionList->instructions = NULL;
    instructionList->count = 0;
    instructionList->capacity = 0;
}

int appendInstruction(InstructionList* instructionList, enum OP operation, int pid, int size, int ptr) {
    if (instructionList->count == instructionList->capacity) {
        int newCap = (instructionList->capacity == 0) ? 128 : instructionList->capacity * 2;
        Instruction* tmp = (Instruction*)realloc(instructionList->instructions, newCap * sizeof(Instruction));
        if (tmp == NULL) return -1;
        instructionList->instructions = tmp;
        instructionList->capacity = newCap;
    }

    Instruction* ins = &instructionList->instructions[instructionList->count];
    ins->operation = operation;
    ins->pid  = pid;
    ins->size = size;
    ins->ptr  = ptr;
    ins->line = instructionList->count + 1;
    instructionList->count++;
    return 0;
}

void freeInstructionList(InstructionList* instructionList) {
    free(instructionList->instructions);
    instructionList->instructions = NULL;
    instructionList->count = 0;
    instructionList->capacity = 0;
}




/////////////////////Generacion de secuencia aleatoria//////////////
static void stateFree(GenState* st) {
    free(st->activePtrs);
    free(st->ptrCountByPid);
    free(st->lastPtrByPid);
    free(st->activeProcesses);
    st->activePtrs = NULL;
    st->ptrCountByPid = NULL;
    st->lastPtrByPid = NULL;
    st->activeProcesses = NULL;
}

// P procesos y N instrucciones
static int stateInit(GenState* st, int P, int N) {
    st->P = P;
    st->nextPtr = 1;
    st->activePtrsCount = 0;
    st->activeProcessesCount = 0;

    st->activePtrs       = (ActivePtr*)calloc(N + 1, sizeof(ActivePtr));
    st->ptrCountByPid    = (int*)calloc(P + 1, sizeof(int));
    st->lastPtrByPid     = (int*)calloc(P + 1, sizeof(int));
    st->activeProcesses  = (int*)calloc(P + 1, sizeof(int));

    if (!st->activePtrs || !st->ptrCountByPid || !st->lastPtrByPid || !st->activeProcesses) {
        stateFree(st);
        return -1;
    }
    return 0;
}

static void stateAddProcess(GenState* state, int pid){
    state->activeProcesses[state->activeProcessesCount++] = pid;
}

static int stateNew(GenState* state, int pid) {
    int ptr = state->nextPtr++;

    state->activePtrs[state->activePtrsCount].ptr = ptr;
    state->activePtrs[state->activePtrsCount].pid = pid;
    state->activePtrsCount++;

    state->ptrCountByPid[pid]++;
    state->lastPtrByPid[pid] = ptr;

    return ptr;
}

static void stateUse(GenState* state, int pid, int ptr){

    state->lastPtrByPid[pid] = ptr;
}

static int stateDelete(GenState* state, int pid, int ptr){
    for(int i = 0; i < state->activePtrsCount; i++){
        if(state->activePtrs[i].ptr == ptr && state->activePtrs[i].pid == pid){
            state->activePtrs[i] = state->activePtrs[state->activePtrsCount - 1];
            state->activePtrsCount--;

            state->ptrCountByPid[pid]--;
            if(state->lastPtrByPid[pid] == ptr){
                state->lastPtrByPid[pid] = 0;
            }
            return 0;
        }
    }
    return -1;
}

static int stateKill(GenState* state, int pid){
    int j = 0;
    for(int i = 0; i < state->activePtrsCount; i++){
        if(state->activePtrs[i].pid != pid){
            state->activePtrs[j++] = state->activePtrs[i];
            
        }
    }

    state->activePtrsCount = j;
    state->ptrCountByPid[pid] = 0;
    state->lastPtrByPid[pid] = 0;

    for(int i = 0; i < state->activeProcessesCount; i++){
        if(state->activeProcesses[i] == pid){
            state->activeProcesses[i] = state->activeProcesses[state->activeProcessesCount - 1];
            state->activeProcessesCount--;
            return 0;
        }
    }


    return -1;
}


static int randomSize(void){
    if(rngRand(1,100)<=80){
        return rngRand(1,4096);
    }
    else{
        return rngRand(4097, 40960);
    }
}

static int pickPtr(const GenState* state, int pid){
    if(state->ptrCountByPid[pid] == 0){
        return 0;
    }

    int randomPointer = rngRand(1, state->ptrCountByPid[pid]);

    for(int i=0; i<state->activePtrsCount; i++){
        if(state->activePtrs[i].pid == pid && --randomPointer == 0){
            return state->activePtrs[i].ptr;
        }
    }
    return 0;
}

int generateInstructions(unsigned int seed, int P, int N,InstructionList* instructionList){
    if(instructionList == NULL || P<1 || N < 2 * P){
        return -1;
    }
    initInstructionList(instructionList);

    GenState state;

    if(stateInit(&state, P, N) != 0){
        return -1;
    }

    rngSeed(seed);

    int ops = N -2 * P;

    //Se crea un new por proceso
    for(int pid = 1; pid <= P; pid++){
        stateAddProcess(&state, pid);
        int size = randomSize();
        int ptr = stateNew(&state, pid);
        if( appendInstruction(instructionList, NEW, pid, size, ptr) != 0){
            stateFree(&state);
            freeInstructionList(instructionList);
            return -1;
        }
    }

    // Use, Delete

    for(int i = 0; i < ops; i++){
        int pid = state.activeProcesses[rngRand(0, state.activeProcessesCount -1)];

        int r = rngRand(1,100);
        //New 20%
        if(state.ptrCountByPid[pid] == 0 || r <= 20){
            int size = randomSize();
            int ptr = stateNew(&state, pid);
            if(appendInstruction(instructionList, NEW, pid, size, ptr) != 0){
                stateFree(&state);
                freeInstructionList(instructionList);
                return -1;
            }

        //Use 60%
        }else if (r <= 80){
            int ptr = pickPtr(&state, pid);
            if(ptr == 0){
                stateFree(&state);
                freeInstructionList(instructionList);
                return -1;
            }
            stateUse(&state, pid, ptr);
            if(appendInstruction(instructionList, USE, pid, 0, ptr) != 0){
                stateFree(&state);
                freeInstructionList(instructionList);
                return -1;
            }
        //Delete %20
        }else{
            int ptr =  pickPtr(&state, pid);
            if(ptr == 0){
                stateFree(&state);
                freeInstructionList(instructionList);
                return -1;
            }
            if(stateDelete(&state, pid, ptr) != 0){
                stateFree(&state);
                freeInstructionList(instructionList);
                return -1;
            }
            if(appendInstruction(instructionList, DELETE, pid, 0, ptr) != 0){
                stateFree(&state);
                freeInstructionList(instructionList);
                return -1;
            }
        }
        
    }

    //Kills
        for(int pid = 1; pid<=P; pid++){
            stateKill(&state, pid);
            if(appendInstruction(instructionList, KILL, pid, 0, 0) != 0){
                stateFree(&state);
                freeInstructionList(instructionList);
                return -1;
            }
        }


        stateFree(&state);
        return 0;
}


//Para debuggear
static void printInstruction(const Instruction* ins){
    switch (ins->operation)
    {
    case NEW:
        printf("new(%d, %d)\n", ins->pid, ins->size);
        break;

    case USE:
        printf("use(%d)\n", ins->ptr);
        break;

    case DELETE:
        printf("delete(%d)\n", ins->ptr);
        break;

    case KILL:
        printf("kill(%d)\n", ins->pid);
        break;
    
    default:
        break;
    }
}


//Archivo
int runFile(char *filename) {

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    int lineCounter = 0;
    int nextPtr = 1;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        lineCounter++;

        int len = (int)strlen(line);
        while (len > 0 && isspace((unsigned char)line[len - 1])) {
            line[--len] = '\0';
        }

        // Trim
        char *start = line;
        while (*start != '\0' && isspace((unsigned char)*start)) {
            start++;
        }
        if (*start == '\0') {
            continue;
        }

        int OP;
        if (strncmp(start, "new(", 4) == 0) {
            OP = NEW;
        } else if (strncmp(start, "use(", 4) == 0) {
            OP = USE;
        } else if (strncmp(start, "delete(", 7) == 0) {
            OP = DELETE;
        } else if (strncmp(start, "kill(", 5) == 0) {
            OP = KILL;
        } else {
            printf("Error: Invalid operation in line %d: %s\n", lineCounter, start);
            fclose(file);
            return 0;
        }

        int pid = 0, size = 0, ptr = 0;

        switch (OP) {
            case NEW:
                if (sscanf(start, "new(%d,%d)", &pid, &size) != 2) {
                    printf("Error: Invalid format in line %d: %s\n", lineCounter, start);
                    fclose(file);
                    return 0;
                }
                ptr = nextPtr++;
                printf("new(%d,%d) ptr=%d\n", pid, size, ptr);
                // newP(pid, size);
                break;

            case USE:
                if (sscanf(start, "use(%d)", &ptr) != 1) {
                    printf("Error: Invalid format in line %d: %s\n", lineCounter, start);
                    fclose(file);
                    return 0;
                }
                printf("use(%d)\n", ptr);
                // useP(ptr);
                break;

            case DELETE:
                if (sscanf(start, "delete(%d)", &ptr) != 1) {
                    printf("Error: Invalid format in line %d: %s\n", lineCounter, start);
                    fclose(file);
                    return 0;
                }
                printf("delete(%d)  \n", ptr);
                // deleteP(ptr);
                break;

            case KILL:
                if (sscanf(start, "kill(%d)", &pid) != 1) {
                    printf("Error: Invalid format in line %d: %s\n", lineCounter, start);
                    fclose(file);
                    return 0;
                }
                printf("kill(%d)\n", pid);

                // killP(pid);
                break;
        }
    }

    fclose(file);
    return 1;
}

int createFile(char *filename, InstructionList *instructionList){
    if(filename == NULL || instructionList == NULL){
        return -1;
    }

    FILE* file = fopen(filename, "w");
    if(file == NULL){
        return -1;
    }

    for(int i = 0; i < instructionList->count; i++){
        const Instruction* ins = &instructionList->instructions[i];
        int writeResult;
        switch (ins->operation)
        {
        case NEW:
            writeResult = fprintf(file, "new(%d,%d)\n",ins->pid, ins->size);
            break;

        case USE:
            writeResult = fprintf(file, "use(%d)\n",ins->ptr);
            break;

        case DELETE:
            writeResult = fprintf(file, "delete(%d)\n",ins->ptr);
            break;

        case KILL:
            writeResult = fprintf(file, "kill(%d)\n",ins->pid);
            break;
        
        default:
            writeResult = -1;
            break;
        }
        if(writeResult < 0){
            fclose(file);
            return -1;
        }

    }

    if(fclose(file) != 0){
        return -1;
    }
    
    return 0;
}






int main(int argc, char *argv[]) {
    //runFile("ejemplo.txt");
    InstructionList IL1;

    int seed = 67;
    int P = 3; //Cantidad de Procesos
    int N = 20; //Cantidad de Operaciones

    char processessFileName[50];
    
    if(generateInstructions(seed, P, N, &IL1) != 0 ){
        printf("Error generando");
        return 1;
    }

    printf("---Semilla %d---\n", seed);
    for(int i = 0; i < IL1.count; i++){
        printInstruction(&IL1.instructions[i]);
    }

    snprintf(processessFileName, sizeof processessFileName, "SecuenciaProcesos-S%d-P%d-N%d", seed, P, N);


    createFile(processessFileName, &IL1);

    freeInstructionList(&IL1);

    printf("\n---EjemploProfe---\n");
    runFile("ejemplo.txt");

    printf("\n---Semilla %d desde archivo ---\n", seed);
    runFile(processessFileName);
    
    return 0;
}