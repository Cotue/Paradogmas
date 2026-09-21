#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "models.h"

static char *copy_string(const char *source)
{
    size_t length = strlen(source) + 1;

    char *copy = malloc(length);

    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, source, length);

    return copy;
}

int main(void)
{
    Catalog catalog;

    catalog_init(&catalog);

    printf("1. Catalog inicializado\n");

    catalog.career_code = copy_string("IC");
    catalog.career_name = copy_string("Ingenieria en Computadores");

    assert(catalog.career_code != NULL);
    assert(catalog.career_name != NULL);

    catalog.course_capacity = 1;
    catalog.courses = malloc(sizeof(Course) * catalog.course_capacity);

    assert(catalog.courses != NULL);

    catalog.course_count = 1;

    Course *course = &catalog.courses[0];

    course_init(course);

    course->code = copy_string("CE2201");
    course->name = copy_string("Estructuras de Datos");
    course->credits = 4;
    course->semester = 2;

    assert(course->code != NULL);
    assert(course->name != NULL);

    printf("2. Course creado: %s\n", course->code);

    course->requirements.capacity = 1;
    course->requirements.items =
        malloc(sizeof(char *) * course->requirements.capacity);

    assert(course->requirements.items != NULL);

    course->requirements.items[0] = copy_string("CE1101");

    assert(course->requirements.items[0] != NULL);

    course->requirements.count = 1;

    printf(
        "3. Requirement agregado: %s\n",
        course->requirements.items[0]
    );

    course->group_capacity = 1;
    course->groups = malloc(sizeof(Group) * course->group_capacity);

    assert(course->groups != NULL);

    course->group_count = 1;

    Group *group = &course->groups[0];

    group_init(group);

    group->number = 1;

    group->schedule_capacity = 2;
    group->schedules =
        malloc(sizeof(Schedule) * group->schedule_capacity);

    assert(group->schedules != NULL);

    group->schedule_count = 2;

    schedule_init(&group->schedules[0]);
    schedule_init(&group->schedules[1]);

    group->schedules[0].day = MONDAY;
    group->schedules[0].start_minutes = 450;
    group->schedules[0].end_minutes = 560;

    group->schedules[1].day = THURSDAY;
    group->schedules[1].start_minutes = 450;
    group->schedules[1].end_minutes = 560;

    printf("4. Group %d creado con %zu horarios\n",
        group->number,
        group->schedule_count
    );

    printf("\nEstado antes de liberar:\n");

    printf("Carrera: %s\n", catalog.career_name);
    printf("Curso: %s\n", course->code);
    printf(
        "Requisito: %s\n",
        course->requirements.items[0]
    );

    printf(
        "Grupo: %d, horarios: %zu\n",
        group->number,
        group->schedule_count
    );

    printf("\n5. Ejecutando catalog_free()\n\n");

    catalog_free(&catalog);

    printf("6. Catalog liberado\n");

    assert(catalog.career_code == NULL);
    assert(catalog.career_name == NULL);
    assert(catalog.courses == NULL);
    assert(catalog.course_count == 0);
    assert(catalog.course_capacity == 0);

    printf("7. Estado final valido\n");

    return 0;
}