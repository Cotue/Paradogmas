#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serializer.h"
#include "models/catalog.h"
#include "models/course.h"
#include "models/group.h"

#define TEMP_JSON "temp_test_serializer.json"

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (copy) {
        memcpy(copy, s, len + 1);
    }
    return copy;
}

static char *read_entire_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }

    long size = ftell(f);
    if (size < 0) {
        fclose(f);
        return NULL;
    }
    rewind(f);

    char *buf = malloc((size_t)size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t read_bytes = fread(buf, 1, (size_t)size, f);
    buf[read_bytes] = '\0';
    fclose(f);
    return buf;
}

static int file_exists(const char *path) {
    FILE *f = fopen(path, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

static int test_valid_serialization(void) {
    const char *name = "Serializacion valida con schema 3.0, carrera y area";
    remove(TEMP_JSON);

    Catalog catalog;
    catalog_init(&catalog);
    catalog.career_code = dup_str("IC");
    catalog.career_name = dup_str("Ingenieria en Computadores");

    Course *c1 = catalog_add_course(
        &catalog, "CE1101", "Intro", "computadores", "computadores", 4, 1
    );
    Course *c2 = catalog_add_course(
        &catalog, "MA1102", "Calculo", "matematica", "matematicas", 4, 1
    );

    int failed = 1;
    if (catalog.career_code && catalog.career_name && c1 && c2) {
        Group *g1 = course_add_group(c1, 1);
        Group *g2 = course_add_group(c2, 1);

        if (g1 && g2 &&
            group_add_schedule(g1, MONDAY, 450, 560) &&
            group_add_schedule(g2, TUESDAY, 450, 560)) {

            Status res = serialize_catalog_to_json(&catalog, TEMP_JSON);
            char *json = read_entire_file(TEMP_JSON);

            if (res == SUCCESS && json &&
                strstr(json, "\"schemaVersion\": \"3.0\"") &&
                strstr(json, "\"plan\": {") &&
                strstr(json, "\"codigo\": \"IC\"") &&
                strstr(json, "\"carrera\": \"computadores\"") &&
                strstr(json, "\"area\": \"computadores\"") &&
                strstr(json, "\"carrera\": \"matematica\"") &&
                strstr(json, "\"area\": \"matematicas\"")) {
                failed = 0;
            }
            free(json);
        }
    }

    remove(TEMP_JSON);
    catalog_free(&catalog);

    printf("%-55s : %s\n", name, failed ? "FAIL" : "PASS");
    return failed;
}

static int test_null_career_fails_without_file(void) {
    const char *name = "Curso con career en NULL devuelve error y no crea archivo";
    remove(TEMP_JSON);

    Catalog catalog;
    catalog_init(&catalog);
    catalog.career_code = dup_str("IC");
    catalog.career_name = dup_str("Ingenieria en Computadores");

    Course *c1 = catalog_add_course(
        &catalog, "CE1101", "Intro", "computadores", "computadores", 4, 1
    );

    int failed = 1;
    if (catalog.career_code && catalog.career_name && c1) {
        free(c1->career);
        c1->career = NULL;

        Status res = serialize_catalog_to_json(&catalog, TEMP_JSON);
        if (res == ERROR_INVALID_FORMAT && !file_exists(TEMP_JSON)) {
            failed = 0;
        }
    }

    remove(TEMP_JSON);
    catalog_free(&catalog);

    printf("%-55s : %s\n", name, failed ? "FAIL" : "PASS");
    return failed;
}

static int test_null_area_fails_without_file(void) {
    const char *name = "Curso con area en NULL devuelve error y no crea archivo";
    remove(TEMP_JSON);

    Catalog catalog;
    catalog_init(&catalog);
    catalog.career_code = dup_str("IC");
    catalog.career_name = dup_str("Ingenieria en Computadores");

    Course *c1 = catalog_add_course(
        &catalog, "CE1101", "Intro", "computadores", "computadores", 4, 1
    );

    int failed = 1;
    if (catalog.career_code && catalog.career_name && c1) {
        free(c1->area);
        c1->area = NULL;

        Status res = serialize_catalog_to_json(&catalog, TEMP_JSON);
        if (res == ERROR_INVALID_FORMAT && !file_exists(TEMP_JSON)) {
            failed = 0;
        }
    }

    remove(TEMP_JSON);
    catalog_free(&catalog);

    printf("%-55s : %s\n", name, failed ? "FAIL" : "PASS");
    return failed;
}

static int test_null_filepath(void) {
    const char *name = "Ruta de salida NULL devuelve ERROR_ARGS";

    Catalog catalog;
    catalog_init(&catalog);

    Status res = serialize_catalog_to_json(&catalog, NULL);
    int failed = res != ERROR_ARGS;

    catalog_free(&catalog);

    printf("%-55s : %s\n", name, failed ? "FAIL" : "PASS");
    return failed;
}

int main(void) {
    int fails = 0;

    fails += test_valid_serialization();
    fails += test_null_career_fails_without_file();
    fails += test_null_area_fails_without_file();
    fails += test_null_filepath();

    printf("\nFallos totales: %d\n", fails);
    return fails ? 1 : 0;
}