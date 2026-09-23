#include <stdio.h>
#include "catalog_parser.h"
#include "history_parser.h"
#include "models/catalog.h"
#include "models/student_history.h"
#include "constants.h"

static int run_integration_test(const char *name, const char *cat_file,
    const char *hist_file, Status expected,
    size_t expected_cat_count, size_t expected_hist_count) {

    Catalog catalog;
    catalog_init(&catalog);

    StudentHistory history;
    student_history_init(&history);

    Status result = parse_catalog(cat_file, &catalog);
    if (result == SUCCESS) {
        result = parse_student_history(hist_file, &catalog, &history);
    }

    int failed = (result != expected);

    if (!failed && expected == SUCCESS) {
        if (catalog.course_count != expected_cat_count) {
            failed = 1;
        }

        if (history.approved_courses.count != expected_hist_count) {
            failed = 1;
        }
    }

    student_history_free(&history);
    catalog_free(&catalog);

    printf("%-45s : %s\n", name, failed ? "FAIL" : "PASS");
    return failed;
}

int main(void) {
    int fails = 0;

    printf("\n=== TESTS KAN-11: INTEGRACION ===\n\n");

    fails += run_integration_test("Entrada valida",
        "tests/fixtures/integration/catalog_valid.tsv",
        "tests/fixtures/integration/history_valid.txt",
        SUCCESS, 2, 2);

    fails += run_integration_test("Catalogo mal formado",
        "tests/fixtures/integration/catalog_malformed.tsv",
        "tests/fixtures/integration/history_valid.txt",
        ERROR_INVALID_FORMAT, 0, 0);

    fails += run_integration_test("Horario invalido (catalogo)",
        "tests/fixtures/integration/catalog_invalid_schedule.tsv",
        "tests/fixtures/integration/history_valid.txt",
        ERROR_INVALID_FORMAT, 0, 0);

    fails += run_integration_test("Historial duplicado",
        "tests/fixtures/integration/catalog_valid.tsv",
        "tests/fixtures/integration/history_duplicate.txt",
        ERROR_INVALID_FORMAT, 0, 0);

    fails += run_integration_test("Codigo desconocido (historial)",
        "tests/fixtures/integration/catalog_valid.tsv",
        "tests/fixtures/integration/history_unknown.txt",
        ERROR_INVALID_FORMAT, 0, 0);

    printf("\nFallos totales: %d\n", fails);
    return fails ? 1 : 0;
}