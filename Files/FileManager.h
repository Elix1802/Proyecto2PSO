#ifndef FileManager_H
#define FileManager_H   

enum OP{
    NEW = 1,
    USE = 2,
    DELETE = 3, 
    KILL = 4
};

typedef struct Instruction {
    enum OP operation;
    int pid; //New y Kill
    int size; //New
    int ptr; //Use y Delete
    int line; 
}Instruction;

typedef struct {
    Instruction* instructions;
    int count;
    int capacity;
} InstructionList;

//P: numero de procesos a simular 
//N: cantidad de instrucciones a generar
int generateInstructions(unsigned int seed, int P, int N, InstructionList* instructionList);
int runFile(char *filename);

void initInstructionList(InstructionList* instructionList);
void addInstruction(InstructionList* instructionList, Instruction instruction);
void freeInstructionList(InstructionList* instructionList);

#endif