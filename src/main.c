#include <stdio.h>
#include "constants.h"
#include "catalog_parser.h"
#include "history_parser.h"
#include "eligibility.h"
#include "conflict_detector.h" 
#include "serializer.h"

int main(int argc, char *argv[]) {
    // Se esperan 4 argumentos: catalogo, historial, salida y codigo del plan
    if (argc != 5) {
        printf("Uso: %s <catalogo.tsv> <historial.txt> <salida.json> <plan>\n", argv[0]);
        return ERROR_ARGS;
    }

    Catalog catalog;
    catalog_init(&catalog);

    StudentHistory history;
    student_history_init(&history);

    Status result = parse_catalog(argv[1], &catalog, argv[4]);
    if (result == SUCCESS) {
        result = parse_student_history(argv[2], &catalog, &history);
        if (result == SUCCESS) {
            
            // 1. Calcular choques de horario 
            detect_catalog_conflicts(&catalog);
            
            // 2. Evaluar requisitos con el historial
            evaluate_catalog_eligibility(&catalog, &history);

            // 3. Generar el JSON
            result = serialize_catalog_to_json(&catalog, argv[3]);
            
            if (result == SUCCESS) {
                printf("Carga exitosa. Archivo JSON generado en: %s\n", argv[3]);
            } else {
                printf("Error al generar el archivo JSON.\n");
            }
        }
    }

    student_history_free(&history);
    catalog_free(&catalog);

    return result;
}