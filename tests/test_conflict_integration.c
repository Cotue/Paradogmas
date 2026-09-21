#include <stdio.h>
#include <string.h>

#include "catalog_parser.h"
#include "conflict_detector.h"


/*
 * Comprueba de forma independiente si dos horarios se superponen.
 *
 * Esta función pertenece solamente al test.
 */
static int schedules_overlap(
    const Schedule *schedule_a,
    const Schedule *schedule_b
)
{
    if (schedule_a->day != schedule_b->day) {
        return 0;
    }

    return (
        schedule_a->start_minutes < schedule_b->end_minutes &&
        schedule_b->start_minutes < schedule_a->end_minutes
    );
}


/*
 * Comprueba si existe al menos un choque entre
 * los horarios de dos grupos.
 */
static int groups_overlap(
    const Group *group_a,
    const Group *group_b
)
{
    for (size_t i = 0; i < group_a->schedule_count; i++) {
        for (size_t j = 0; j < group_b->schedule_count; j++) {

            if (schedules_overlap(
                    &group_a->schedules[i],
                    &group_b->schedules[j]
                )) {
                return 1;
            }
        }
    }

    return 0;
}


/*
 * Busca un curso por código.
 *
 * Lo implementamos aquí para trabajar con const
 * sin modificar el catálogo.
 */
static const Course *find_course(
    const Catalog *catalog,
    const char *code
)
{
    for (size_t i = 0; i < catalog->course_count; i++) {
        if (strcmp(catalog->courses[i].code, code) == 0) {
            return &catalog->courses[i];
        }
    }

    return NULL;
}


/*
 * Busca un grupo por número dentro de un curso.
 */
static const Group *find_group(
    const Course *course,
    int group_number
)
{
    for (size_t i = 0; i < course->group_count; i++) {
        if (course->groups[i].number == group_number) {
            return &course->groups[i];
        }
    }

    return NULL;
}


/*
 * Comprueba si un grupo contiene una referencia
 * específica dentro de su lista de conflictos.
 */
static int has_conflict_reference(
    const Group *group,
    const char *course_code,
    int group_number
)
{
    for (size_t i = 0; i < group->conflict_count; i++) {

        const GroupConflict *conflict =
            &group->conflicts[i];

        if (
            strcmp(conflict->course_code, course_code) == 0 &&
            conflict->group_number == group_number
        ) {
            return 1;
        }
    }

    return 0;
}


/*
 * Calcula cuántos grupos de otros cursos deberían
 * presentar conflicto contra este grupo.
 *
 * Esto permite comparar el resultado esperado
 * contra group->conflict_count.
 */
static size_t expected_conflict_count(
    const Catalog *catalog,
    const Course *source_course,
    const Group *source_group
)
{
    size_t count = 0;

    for (size_t i = 0; i < catalog->course_count; i++) {

        const Course *other_course =
            &catalog->courses[i];

        /*
         * Los grupos del mismo curso NO se comparan.
         */
        if (other_course == source_course) {
            continue;
        }

        for (size_t j = 0;
             j < other_course->group_count;
             j++) {

            const Group *other_group =
                &other_course->groups[j];

            if (groups_overlap(
                    source_group,
                    other_group
                )) {
                count++;
            }
        }
    }

    return count;
}


/*
 * Valida toda la información producida por KAN-12.
 *
 * Retorna 1 si todo está correcto.
 * Retorna 0 si encuentra una inconsistencia.
 */
static int validate_conflict_results(
    const Catalog *catalog
)
{
    for (size_t i = 0; i < catalog->course_count; i++) {

        const Course *course =
            &catalog->courses[i];

        int expected_course_conflict = 0;

        for (size_t j = 0;
             j < course->group_count;
             j++) {

            const Group *group =
                &course->groups[j];

            size_t expected_count =
                expected_conflict_count(
                    catalog,
                    course,
                    group
                );

            int expected_group_conflict =
                expected_count > 0;

            /*
             * El booleano del grupo debe coincidir
             * con la existencia real de conflictos.
             */
            if (
                group->has_conflict !=
                expected_group_conflict
            ) {
                printf(
                    "ERROR: %s grupo %d tiene "
                    "has_conflict incorrecto\n",
                    course->code,
                    group->number
                );

                return 0;
            }

            /*
             * La cantidad almacenada debe coincidir
             * exactamente con la cantidad esperada.
             */
            if (
                group->conflict_count !=
                expected_count
            ) {
                printf(
                    "ERROR: %s grupo %d esperaba "
                    "%zu conflictos pero almacena %zu\n",
                    course->code,
                    group->number,
                    expected_count,
                    group->conflict_count
                );

                return 0;
            }

            if (expected_group_conflict) {
                expected_course_conflict = 1;
            }

            /*
             * Validar cada referencia almacenada.
             */
            for (size_t k = 0;
                 k < group->conflict_count;
                 k++) {

                const GroupConflict *conflict =
                    &group->conflicts[k];

                /*
                 * El curso referenciado debe existir.
                 */
                const Course *target_course =
                    find_course(
                        catalog,
                        conflict->course_code
                    );

                if (target_course == NULL) {
                    printf(
                        "ERROR: %s grupo %d referencia "
                        "curso inexistente %s\n",
                        course->code,
                        group->number,
                        conflict->course_code
                    );

                    return 0;
                }

                /*
                 * Nunca debe existir conflicto
                 * contra el mismo curso.
                 */
                if (target_course == course) {
                    printf(
                        "ERROR: %s grupo %d presenta "
                        "conflicto contra su mismo curso\n",
                        course->code,
                        group->number
                    );

                    return 0;
                }

                /*
                 * El grupo referenciado debe existir.
                 */
                const Group *target_group =
                    find_group(
                        target_course,
                        conflict->group_number
                    );

                if (target_group == NULL) {
                    printf(
                        "ERROR: %s grupo %d referencia "
                        "%s grupo %d inexistente\n",
                        course->code,
                        group->number,
                        conflict->course_code,
                        conflict->group_number
                    );

                    return 0;
                }

                /*
                 * Los horarios realmente deben chocar.
                 */
                if (!groups_overlap(
                        group,
                        target_group
                    )) {

                    printf(
                        "ERROR: conflicto registrado "
                        "sin superposicion real: "
                        "%s G%d <-> %s G%d\n",
                        course->code,
                        group->number,
                        target_course->code,
                        target_group->number
                    );

                    return 0;
                }

                /*
                 * La relación debe ser simétrica.
                 *
                 * Si A dice que choca con B,
                 * B también debe decir que choca con A.
                 */
                if (!has_conflict_reference(
                        target_group,
                        course->code,
                        group->number
                    )) {

                    printf(
                        "ERROR: conflicto no simetrico: "
                        "%s G%d -> %s G%d\n",
                        course->code,
                        group->number,
                        target_course->code,
                        target_group->number
                    );

                    return 0;
                }
            }
        }

        /*
         * Course.has_conflict debe ser true
         * exactamente cuando alguno de sus grupos
         * tenga conflicto.
         */
        if (
            course->has_conflict !=
            expected_course_conflict
        ) {

            printf(
                "ERROR: Course.has_conflict incorrecto "
                "para %s\n",
                course->code
            );

            return 0;
        }
    }

    return 1;
}


