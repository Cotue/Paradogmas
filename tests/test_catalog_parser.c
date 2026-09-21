#include <stdio.h>
#include <stdlib.h>
#include "catalog_parser.h"

#define TEMP_FILE "temp_test.tsv"

static int run_test(const char *name, const char *content, Status expected) {
    FILE *f = fopen(TEMP_FILE, "w");

    if (!f) {
        printf("%-45s : FAIL (no se pudo crear archivo)\n", name);
        return 1;
    }

    fputs(content, f);
    fclose(f);

    Catalog catalog;
    catalog_init(&catalog);

    Status result = parse_catalog(TEMP_FILE, &catalog);

    if (result == SUCCESS)
        catalog_free(&catalog);

    remove(TEMP_FILE);

    int failed = result != expected;

    printf("%-45s : %s\n", name, failed ? "FAIL" : "PASS");

    return failed;
}

int main(void) {
    int fails = 0;

    Catalog catalog;
    catalog_init(&catalog);

    Status missing = parse_catalog("no_existe.tsv", &catalog);

    printf("%-45s : %s\n",
           "Archivo inexistente",
           missing == ERROR_FILE_NOT_FOUND ? "PASS" : "FAIL");

    fails += missing != ERROR_FILE_NOT_FOUND;

    fails += run_test(
        "Encabezado válido",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n",
        SUCCESS
    );

    fails += run_test(
        "Encabezado inválido",
        "carrera\tcodigo\tsemestre\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Línea vacía antes de encabezado",
        "\n\n"
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Carrera vacía",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Carrera '-'",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "-\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Carrera contradictoria",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\n"
        "MC\t1\tCE1102\tTaller\t2\t-\t-\t1\tMAR\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Requisitos vacíos",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Correquisitos vacíos",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Lista A;",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\tA;\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Lista A;;B",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\tA;;B\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Créditos contradictorios",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\n"
        "IC\t1\tCE1101\tIntro\t3\t-\t-\t2\tMAR\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Correquisitos contradictorios",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\tA\t1\tLUN\t07:30\t09:20\n"
        "IC\t1\tCE1101\tIntro\t4\t-\tB\t2\tMAR\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Requisitos distinto orden",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\tA;B\t-\t1\tLUN\t07:30\t09:20\n"
        "IC\t1\tCE1101\tIntro\t4\tB;A\t-\t2\tMAR\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Múltiples horarios",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tJUE\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Dos grupos",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t2\tMAR\t07:30\t09:20\n",
        SUCCESS
    );

    fails += run_test(
        "Créditos inválidos",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\tX\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Horario inválido",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t25:00\t26:00\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Horario invertido",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t09:20\t07:30\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Menos de 11 columnas",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Más de 11 columnas",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\tEXTRA\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Día inválido",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t1\tDOM\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Semestre inválido",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t5\tCE1101\tIntro\t4\t-\t-\t1\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    fails += run_test(
        "Grupo inválido",
        "carrera\tsemestre\tcodigo\tnombre\tcreditos\trequisitos\tcorrequisitos\tgrupo\tdia\tinicio\tfin\n"
        "IC\t1\tCE1101\tIntro\t4\t-\t-\t0\tLUN\t07:30\t09:20\n",
        ERROR_INVALID_FORMAT
    );

    printf("\nFallos totales: %d\n", fails);

    return fails ? 1 : 0;
}