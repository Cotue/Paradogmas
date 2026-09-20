/*
 * ============================================================
 * TESTS KAN-9 - Carga del historial académico
 * ============================================================
 *
 * Este test prueba directamente history_parser.c.
 *
 * ARCHIVOS NECESARIOS PARA COMPILAR:
 *
 * tests/test_history_parser.c
 * src/history_parser.c
 * src/models/catalog.c
 * src/models/code_list.c
 * src/models/course.c
 * src/models/group.c
 * src/models/schedule.c
 * src/models/student_history.c
 *
 * NO se necesitan:
 *
 * src/catalog_parser.c
 * src/validator.c
 *
 * porque este test crea manualmente un Catalog válido en memoria.
 * De esta forma se prueba KAN-9 de manera aislada.
 *
 * ------------------------------------------------------------
 * COMPILAR DESDE LA RAÍZ DEL PROYECTO EN POWERSHELL:
 *
 * gcc tests/test_history_parser.c `
 * src/history_parser.c `
 * src/models/catalog.c `
 * src/models/code_list.c `
 * src/models/course.c `
 * src/models/group.c `
 * src/models/schedule.c `
 * src/models/student_history.c `
 * -Iinclude `
 * -Wall -Wextra -Wpedantic `
 * -std=c11 `
 * -o test_history_parser.exe
 *
 * ------------------------------------------------------------
 * EJECUTAR:
 *
 * .\test_history_parser.exe
 *
 * Si todos los casos pasan:
 *
 * Fallos totales: 0
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history_parser.h"

#define TEMP_HISTORY_FILE "temp_history_test.txt"


/*
 * Crea un archivo temporal con el contenido recibido.
 *
 * Retorna:
 * 1 -> archivo creado correctamente
 * 0 -> error
 */
static int create_temp_history(const char *content)
{
    FILE *file = fopen(TEMP_HISTORY_FILE, "w");

    if (file == NULL) {
        return 0;
    }

    if (content != NULL) {
        fputs(content, file);
    }

    fclose(file);

    return 1;
}


/*
 * Imprime PASS o FAIL y retorna:
 *
 * 0 -> prueba correcta
 * 1 -> prueba fallida
 */
static int report_result(const char *name, int passed)
{
    printf(
        "%-50s : %s\n",
        name,
        passed ? "PASS" : "FAIL"
    );

    return passed ? 0 : 1;
}


/*
 * ============================================================
 * CRITERIO:
 * El historial se carga desde archivo.
 *
 * También verifica:
 * La información se almacena en StudentHistory.
 * ============================================================
 */
static int test_valid_history(Catalog *catalog)
{
    if (!create_temp_history(
            "CE1101\n"
            "MA1102\n"
            "FI1101\n")) {
        return report_result(
            "Historial válido",
            0
        );
    }

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        TEMP_HISTORY_FILE,
        catalog,
        &history
    );

    int passed =
        status == SUCCESS &&
        history.approved_courses.count == 3 &&
        strcmp(
            history.approved_courses.items[0],
            "CE1101"
        ) == 0 &&
        strcmp(
            history.approved_courses.items[1],
            "MA1102"
        ) == 0 &&
        strcmp(
            history.approved_courses.items[2],
            "FI1101"
        ) == 0;

    student_history_free(&history);
    remove(TEMP_HISTORY_FILE);

    return report_result(
        "Historial válido y almacenado",
        passed
    );
}


/*
 * ============================================================
 * CRITERIO:
 * Se eliminan espacios accidentales antes y después.
 *
 * El programa NO corrige el código.
 * Solamente elimina whitespace externo.
 *
 * "   CE1101   " -> "CE1101"
 * ============================================================
 */
static int test_external_whitespace(Catalog *catalog)
{
    if (!create_temp_history(
            "   CE1101   \n"
            "\tMA1102\t\n")) {
        return report_result(
            "Espacios externos",
            0
        );
    }

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        TEMP_HISTORY_FILE,
        catalog,
        &history
    );

    int passed =
        status == SUCCESS &&
        history.approved_courses.count == 2 &&
        strcmp(
            history.approved_courses.items[0],
            "CE1101"
        ) == 0 &&
        strcmp(
            history.approved_courses.items[1],
            "MA1102"
        ) == 0;

    student_history_free(&history);
    remove(TEMP_HISTORY_FILE);

    return report_result(
        "Espacios externos eliminados",
        passed
    );
}


