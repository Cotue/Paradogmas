#include "catalog_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "validator.h"
#include <string.h>

static int parse_code_list(CodeList *list, const char *field) {
    if (!field || strcmp(field, EMPTY_FIELD) == 0 || strlen(field) == 0) {
        return 1; // Correctamente vacío
    }

    char *field_copy = malloc(strlen(field) + 1);
    if (!field_copy) return 0;
    strcpy(field_copy, field);

    char *start = field_copy;
    char *delim;
    int success = 1;

    while (start && *start != '\0') {
        delim = strstr(start, LIST_DELIMITER);
        if (delim) {
            *delim = '\0';
        }
        
        if (strlen(start) > 0) {
            if (!code_list_add(list, start)) {
                success = 0;
                break;
            }
        }
        start = delim ? delim + strlen(LIST_DELIMITER) : NULL;
    }

    free(field_copy);
    return success;
}

Status parse_catalog(const char *filepath, Catalog *catalog) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        return ERROR_FILE_NOT_FOUND;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        line[strcspn(line, "\r\n")] = 0; 
        if (strlen(line) == 0) continue;

        char *columns[11] = {NULL};
        int col_count = 0;
        char *start = line;
        
        // Parsing manual usando strchr para preservar campos vacíos
        while (start && col_count < 11) {
            columns[col_count++] = start;
            char *tab = strchr(start, '\t');
            if (tab) {
                *tab = '\0';
                start = tab + 1;
            } else {
                start = NULL;
            }
        }

        if (col_count != 11) {
            printf("Error: Columnas incorrectas en linea %d.\n", line_number);
            fclose(file);
            catalog_free(catalog);
            return ERROR_INVALID_FORMAT;
        }

        const char *codigo = columns[2];
        const char *nombre = columns[3];
        const char *creditos_str = columns[4];
        const char *requisitos_str = columns[5];
        const char *correquisitos_str = columns[6];
        const char *grupo_str = columns[7];
        
        // [INTEGRACIÓN 7B]: Validaciones de formato aquí.
// [INTEGRACIÓN 7B]: Validaciones de formato
        if (validate_course_identity(codigo, nombre) != SUCCESS) {
            printf("Error: Código o nombre de curso inválido en línea %d.\n", line_number);
            fclose(file);
            catalog_free(catalog);
            return ERROR_INVALID_FORMAT;
        }

        if (validate_day_format(columns[8]) != SUCCESS) {
            printf("Error: Día inválido en línea %d.\n", line_number);
            fclose(file);
            catalog_free(catalog);
            return ERROR_INVALID_FORMAT;
        }

        if (validate_time_format(columns[9], columns[10]) != SUCCESS) {
            printf("Error: Horario inválido en línea %d.\n", line_number);
            fclose(file);
            catalog_free(catalog);
            return ERROR_INVALID_FORMAT;
        }

        int creditos = atoi(creditos_str);
        int semestre = atoi(columns[1]);
        int grupo_num = atoi(grupo_str);
        
        // [INTEGRACIÓN 7B]: Mapeo de Día
        // [INTEGRACIÓN 7B]: Mapeo de Día
        Day dia_enum = DAY_INVALID;
        if (strcmp(columns[8], "LUN") == 0) dia_enum = MONDAY;
        else if (strcmp(columns[8], "MAR") == 0) dia_enum = TUESDAY;
        else if (strcmp(columns[8], "MIE") == 0) dia_enum = WEDNESDAY;
        else if (strcmp(columns[8], "JUE") == 0) dia_enum = THURSDAY;
        else if (strcmp(columns[8], "VIE") == 0) dia_enum = FRIDAY;
        else if (strcmp(columns[8], "SAB") == 0) dia_enum = SATURDAY;

        // [INTEGRACIÓN 7B]: Conversión de horas a minutos desde medianoche
        int h_ini, m_ini, h_fin, m_fin;
        sscanf(columns[9], "%d:%d", &h_ini, &m_ini);
        sscanf(columns[10], "%d:%d", &h_fin, &m_fin);
        
        int inicio_minutos = (h_ini * 60) + m_ini;
        int fin_minutos = (h_fin * 60) + m_fin;

        
    

        Course *course = find_course_by_code(catalog, codigo);
        if (!course) {
            course = catalog_add_course(catalog, codigo, nombre, creditos, semestre);
            if (!course) {
                fclose(file);
                catalog_free(catalog);
                return ERROR_MEMORY;
            }
            if (!parse_code_list(&course->requirements, requisitos_str) ||
                !parse_code_list(&course->corequisites, correquisitos_str)) {
                fclose(file);
                catalog_free(catalog);
                return ERROR_MEMORY;
            }
        } else {
            if (strcmp(course->name, nombre) != 0 || course->credits != creditos) {
                printf("Error: Contradiccion en el curso %s.\n", codigo);
                fclose(file);
                catalog_free(catalog);
                return ERROR_INVALID_FORMAT;
            }
        }

        Group *group = find_group_by_number(course, grupo_num);
        if (!group) {
            group = course_add_group(course, grupo_num);
            if (!group) {
                fclose(file);
                catalog_free(catalog);
                return ERROR_MEMORY;
            }
        }

        if (!group_add_schedule(group, dia_enum, inicio_minutos, fin_minutos)) {
            fclose(file);
            catalog_free(catalog);
            return ERROR_MEMORY;
        }
    }

    fclose(file);
    return SUCCESS;
}