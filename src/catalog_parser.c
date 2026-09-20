#include "catalog_parser.h"
#include "validator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Status parse_code_list(CodeList *list, const char *field) {
    if (!field || *field == '\0') return ERROR_INVALID_FORMAT;
    if (strcmp(field, EMPTY_FIELD) == 0) return SUCCESS;

    size_t len = strlen(field);
    if (field[0] == ';' || field[len - 1] == ';' || strstr(field, ";;"))
        return ERROR_INVALID_FORMAT;

    char *copy = malloc(len + 1);
    if (!copy) return ERROR_MEMORY;
    strcpy(copy, field);

    char *start = copy;

    while (start && *start) {
        char *delim = strstr(start, LIST_DELIMITER);

        if (delim)
            *delim = '\0';

        if (!code_list_add(list, start)) {
            free(copy);
            return ERROR_MEMORY;
        }

        start = delim ? delim + strlen(LIST_DELIMITER) : NULL;
    }

    free(copy);
    return SUCCESS;
}

static int code_list_equals(const CodeList *a, const CodeList *b) {
    if (a->count != b->count) return 0;

    for (size_t i = 0; i < a->count; i++) {
        int count_a = 0, count_b = 0;

        for (size_t j = 0; j < a->count; j++) {
            if (strcmp(a->items[j], a->items[i]) == 0) count_a++;
            if (strcmp(b->items[j], a->items[i]) == 0) count_b++;
        }

        if (count_a != count_b) return 0;
    }

    return 1;
}

static int valid_header(char **c, int count) {
    static const char *expected[] = {
        "carrera", "semestre", "codigo", "nombre", "creditos",
        "requisitos", "correquisitos", "grupo", "dia", "inicio", "fin"
    };

    if (count != 11) return 0;

    for (int i = 0; i < 11; i++)
        if (strcmp(c[i], expected[i]) != 0)
            return 0;

    return 1;
}

Status parse_catalog(const char *filepath, Catalog *catalog) {
    FILE *file = fopen(filepath, "r");
    if (!file) return ERROR_FILE_NOT_FOUND;

    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int header_parsed = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        line[strcspn(line, "\r\n")] = '\0';

        if (*line == '\0')
            continue;

        char *columns[12] = {0};
        int col_count = 0;
        char *start = line;

        while (start) {
            if (col_count < 12)
                columns[col_count] = start;

            col_count++;

            char *tab = strchr(start, '\t');

            if (tab) {
                *tab = '\0';
                start = tab + 1;
            } else {
                start = NULL;
            }
        }

        if (!header_parsed) {
            if (!valid_header(columns, col_count)) {
                printf("Error: Encabezado TSV inválido en línea %d.\n", line_number);
                fclose(file);
                catalog_free(catalog);
                return ERROR_INVALID_FORMAT;
            }

            header_parsed = 1;
            continue;
        }

        if (validate_column_count(col_count) != SUCCESS) {
            printf("Error: Cantidad de columnas incorrecta en línea %d.\n", line_number);
            fclose(file);
            catalog_free(catalog);
            return ERROR_INVALID_FORMAT;
        }

        const char *career = columns[0];
        const char *semester_str = columns[1];
        const char *code = columns[2];
        const char *name = columns[3];
        const char *credits_str = columns[4];
        const char *req_str = columns[5];
        const char *coreq_str = columns[6];
        const char *group_str = columns[7];
        const char *day_str = columns[8];
        const char *start_str = columns[9];
        const char *end_str = columns[10];

        if (!career || *career == '\0' ||
            strcmp(career, EMPTY_FIELD) == 0 ||
            strlen(career) >= MAX_CAREER_CODE_LENGTH) {
            fclose(file);
            catalog_free(catalog);
            return ERROR_INVALID_FORMAT;
        }

        if (!catalog->career_code) {
            catalog->career_code = malloc(strlen(career) + 1);

            if (!catalog->career_code) {
                fclose(file);
                catalog_free(catalog);
                return ERROR_MEMORY;
            }

            strcpy(catalog->career_code, career);
        } else if (strcmp(catalog->career_code, career) != 0) {
            fclose(file);
            catalog_free(catalog);
            return ERROR_INVALID_FORMAT;
        }

        if (validate_course_identity(code, name) != SUCCESS ||
            validate_day_format(day_str) != SUCCESS ||
            validate_time_format(start_str, end_str) != SUCCESS ||
            validate_semester_format(semester_str) != SUCCESS ||
            validate_credits_format(credits_str) != SUCCESS ||
            validate_group_format(group_str) != SUCCESS) {

            fclose(file);
            catalog_free(catalog);
            return ERROR_INVALID_FORMAT;
        }

        int semester = atoi(semester_str);
        int credits = atoi(credits_str);
        int group_number = atoi(group_str);

        Day day = parse_day_string(day_str);

        int sh, sm, eh, em;
        sscanf(start_str, "%d:%d", &sh, &sm);
        sscanf(end_str, "%d:%d", &eh, &em);

        int start_minutes = sh * 60 + sm;
        int end_minutes = eh * 60 + em;

        Course *course = find_course_by_code(catalog, code);

        if (!course) {
            course = catalog_add_course(catalog, code, name, credits, semester);

            if (!course) {
                fclose(file);
                catalog_free(catalog);
                return ERROR_MEMORY;
            }

            Status r1 = parse_code_list(&course->requirements, req_str);
            Status r2 = parse_code_list(&course->corequisites, coreq_str);

            if (r1 != SUCCESS || r2 != SUCCESS) {
                fclose(file);
                catalog_free(catalog);
                return r1 != SUCCESS ? r1 : r2;
            }

        } else {
            CodeList temp_req, temp_coreq;
            code_list_init(&temp_req);
            code_list_init(&temp_coreq);

            Status r1 = parse_code_list(&temp_req, req_str);
            Status r2 = parse_code_list(&temp_coreq, coreq_str);

            if (r1 != SUCCESS || r2 != SUCCESS) {
                code_list_free(&temp_req);
                code_list_free(&temp_coreq);
                fclose(file);
                catalog_free(catalog);
                return r1 != SUCCESS ? r1 : r2;
            }

            int contradiction =
                course->semester != semester ||
                course->credits != credits ||
                strcmp(course->name, name) != 0 ||
                !code_list_equals(&course->requirements, &temp_req) ||
                !code_list_equals(&course->corequisites, &temp_coreq);

            code_list_free(&temp_req);
            code_list_free(&temp_coreq);

            if (contradiction) {
                fclose(file);
                catalog_free(catalog);
                return ERROR_INVALID_FORMAT;
            }
        }

        Group *group = find_group_by_number(course, group_number);

        if (!group) {
            group = course_add_group(course, group_number);

            if (!group) {
                fclose(file);
                catalog_free(catalog);
                return ERROR_MEMORY;
            }
        }

        if (!group_add_schedule(group, day, start_minutes, end_minutes)) {
            fclose(file);
            catalog_free(catalog);
            return ERROR_MEMORY;
        }
    }

    fclose(file);
    return SUCCESS;
}