/*
 * ============================================================
 * CRITERIO:
 * Los duplicados producen error.
 *
 * También comprueba que los espacios externos se eliminan
 * antes de comprobar duplicados.
 *
 * CE1101
 * " CE1101 "
 *
 * debe considerarse duplicado.
 * ============================================================
 */
static int test_duplicate_course(Catalog *catalog)
{
    if (!create_temp_history(
            "CE1101\n"
            "   CE1101   \n")) {
        return report_result(
            "Curso duplicado",
            0
        );
    }

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        TEMP_HISTORY_FILE,
        catalog,
        &history
    );

    /*
     * Ante un error, history_parser debe liberar
     * cualquier información cargada parcialmente.
     */
    int passed =
        status == ERROR_INVALID_FORMAT &&
        history.approved_courses.items == NULL &&
        history.approved_courses.count == 0 &&
        history.approved_courses.capacity == 0;

    student_history_free(&history);
    remove(TEMP_HISTORY_FILE);

    return report_result(
        "Cursos duplicados producen error",
        passed
    );
}


/*
 * ============================================================
 * CRITERIO:
 * Los códigos inexistentes producen error.
 * ============================================================
 */
static int test_nonexistent_course(Catalog *catalog)
{
    if (!create_temp_history(
            "CE1101\n"
            "XX9999\n")) {
        return report_result(
            "Código inexistente",
            0
        );
    }

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        TEMP_HISTORY_FILE,
        catalog,
        &history
    );

    /*
     * CE1101 fue agregado primero.
     *
     * Cuando XX9999 falla, el historial completo
     * debe ser limpiado.
     */
    int passed =
        status == ERROR_INVALID_FORMAT &&
        history.approved_courses.items == NULL &&
        history.approved_courses.count == 0;

    student_history_free(&history);
    remove(TEMP_HISTORY_FILE);

    return report_result(
        "Código inexistente produce error",
        passed
    );
}


/*
 * ============================================================
 * CRITERIO:
 * Un archivo vacío se maneja explícitamente.
 *
 * Decisión del proyecto:
 *
 * archivo vacío
 * -> estudiante sin cursos aprobados
 * -> SUCCESS
 * ============================================================
 */
static int test_empty_history(Catalog *catalog)
{
    if (!create_temp_history("")) {
        return report_result(
            "Archivo vacío",
            0
        );
    }

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        TEMP_HISTORY_FILE,
        catalog,
        &history
    );

    int passed =
        status == SUCCESS &&
        history.approved_courses.items == NULL &&
        history.approved_courses.count == 0 &&
        history.approved_courses.capacity == 0;

    student_history_free(&history);
    remove(TEMP_HISTORY_FILE);

    return report_result(
        "Archivo vacío manejado explícitamente",
        passed
    );
}


/*
 * ============================================================
 * Caso adicional:
 *
 * Un archivo que solo contiene espacios o líneas vacías
 * también representa un historial sin cursos aprobados.
 * ============================================================
 */
static int test_whitespace_only_history(Catalog *catalog)
{
    if (!create_temp_history(
            "\n"
            "   \n"
            "\t\n"
            "\n")) {
        return report_result(
            "Archivo solo whitespace",
            0
        );
    }

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        TEMP_HISTORY_FILE,
        catalog,
        &history
    );

    int passed =
        status == SUCCESS &&
        history.approved_courses.count == 0;

    student_history_free(&history);
    remove(TEMP_HISTORY_FILE);

    return report_result(
        "Archivo solo con espacios",
        passed
    );
}


/*
 * ============================================================
 * REGLA DE CONSISTENCIA:
 *
 * El programa NO corrige mayúsculas/minúsculas.
 *
 * Si el catálogo contiene:
 *
 * CE1101
 *
 * entonces:
 *
 * ce1101
 *
 * no se convierte automáticamente a CE1101.
 * Debe producir inconsistencia.
 * ============================================================
 */
