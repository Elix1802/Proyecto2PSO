#ifndef FileManager_H
#define FileManager_H   

enum OP{
    NEW = 1,
    USE = 2,
    DELETE = 3, 
    KILL = 4
};

typedef struct {
    OP operation;
    int pid;
    int size;
    int ptr;
    int line;
}Instruction;

typedef struct {
    Instruction* instructions;
    int count;
} InstructionList;

//P: numero de procesos a simular 
//N: cantidad de instrucciones a generar
int generateInstructions(unsigned int seed, int P, int N, InstructionList* instructionList);
int runFile(char *filename);

void freeInstructionList(InstructionList* instructionList);

#endif