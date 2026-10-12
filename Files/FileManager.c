#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




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



int main(int argc, char *argv[]) {


    //runFile("ejemplo.txt");
    printf("Semilla 1\n");
    rngSeed(1);
    for(int i = 0; i < 10; i++) {
        printf("%d ", rngRand(1, 100));
    }
    printf("\n");

    printf("Semilla 2\n");
    rngSeed(2);
    for(int i = 0; i < 10; i++) {
        printf("%d ", rngRand(1, 100));
    }
    printf("\n");

    printf("Semilla 1 de nuevo\n");
    rngSeed(1);
    for(int i = 0; i < 10; i++) {
        printf("%d ", rngRand(1, 100));
    }
    printf("\n");

    return 0;
}