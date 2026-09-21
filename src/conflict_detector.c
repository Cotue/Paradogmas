#include "conflict_detector.h"

static int schedules_conflict(
    const Schedule *schedule_a,
    const Schedule *schedule_b
)
{
    if (schedule_a == NULL || schedule_b == NULL) {
        return 0;
    }

    if (schedule_a->day != schedule_b->day) {
        return 0;
    }

    return (
        schedule_a->start_minutes < schedule_b->end_minutes &&
        schedule_b->start_minutes < schedule_a->end_minutes
    );
}
static int groups_conflict(
    const Group *group_a,
    const Group *group_b
)
{
    if (group_a == NULL || group_b == NULL) {
        return 0;
    }

    for (size_t i = 0; i < group_a->schedule_count; i++) {
        for (size_t j = 0; j < group_b->schedule_count; j++) {

            if (schedules_conflict(
                    &group_a->schedules[i],
                    &group_b->schedules[j]
                )) {
                return 1;
            }
        }
    }

    return 0;
}
static void clear_catalog_conflicts(Catalog *catalog)
{
    if (catalog == NULL) {
        return;
    }

    for (size_t i = 0; i < catalog->course_count; i++) {
        Course *course = &catalog->courses[i];

        course->has_conflict = false;

        for (size_t j = 0; j < course->group_count; j++) {
            group_clear_conflicts(&course->groups[j]);
        }
    }
}

Status detect_catalog_conflicts(Catalog *catalog)
{
    
    if (catalog == NULL) {
        return ERROR_ARGS;
    }

    /*
     * Los conflictos son información calculada.
     * Se limpian antes de volver a realizar el análisis.
     */
    clear_catalog_conflicts(catalog);

    /*
     * Se comparan únicamente cursos diferentes.
     *
     * j comienza en i + 1 para:
     *
     * 1. evitar comparar un curso consigo mismo;
     * 2. evitar comparar dos veces el mismo par.
     */
    for (size_t i = 0; i < catalog->course_count; i++) {
        Course *course_a = &catalog->courses[i];

        for (size_t j = i + 1; j < catalog->course_count; j++) {
            Course *course_b = &catalog->courses[j];

            /*
             * Ahora se comparan todos los grupos
             * disponibles de ambos cursos.
             */
            for (size_t group_a_index = 0;
                 group_a_index < course_a->group_count;
                 group_a_index++) {

                Group *group_a =
                    &course_a->groups[group_a_index];

                for (size_t group_b_index = 0;
                     group_b_index < course_b->group_count;
                     group_b_index++) {

                    Group *group_b =
                        &course_b->groups[group_b_index];

                    /*
                     * Si ninguno de sus bloques horarios
                     * se superpone, no hay nada que registrar.
                     */
                    if (!groups_conflict(group_a, group_b)) {
                        continue;
                    }

                    /*
                     * El conflicto se almacena en ambos sentidos.
                     *
                     * A -> B
                     */
                    if (!group_add_conflict(
                            group_a,
                            course_b->code,
                            group_b->number
                        )) {

                        clear_catalog_conflicts(catalog);
                        return ERROR_MEMORY;
                    }

                    /*
                     * B -> A
                     */
                    if (!group_add_conflict(
                            group_b,
                            course_a->code,
                            group_a->number
                        )) {

                        clear_catalog_conflicts(catalog);
                        return ERROR_MEMORY;
                    }

                    /*
                     * group_add_conflict() ya establece
                     * Group.has_conflict = true.
                     *
                     * Aquí marcamos también los cursos.
                     */
                    course_a->has_conflict = true;
                    course_b->has_conflict = true;
                }
            }
        }
    }

    return SUCCESS;
}