static int test_code_not_corrected(Catalog *catalog)
{
    if (!create_temp_history("ce1101\n")) {
        return report_result(
            "Código no normalizado",
            0
        );
    }

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        TEMP_HISTORY_FILE,
        catalog,
        &history
    );

    int passed =
        status == ERROR_INVALID_FORMAT &&
        history.approved_courses.count == 0;

    student_history_free(&history);
    remove(TEMP_HISTORY_FILE);

    return report_result(
        "Código inconsistente no se corrige",
        passed
    );
}


/*
 * ============================================================
 * CRITERIO:
 * Toda memoria reservada puede liberarse.
 *
 * Primero se carga información válida.
 *
 * Después se llama:
 *
 * student_history_free()
 *
 * y verificamos que la estructura vuelva a su estado inicial.
 * ============================================================
 */
static int test_memory_release(Catalog *catalog)
{
    if (!create_temp_history(
            "CE1101\n"
            "MA1102\n")) {
        return report_result(
            "Liberación de memoria",
            0
        );
    }

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        TEMP_HISTORY_FILE,
        catalog,
        &history
    );

    if (status != SUCCESS) {
        student_history_free(&history);
        remove(TEMP_HISTORY_FILE);

        return report_result(
            "Liberación de memoria",
            0
        );
    }

    /*
     * Antes del free debe existir memoria reservada.
     */
    int had_data =
        history.approved_courses.items != NULL &&
        history.approved_courses.count == 2;

    student_history_free(&history);

    /*
     * Después del free:
     *
     * items = NULL
     * count = 0
     * capacity = 0
     */
    int was_released =
        history.approved_courses.items == NULL &&
        history.approved_courses.count == 0 &&
        history.approved_courses.capacity == 0;

    remove(TEMP_HISTORY_FILE);

    return report_result(
        "Memoria del historial puede liberarse",
        had_data && was_released
    );
}


/*
 * ============================================================
 * Caso adicional:
 * Archivo inexistente.
 * ============================================================
 */
static int test_missing_file(Catalog *catalog)
{
    remove("historial_que_no_existe.txt");

    StudentHistory history;
    student_history_init(&history);

    Status status = parse_student_history(
        "historial_que_no_existe.txt",
        catalog,
        &history
    );

    int passed =
        status == ERROR_FILE_NOT_FOUND &&
        history.approved_courses.count == 0;

    student_history_free(&history);

    return report_result(
        "Archivo inexistente",
        passed
    );
}


int main(void)
{
    int fails = 0;

    /*
     * ========================================================
     * CATÁLOGO DE PRUEBA
     * ========================================================
     *
     * No usamos catalog_parser aquí.
     *
     * Construimos directamente un Catalog válido para aislar
     * las pruebas de KAN-9.
     */

    Catalog catalog;
    catalog_init(&catalog);

    Course *course1 = catalog_add_course(
        &catalog,
        "CE1101",
        "Introduccion",
        4,
        1
    );

    Course *course2 = catalog_add_course(
        &catalog,
        "MA1102",
        "Matematica",
        4,
        1
    );

    Course *course3 = catalog_add_course(
        &catalog,
        "FI1101",
        "Fisica",
        3,
        1
    );

    /*
     * Si no se pudo construir el catálogo base,
     * no tiene sentido continuar las pruebas.
     */
    if (
        course1 == NULL ||
        course2 == NULL ||
        course3 == NULL
    ) {
        printf(
            "ERROR: No se pudo construir el catálogo de prueba.\n"
        );

        catalog_free(&catalog);

        return 1;
    }


    printf("\n=== TESTS KAN-9: HISTORY PARSER ===\n\n");

    fails += test_valid_history(&catalog);

    fails += test_external_whitespace(&catalog);

    fails += test_duplicate_course(&catalog);

    fails += test_nonexistent_course(&catalog);

    fails += test_empty_history(&catalog);

    fails += test_whitespace_only_history(&catalog);

    fails += test_code_not_corrected(&catalog);

    fails += test_memory_release(&catalog);

    fails += test_missing_file(&catalog);


    catalog_free(&catalog);

    printf("\nFallos totales: %d\n", fails);

    return fails ? 1 : 0;
}