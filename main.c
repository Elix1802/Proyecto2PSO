#include <stdio.h>
#include <stdlib.h>
#include "MMU.h"
#include "DataStructs/NodePage.h"

int main() {
    printf("=== PRUEBA DE LA MMU ===\n");

    int total_memory = 400000;
    int page_size = 4096;
    int alg_id = 0; 

    MMU *mmu = createMMU(alg_id, total_memory, page_size);
    if (mmu == NULL) {
        printf("Error: Fallo al crear la MMU.\n");
        return 1;
    }
    printf("[OK] MMU creada correctamente.\n");

    int pid = 1;
    int size = 5000;
    int ptr = newP(mmu, pid, size);

    if (ptr != -1) {
        printf("[OK] Instrucción newP ejecutada con éxito. Puntero lógico asignado (ptr): %d\n", ptr);
    } else {
        printf("Error: newP retornó -1.\n");
    }

    int count = 0;
    NodePage **pages = getPages(mmu, pid, &count);
    if (pages != NULL && count > 0) {
        printf("[OK] getPages funcionó correctamente. Páginas encontradas para PID %d: %d\n", pid, count);
        free(pages); 
    } else {
        printf("Aviso: getPages no encontró páginas (verificar si findProcess está implementado).\n");
    }

    destroyMMU(mmu);
    printf("[OK] MMU destruida exitosamente. Pruebas finalizadas.\n");

    return 0;
}