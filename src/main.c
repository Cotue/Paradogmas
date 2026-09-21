#include <stdio.h>
#include "constants.h"
#include "catalog_parser.h"
#include "models/catalog.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <catalogo.tsv>\n", argv[0]);
        return ERROR_ARGS;
    }

    Catalog catalog;
    catalog_init(&catalog);

    Status result = parse_catalog(argv[1], &catalog);
    if (result == SUCCESS) {
        printf("Catalogo cargado exitosamente.\n");
    }

    catalog_free(&catalog);
    return result;
}