/*
 * Cuenta información únicamente para mostrar
 * un resumen del dataset probado.
 */
static void print_catalog_summary(
    const Catalog *catalog
)
{
    size_t total_groups = 0;
    size_t total_schedules = 0;
    size_t total_conflict_references = 0;

    for (size_t i = 0; i < catalog->course_count; i++) {

        const Course *course =
            &catalog->courses[i];

        total_groups += course->group_count;

        for (size_t j = 0;
             j < course->group_count;
             j++) {

            const Group *group =
                &course->groups[j];

            total_schedules +=
                group->schedule_count;

            total_conflict_references +=
                group->conflict_count;
        }
    }

    printf(
        "  Cursos: %zu\n",
        catalog->course_count
    );

    printf(
        "  Grupos: %zu\n",
        total_groups
    );

    printf(
        "  Bloques horarios: %zu\n",
        total_schedules
    );

    /*
     * Cada conflicto se almacena dos veces:
     *
     * A -> B
     * B -> A
     *
     * Por eso dividimos entre dos para mostrar
     * pares reales de conflicto.
     */
    printf(
        "  Pares de conflicto: %zu\n",
        total_conflict_references / 2
    );
}


/*
 * Ejecuta la prueba completa para un dataset.
 */
static int test_dataset(
    const char *name,
    const char *file_path
)
{
    Catalog catalog;
    catalog_init(&catalog);

    printf("\nProbando: %s\n", name);
    printf("Archivo: %s\n", file_path);

    /*
     * PASO 1:
     * cargar el catálogo real mediante KAN-7.
     */
    Status parse_status =
        parse_catalog(file_path, &catalog);

    if (parse_status != SUCCESS) {
        printf(
            "  FAIL: parse_catalog retorno %d\n",
            parse_status
        );

        return 1;
    }

    /*
     * Un dataset real no debería producir
     * un catálogo vacío.
     */
    if (catalog.course_count == 0) {
        printf(
            "  FAIL: catalogo cargado sin cursos\n"
        );

        catalog_free(&catalog);

        return 1;
    }

    /*
     * PASO 2:
     * ejecutar KAN-12 sobre el catálogo cargado.
     */
    Status conflict_status =
        detect_catalog_conflicts(&catalog);

    if (conflict_status != SUCCESS) {
        printf(
            "  FAIL: detect_catalog_conflicts "
            "retorno %d\n",
            conflict_status
        );

        catalog_free(&catalog);

        return 1;
    }

    /*
     * PASO 3:
     * comprobar matemáticamente todos los
     * resultados obtenidos.
     */
    if (!validate_conflict_results(&catalog)) {
        printf(
            "  FAIL: resultados de conflictos "
            "inconsistentes\n"
        );

        catalog_free(&catalog);

        return 1;
    }

    /*
     * PASO 4:
     * ejecutar el detector nuevamente.
     *
     * Esto verifica también con datos reales
     * que limpiar + recalcular funciona.
     */
    conflict_status =
        detect_catalog_conflicts(&catalog);

    if (conflict_status != SUCCESS) {
        printf(
            "  FAIL: segunda deteccion retorno %d\n",
            conflict_status
        );

        catalog_free(&catalog);

        return 1;
    }

    if (!validate_conflict_results(&catalog)) {
        printf(
            "  FAIL: resultados inconsistentes "
            "despues de recalcular\n"
        );

        catalog_free(&catalog);

        return 1;
    }

    print_catalog_summary(&catalog);

    catalog_free(&catalog);

    printf("  Resultado: PASS\n");

    return 0;
}


int main(void)
{
    int fails = 0;

    printf(
        "\n=== TEST INTEGRACION KAN-12 ===\n"
    );

    fails += test_dataset(
        "Ingenieria en Computadores",
        "data/clean/computadores.tsv"
    );

    fails += test_dataset(
        "Ingenieria Electronica",
        "data/clean/electronica.tsv"
    );

    printf(
        "\nFallos totales: %d\n",
        fails
    );

    return fails ? 1 : 0;
}