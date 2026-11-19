#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog_parser.h"

#define TEMP_FILE "temp_test.tsv"
#define HEADER_12 "carrera\tsemestre\tcodigo\tnombre\tcreditos\tarea\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
#define HEADER_11 "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"

#define CARRERA_CE "computadores"
#define CARRERA_MA "matematica"

static int run_test(const char *name, const char *plan_code, const char *content, Status expected) {
    FILE *f = fopen(TEMP_FILE, "w");

    if (!f) {
        printf("%-45s : FAIL (no se pudo crear archivo)\n", name);
        return 1;
    }

    fputs(content, f);
    fclose(f);

    Catalog catalog;
    catalog_init(&catalog);

    Status result = parse_catalog(TEMP_FILE, &catalog, plan_code);

    if (result == SUCCESS)
        catalog_free(&catalog);

    remove(TEMP_FILE);

    int failed = result != expected;

    printf("%-45s : %s\n", name, failed ? "FAIL" : "PASS");

    return failed;
}

static int test_loaded_course_fields(void) {
    const char *name = "Campos career y area del curso cargado";
    const char *content =
        HEADER_12
        CARRERA_MA "\t1\tMA1102\tCalculo\t4\tmatematicas\t-\t-\t1\tLUN\t07:30\t09:20\n";

    FILE *f = fopen(TEMP_FILE, "w");
    if (!f) {
        printf("%-45s : FAIL (no se pudo crear archivo)\n", name);
        return 1;
    }

    fputs(content, f);
    fclose(f);

    Catalog catalog;
    catalog_init(&catalog);

    Status result = parse_catalog(TEMP_FILE, &catalog, "IC");
    remove(TEMP_FILE);

    int failed = 1;
    if (result == SUCCESS && catalog.course_count == 1) {
        Course *c = &catalog.courses[0];
        if (c->career && strcmp(c->career, CARRERA_MA) == 0 &&
            c->area && strcmp(c->area, "matematicas") == 0) {
            failed = 0;
        }
    }

    if (result == SUCCESS)
        catalog_free(&catalog);

    printf("%-45s : %s\n", name, failed ? "FAIL" : "PASS");
    return failed;
}

int main(void) {
    int fails = 0;

    Catalog catalog;
    catalog_init(&catalog);

    Status missing = parse_catalog("no_existe.tsv", &catalog, "IC");

    printf("%-45s : %s\n",
           "Archivo inexistente",
           missing == ERROR_FILE_NOT_FOUND ? "PASS" : "FAIL");

    fails += missing != ERROR_FILE_NOT_FOUND;

    fails += run_test(
        "Encabezado valido",
        "IC",
        HEADER_12,
        SUCCESS
    );

    fails += run_test(
        "Encabezado viejo de 11 columnas",
        "IC",
        HEADER_11,
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Encabezado invalido",
        "IC",
        "carrera\tcodigo\tsemestre\tnombre\tcreditos\tarea\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Plan invalido XX",
        "XX",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Plan valido IE",
        "IE",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Linea vacia antes de encabezado",
        "IC",
        "\n\n"
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Carrera vacia",
        "IC",
        HEADER_12
        "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Carrera '-'",
        "IC",
        HEADER_12
        "-\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Carrera distinta al prefijo",
        "IC",
        HEADER_12
        CARRERA_MA "\t1\tCE1102\tTaller\t2\tcomputadores\t-\t-\t1\tMAR\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Prefijo desconocido",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tZZ1101\tRaro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Area vacia",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\t\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Area '-'",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\t-\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Area 'otros'",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\totros\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Area con mayuscula 'Matematicas'",
        "IC",
        HEADER_12
        CARRERA_MA "\t1\tMA1102\tCalculo\t4\tMatematicas\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Area contradictoria en mismo curso",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n"
        CARRERA_CE "\t1\tCE1101\tIntro\t4\toptativas\t-\t-\t2\tMAR\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Curso CE con area optativas valido",
        "IC",
        HEADER_12
        CARRERA_CE "\t6\tCE4101\tTopicos\t3\toptativas\t-\t-\t1\tLUN\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Requisitos vacios",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Correquisitos vacios",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Lista A;",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\tA;\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Lista A;;B",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\tA;;B\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Requisitos separados con coma",
        "IC",
        HEADER_12
        CARRERA_CE "\t2\tCE1102\tTaller\t2\tcomputadores\tCE1101, MA1102\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Creditos contradictorios",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n"
        CARRERA_CE "\t1\tCE1101\tIntro\t3\tcomputadores\t-\t-\t2\tMAR\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Correquisitos contradictorios",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\tA\t1\tLUN\t07:30\t09:20\n"
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\tB\t2\tMAR\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Requisitos distinto orden",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\tA;B\t-\t1\tLUN\t07:30\t09:20\n"
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\tB;A\t-\t2\tMAR\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Multiples horarios",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n"
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tJUE\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Dos grupos",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n"
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t2\tMAR\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Creditos invalidos",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\tX\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Horario invalido",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t25:00\t26:00\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Horario invertido",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t09:20\t07:30\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Menos de 12 columnas",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Mas de 12 columnas",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\tEXTRA\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Dia invalido",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tDOM\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Semestre 7 valido",
        "IC",
        HEADER_12
        CARRERA_CE "\t7\tCE4101\tRedes\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Semestre 8 valido",
        "IC",
        HEADER_12
        CARRERA_CE "\t8\tCE5101\tProyecto\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Semestre invalido",
        "IC",
        HEADER_12
        CARRERA_CE "\t9\tCE1101\tIntro\t4\tcomputadores\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Grupo invalido",
        "IC",
        HEADER_12
        CARRERA_CE "\t1\tCE1101\tIntro\t4\tcomputadores\t-\t-\t0\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += test_loaded_course_fields();

    printf("\nFallos totales: %d\n", fails);

    return fails ? 1 : 0;
}