/*
 * ============================================================
 * TESTS KAN-12 - Detección de conflictos de horario
 * ============================================================
 *
 * Este test prueba conflict_detector.c de forma aislada.
 *
 * NO crea archivos TSV temporales.
 * NO usa datos inventados en formato TSV.
 *
 * Los Catalog/Course/Group/Schedule se construyen directamente
 * en memoria para controlar exactamente los casos límite.
 *
 * ============================================================
 * ARCHIVOS NECESARIOS PARA COMPILAR:
 *
 * tests/test_conflict_detector.c
 * src/conflict_detector.c
 * src/models/catalog.c
 * src/models/code_list.c
 * src/models/course.c
 * src/models/group.c
 * src/models/schedule.c
 *
 * ------------------------------------------------------------
 * COMPILAR EN POWERSHELL DESDE LA RAÍZ:
 *
 * gcc tests/test_conflict_detector.c `
 * src/conflict_detector.c `
 * src/models/catalog.c `
 * src/models/code_list.c `
 * src/models/course.c `
 * src/models/group.c `
 * src/models/schedule.c `
 * -Iinclude `
 * -Wall -Wextra -Wpedantic `
 * -std=c11 `
 * -o test_conflict_detector.exe
 *
 * ------------------------------------------------------------
 * EJECUTAR:
 *
 * .\test_conflict_detector.exe
 *
 * Resultado esperado:
 *
 * Fallos totales: 0
 * ============================================================
 */

#include <stdio.h>
#include <string.h>

#include "conflict_detector.h"


static int report_result(const char *name, int passed)
{
    printf(
        "%-55s : %s\n",
        name,
        passed ? "PASS" : "FAIL"
    );

    return passed ? 0 : 1;
}


/*
 * Crea un curso dentro del catálogo.
 */
static Course *create_course(
    Catalog *catalog,
    const char *code
)
{
    return catalog_add_course(
        catalog,
        code,
        code,
        4,
        1
    );
}


/*
 * Crea un grupo dentro de un curso.
 */
static Group *create_group(
    Course *course,
    int number
)
{
    return course_add_group(course, number);
}


/*
 * Agrega un horario a un grupo.
 */
static int add_schedule(
    Group *group,
    Day day,
    int start,
    int end
)
{
    return group_add_schedule(
        group,
        day,
        start,
        end
    ) != NULL;
}

/*
 * ============================================================
 * MISMO DÍA, PERO HORARIOS SEPARADOS
 *
 * CE1101:
 * 07:30 - 09:20
 *
 * MA1102:
 * 11:10 - 13:00
 *
 * Resultado: no existe choque.
 * ============================================================
 */
static int test_separated_schedules_same_day(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 1);

    add_schedule(group_a, MONDAY, 450, 560);
    add_schedule(group_b, MONDAY, 670, 780);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        !course_a->has_conflict &&
        !course_b->has_conflict &&
        !group_a->has_conflict &&
        !group_b->has_conflict &&
        group_a->conflict_count == 0 &&
        group_b->conflict_count == 0;

    catalog_free(&catalog);

    return report_result(
        "Horarios separados el mismo dia no producen choque",
        passed
    );
}
/*
 * ============================================================
 * UN GRUPO CON CONFLICTOS CONTRA DOS CURSOS
 *
 * CE1101:
 * 07:30 - 11:10
 *
 * MA1102:
 * 07:30 - 09:20
 *
 * FI1101:
 * 09:20 - 11:10
 *
 * CE1101 choca con ambos.
 * MA1102 y FI1101 son consecutivos y no chocan.
 *
 * Esto comprueba que Group.conflicts puede almacenar
 * múltiples conflictos.
 * ============================================================
 */
static int test_multiple_conflicts_for_one_group(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");
    Course *course_c = create_course(&catalog, "FI1101");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 1);
    Group *group_c = create_group(course_c, 1);

    /*
     * CE1101: 07:30 - 11:10
     */
    add_schedule(group_a, MONDAY, 450, 670);

    /*
     * MA1102: 07:30 - 09:20
     */
    add_schedule(group_b, MONDAY, 450, 560);

    /*
     * FI1101: 09:20 - 11:10
     */
    add_schedule(group_c, MONDAY, 560, 670);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&

        /* CE1101 tiene dos conflictos */
        course_a->has_conflict &&
        group_a->has_conflict &&
        group_a->conflict_count == 2 &&

        /* MA1102 solamente choca con CE1101 */
        course_b->has_conflict &&
        group_b->has_conflict &&
        group_b->conflict_count == 1 &&

        /* FI1101 solamente choca con CE1101 */
        course_c->has_conflict &&
        group_c->has_conflict &&
        group_c->conflict_count == 1 &&

        /* Primer conflicto de CE1101 */
        strcmp(
            group_a->conflicts[0].course_code,
            "MA1102"
        ) == 0 &&

        group_a->conflicts[0].group_number == 1 &&

        /* Segundo conflicto de CE1101 */
        strcmp(
            group_a->conflicts[1].course_code,
            "FI1101"
        ) == 0 &&

        group_a->conflicts[1].group_number == 1 &&

        /* Simetría MA1102 -> CE1101 */
        strcmp(
            group_b->conflicts[0].course_code,
            "CE1101"
        ) == 0 &&

        /* Simetría FI1101 -> CE1101 */
        strcmp(
            group_c->conflicts[0].course_code,
            "CE1101"
        ) == 0;

    catalog_free(&catalog);

    return report_result(
        "Un grupo puede almacenar conflictos con dos cursos",
        passed
    );
}
/*
 * ============================================================
 * MISMO DÍA + SUPERPOSICIÓN PARCIAL
 *
 * CE1101:
 * 07:30 - 09:20
 *
 * MA1102:
 * 08:30 - 10:20
 *
 * Resultado: choque.
 * ============================================================
 */
