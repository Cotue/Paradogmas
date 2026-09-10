#include <stdio.h>
#include "constants.h"
#include "models.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    printf("  CEmestre v%s\n", OUTPUT_SCHEMA_VERSION);
    printf("  Entorno de compilacion verificado.\n");

    return SUCCESS;
}