#include "serializer.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Convierte un enum Day a la cadena de texto esperada por el contrato de JSON
static const char* day_to_string(Day day) {
    switch (day) {
        case MONDAY:    return "LUN";
        case TUESDAY:   return "MAR";
        case WEDNESDAY: return "MIE";
        case THURSDAY:  return "JUE";
        case FRIDAY:    return "VIE";
        case SATURDAY:  return "SAB";
        default:        return "UNK";
    }
}

// Imprime minutos desde medianoche a formato HH:MM en JSON
static void print_time_json(FILE *file, int minutes) {
    int h = minutes / 60;
    int m = minutes % 60;
    fprintf(file, "\"%02d:%02d\"", h, m);
}

// Imprime una estructura CodeList en formato de arreglo JSON ["CE1101", "MA1102"]
static void print_code_list_json(FILE *file, const CodeList *list) {
    fprintf(file, "[");
    if (list) {
        for (size_t i = 0; i < list->count; i++) {
            fprintf(file, "\"%s\"", list->items[i]);
            if (i < list->count - 1) {
                fprintf(file, ", ");
            }
        }
    }
    fprintf(file, "]");
}

Status serialize_catalog_to_json(const Catalog *catalog, const char *filepath) {
    if (!catalog || !filepath) {
        return ERROR_ARGS;
    }

    FILE *file = fopen(filepath, "w");
    if (!file) {
        return ERROR_FILE_NOT_FOUND;
    }

    // Encabezado principal del archivo JSON
    fprintf(file, "{\n");
    fprintf(file, "  \"schemaVersion\": \"%s\",\n", OUTPUT_SCHEMA_VERSION);
    fprintf(file, "  \"carrera\": {\n");
    fprintf(file, "    \"codigo\": \"%s\",\n", catalog->career_code ? catalog->career_code : "IC");
    fprintf(file, "    \"nombre\": \"%s\"\n", catalog->career_name ? catalog->career_name : "Ingeniería en Computadores");
    fprintf(file, "  },\n");
    fprintf(file, "  \"cursos\": [\n");

    for (size_t i = 0; i < catalog->course_count; i++) {
        Course *c = &catalog->courses[i];

        fprintf(file, "    {\n");
        fprintf(file, "      \"codigo\": \"%s\",\n", c->code);
        fprintf(file, "      \"nombre\": \"%s\",\n", c->name);
        fprintf(file, "      \"creditos\": %d,\n", c->credits);
        fprintf(file, "      \"semestre\": %d,\n\n", c->semester);

        // 1. Requisitos, Correquisitos y Correquisitos Pendientes (al inicio del objeto Curso)
        fprintf(file, "      \"requisitos\": ");
        print_code_list_json(file, &c->requirements);
        fprintf(file, ",\n");

        fprintf(file, "      \"correquisitos\": ");
        print_code_list_json(file, &c->corequisites);
        fprintf(file, ",\n");

        fprintf(file, "      \"correquisitosPendientes\": ");
        print_code_list_json(file, &c->pending_corequisites);
        fprintf(file, ",\n\n");

        // 2. Grupos y Horarios
        fprintf(file, "      \"grupos\": [\n");
        for (size_t j = 0; j < c->group_count; j++) {
            Group *g = &c->groups[j];

            fprintf(file, "        {\n");
            fprintf(file, "          \"numero\": %d,\n", g->number);
            fprintf(file, "          \"horarios\": [\n");

            for (size_t k = 0; k < g->schedule_count; k++) {
                Schedule *s = &g->schedules[k];

                fprintf(file, "            {\n");
                fprintf(file, "              \"dia\": \"%s\",\n", day_to_string(s->day));
                fprintf(file, "              \"inicio\": ");
                print_time_json(file, s->start_minutes);
                fprintf(file, ",\n");
                fprintf(file, "              \"fin\": ");
                print_time_json(file, s->end_minutes);
                fprintf(file, "\n            }");

                if (k < g->schedule_count - 1) fprintf(file, ",");
                fprintf(file, "\n");
            }

            fprintf(file, "          ],\n");
            fprintf(file, "          \"tieneChoque\": %s,\n", g->has_conflict ? "true" : "false");
            
            // Detalle de choques por grupo
            fprintf(file, "          \"choques\": [\n");
            for (size_t l = 0; l < g->conflict_count; l++) {
                fprintf(file, "            {\n");
                fprintf(file, "              \"curso\": \"%s\",\n", g->conflicts[l].course_code);
                fprintf(file, "              \"grupo\": %d\n", g->conflicts[l].group_number);
                fprintf(file, "            }");
                if (l < g->conflict_count - 1) fprintf(file, ",");
                fprintf(file, "\n");
            }
            fprintf(file, "          ]\n");

            fprintf(file, "        }");
            if (j < c->group_count - 1) fprintf(file, ",");
            fprintf(file, "\n");
        }
        fprintf(file, "      ],\n\n");

        // 3. Banderas finales a nivel de Curso (exactamente según el contrato)
        fprintf(file, "      \"tieneChoque\": %s,\n", c->has_conflict ? "true" : "false");
        fprintf(file, "      \"puedeMatricular\": %s\n", c->can_enroll ? "true" : "false");

        fprintf(file, "    }");
        if (i < catalog->course_count - 1) fprintf(file, ",");
        fprintf(file, "\n");
    }

    fprintf(file, "  ]\n");
    fprintf(file, "}\n");

    fclose(file);
    return SUCCESS;
}