static int test_partial_overlap(void)
{
    printf("DEBUG 1: entrando al primer test\n");

    Catalog catalog;
    catalog_init(&catalog);

    printf("DEBUG 2: catalog creado\n");

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    printf("DEBUG 3: cursos creados\n");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 1);

    printf("DEBUG 4: grupos creados\n");

    add_schedule(group_a, MONDAY, 450, 560);
    add_schedule(group_b, MONDAY, 510, 620);

    printf("DEBUG 5: horarios creados\n");

    Status status = detect_catalog_conflicts(&catalog);

    printf("DEBUG 6: detector terminado\n");

    int passed =
        status == SUCCESS &&
        group_a->has_conflict &&
        group_b->has_conflict &&
        course_a->has_conflict &&
        course_b->has_conflict &&
        group_a->conflict_count == 1 &&
        group_b->conflict_count == 1;

    printf("DEBUG 7: antes del free\n");

    catalog_free(&catalog);

    printf("DEBUG 8: catalog liberado\n");

    return report_result(
        "Mismo día con superposición parcial",
        passed
    );
}


/*
 * ============================================================
 * HORARIOS IDÉNTICOS
 * ============================================================
 */
static int test_identical_schedules(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 2);

    add_schedule(group_a, TUESDAY, 450, 560);
    add_schedule(group_b, TUESDAY, 450, 560);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        group_a->has_conflict &&
        group_b->has_conflict;

    catalog_free(&catalog);

    return report_result(
        "Horarios idénticos producen choque",
        passed
    );
}


/*
 * ============================================================
 * UN HORARIO CONTENIDO DENTRO DE OTRO
 * ============================================================
 */
static int test_contained_schedule(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 1);

    add_schedule(group_a, WEDNESDAY, 450, 680);
    add_schedule(group_b, WEDNESDAY, 510, 560);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        group_a->has_conflict &&
        group_b->has_conflict;

    catalog_free(&catalog);

    return report_result(
        "Horario contenido dentro de otro",
        passed
    );
}


/*
 * ============================================================
 * HORARIOS CONSECUTIVOS
 *
 * 07:30 - 09:20
 * 09:20 - 11:10
 *
 * No existe superposición.
 * ============================================================
 */
static int test_consecutive_schedules(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 1);

    add_schedule(group_a, MONDAY, 450, 560);
    add_schedule(group_b, MONDAY, 560, 670);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        !group_a->has_conflict &&
        !group_b->has_conflict &&
        !course_a->has_conflict &&
        !course_b->has_conflict &&
        group_a->conflict_count == 0 &&
        group_b->conflict_count == 0;

    catalog_free(&catalog);

    return report_result(
        "Horarios consecutivos no producen choque",
        passed
    );
}


/*
 * ============================================================
 * MISMAS HORAS PERO DÍAS DIFERENTES
 * ============================================================
 */
static int test_different_days(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 1);

    add_schedule(group_a, MONDAY, 450, 560);
    add_schedule(group_b, TUESDAY, 450, 560);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        !group_a->has_conflict &&
        !group_b->has_conflict;

    catalog_free(&catalog);

    return report_result(
        "Mismas horas en días diferentes no chocan",
        passed
    );
}


/*
 * ============================================================
 * DOS GRUPOS DEL MISMO CURSO
 *
 * Aunque tengan exactamente el mismo horario,
 * NO deben compararse entre ellos.
 * ============================================================
 */
static int test_same_course_groups(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course = create_course(&catalog, "CE1101");

    Group *group_1 = create_group(course, 1);
    Group *group_2 = create_group(course, 2);

    add_schedule(group_1, MONDAY, 450, 560);
    add_schedule(group_2, MONDAY, 450, 560);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        !course->has_conflict &&
        !group_1->has_conflict &&
        !group_2->has_conflict &&
        group_1->conflict_count == 0 &&
        group_2->conflict_count == 0;

    catalog_free(&catalog);

    return report_result(
        "Grupos del mismo curso no se comparan",
        passed
    );
}


/*
 * ============================================================
 * GRUPO CON MÚLTIPLES BLOQUES
 *
 * Solo uno de los bloques necesita chocar para considerar
 * que los grupos presentan conflicto.
 * ============================================================
 */
