#include <stdio.h>
#include "constants.h"
#include "catalog_parser.h"
#include "history_parser.h"
#include "models/catalog.h"
#include "models/student_history.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <catalogo.tsv> <historial.txt>\n", argv[0]);
        return ERROR_ARGS;
    }

    Catalog catalog;
    catalog_init(&catalog);

    StudentHistory history;
    student_history_init(&history);

    Status result = parse_catalog(argv[1], &catalog);
    if (result == SUCCESS) {
        result = parse_student_history(argv[2], &catalog, &history);
        if (result == SUCCESS) {
            printf("Carga exitosa.\n");
        }
    }

    student_history_free(&history);
    catalog_free(&catalog);

    return result;
}