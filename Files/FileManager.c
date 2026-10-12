#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "FileManager.h"




enum OP {
    NEW = 1,
    USE = 2,
    DELETE = 3,
    KILL = 4
};

typedef struct Instruction {
    enum OP operation;
    int pid; //Para New y Kill
    int size; //Para New
    int ptr; //Para Use y Delete
    int line;
} Instruction;

//Semilla y random
static void rngSeed(unsigned int seed) {
    srand(seed);
}

static int rngRand(int min, int max) {
    return min + rand() % (max - min + 1);
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



int main(int argc, char *argv[]) {


    //runFile("ejemplo.txt");

    InstructionList list;
    initInstructionList(&list);

    //Crear las instrucciones
    for (int i = 0; i < 300; i++) {
        if (appendInstruction(&list, NEW, 1, 100 + i, i + 1) != 0) {
            
            printf("Error de memoria\n");
            return 1;
        }
    }
    appendInstruction(&list, USE, 1, 0, 5);
    appendInstruction(&list, KILL, 1, 0, 0);

    printf("count=%d capacity=%d\n", list.count, list.capacity);
    printf("primera: op=%d pid=%d size=%d ptr=%d line=%d\n",
           list.instructions[0].operation, list.instructions[0].pid,
           list.instructions[0].size, list.instructions[0].ptr,
           list.instructions[0].line);
    printf("ultima: op=%d pid=%d line=%d\n",
           list.instructions[list.count - 1].operation,
           list.instructions[list.count - 1].pid,
           list.instructions[list.count - 1].line);

    freeInstructionList(&list);
    printf("tras free: count=%d ptr=%p\n", list.count, (void*)list.instructions);
    return 0;
}