static int test_multiple_schedule_blocks(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 1);

    /*
     * CE1101:
     * LUN 07:30-09:20
     * JUE 07:30-09:20
     */
    add_schedule(group_a, MONDAY, 450, 560);
    add_schedule(group_a, THURSDAY, 450, 560);

    /*
     * MA1102:
     * MAR 07:30-09:20
     * JUE 08:30-10:20
     */
    add_schedule(group_b, TUESDAY, 450, 560);
    add_schedule(group_b, THURSDAY, 510, 620);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        group_a->has_conflict &&
        group_b->has_conflict &&
        group_a->conflict_count == 1 &&
        group_b->conflict_count == 1;

    catalog_free(&catalog);

    return report_result(
        "Grupo con múltiples bloques detecta un choque",
        passed
    );
}


/*
 * ============================================================
 * SOLO UN GRUPO DE UN CURSO TIENE CHOQUE
 * ============================================================
 */
static int test_only_one_group_conflicts(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a1 = create_group(course_a, 1);
    Group *group_a2 = create_group(course_a, 2);

    Group *group_b = create_group(course_b, 1);

    add_schedule(group_a1, MONDAY, 450, 560);
    add_schedule(group_a2, TUESDAY, 450, 560);

    add_schedule(group_b, MONDAY, 510, 620);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        course_a->has_conflict &&
        course_b->has_conflict &&
        group_a1->has_conflict &&
        !group_a2->has_conflict &&
        group_b->has_conflict;

    catalog_free(&catalog);

    return report_result(
        "Solo el grupo que realmente choca queda marcado",
        passed
    );
}


/*
 * ============================================================
 * CONFLICTO ALMACENADO EN AMBOS SENTIDOS
 * ============================================================
 */
static int test_symmetric_conflict(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 2);

    add_schedule(group_a, FRIDAY, 450, 560);
    add_schedule(group_b, FRIDAY, 510, 620);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        group_a->conflict_count == 1 &&
        group_b->conflict_count == 1 &&

        strcmp(
            group_a->conflicts[0].course_code,
            "MA1102"
        ) == 0 &&

        group_a->conflicts[0].group_number == 2 &&

        strcmp(
            group_b->conflicts[0].course_code,
            "CE1101"
        ) == 0 &&

        group_b->conflicts[0].group_number == 1;

    catalog_free(&catalog);

    return report_result(
        "Conflicto se almacena correctamente en ambos sentidos",
        passed
    );
}


/*
 * ============================================================
 * EJECUTAR EL DETECTOR DOS VECES
 *
 * No deben duplicarse los conflictos.
 * ============================================================
 */
static int test_repeated_detection(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course_a = create_course(&catalog, "CE1101");
    Course *course_b = create_course(&catalog, "MA1102");

    Group *group_a = create_group(course_a, 1);
    Group *group_b = create_group(course_b, 1);

    add_schedule(group_a, MONDAY, 450, 560);
    add_schedule(group_b, MONDAY, 510, 620);

    Status first =
        detect_catalog_conflicts(&catalog);

    Status second =
        detect_catalog_conflicts(&catalog);

    int passed =
        first == SUCCESS &&
        second == SUCCESS &&
        group_a->conflict_count == 1 &&
        group_b->conflict_count == 1;

    catalog_free(&catalog);

    return report_result(
        "Ejecutar detector nuevamente no duplica conflictos",
        passed
    );
}


/*
 * ============================================================
 * CATÁLOGO CON UN SOLO CURSO
 * ============================================================
 */
static int test_single_course(void)
{
    Catalog catalog;
    catalog_init(&catalog);

    Course *course = create_course(&catalog, "CE1101");
    Group *group = create_group(course, 1);

    add_schedule(group, MONDAY, 450, 560);

    Status status = detect_catalog_conflicts(&catalog);

    int passed =
        status == SUCCESS &&
        !course->has_conflict &&
        !group->has_conflict &&
        group->conflict_count == 0;

    catalog_free(&catalog);

    return report_result(
        "Catálogo con un solo curso no genera conflictos",
        passed
    );
}


/*
 * ============================================================
 * ARGUMENTO INVÁLIDO
 * ============================================================
 */
static int test_null_catalog(void)
{
    Status status = detect_catalog_conflicts(NULL);

    return report_result(
        "Catalog NULL retorna ERROR_ARGS",
        status == ERROR_ARGS
    );
}


int main(void)
{
    int fails = 0;

    printf(
        "\n=== TESTS KAN-12: CONFLICT DETECTOR ===\n\n"
    );

    fails += test_partial_overlap();
    fails += test_identical_schedules();
    fails += test_contained_schedule();
    fails += test_consecutive_schedules();
    fails += test_different_days();
    fails += test_same_course_groups();
    fails += test_multiple_schedule_blocks();
    fails += test_only_one_group_conflicts();
    fails += test_symmetric_conflict();
    fails += test_repeated_detection();
    fails += test_single_course();
    fails += test_null_catalog();
    fails += test_multiple_conflicts_for_one_group();
    fails += test_separated_schedules_same_day();

    printf(
        "\nFallos totales: %d\n",
        fails
    );

    return fails ? 1 : 0;
}