#ifndef COURSE_H
#define COURSE_H

#include <stdbool.h>
#include <stddef.h>

#include "code_list.h"
#include "group.h"

typedef struct {
    char *code;
    char *name;

    int credits;
    int semester;

    CodeList requirements;
    CodeList corequisites;
    CodeList pending_corequisites;

    Group *groups;
    size_t group_count;
    size_t group_capacity;

    bool has_conflict; // hay conflictos?

    bool can_enroll; // puede matricular?
} Course;

void course_init(Course *course);
void course_free(Course *course);

#endif