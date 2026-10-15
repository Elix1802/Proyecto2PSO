#include <stdio.h>
#include <stdlib.h>
#include "DataStructs/MMU.h"
#include "Files/FileManager.h"
char rutaSequencias[9999];

int main(void) {
    InstructionList IL1;

    int seed = 17;
    int P = 3; //Cantidad de Procesos
    int N = 20; //Cantidad de Operaciones

    char processessFileName[50];
    
    if(generateInstructions(seed, P, N, &IL1) != 0 ){
        printf("Error generando");
        return 1;
    }

    snprintf(processessFileName, sizeof processessFileName, "Files/Sequences/SecuenciaProcesos-S%d-P%d-N%d", seed, P, N);


    createFile(processessFileName, &IL1);

    freeInstructionList(&IL1);

    printf("\n---EjemploProfe---\n");
    runFile("Files/Sequences/ejemplo.txt");

    printf("\n---Semilla %d desde archivo ---\n", seed);
    printf("%s \n \n",processessFileName);
    runFile(processessFileName);
    
    return 0;

    return 0;
}