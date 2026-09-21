#include "models/course.h"

#include <stdlib.h>

void course_init(Course *course)
{
    if (course == NULL) {
        return;
    }

    course->code = NULL;
    course->name = NULL;

    course->credits = 0;
    course->semester = 0;

    code_list_init(&course->requirements);
    code_list_init(&course->corequisites);
    code_list_init(&course->pending_corequisites);

    course->groups = NULL;
    course->group_count = 0;
    course->group_capacity = 0;

    course->has_conflict = false;
    course->can_enroll = false;
}

void course_free(Course *course)
{
    if (course == NULL) {
        return;
    }

    free(course->code);
    free(course->name);

    code_list_free(&course->requirements);
    code_list_free(&course->corequisites);
    code_list_free(&course->pending_corequisites);

    for (size_t i = 0; i < course->group_count; i++) {
        group_free(&course->groups[i]);
    }

    free(course->groups);

    course_init(course);
}

#include "constants.h"

Group* find_group_by_number(Course *course, int number) {
    for (size_t i = 0; i < course->group_count; i++) {
        if (course->groups[i].number == number) {
            return &course->groups[i];
        }
    }
    return NULL;
}

Group* course_add_group(Course *course, int number) {
    if (course->group_count == course->group_capacity) {
        size_t new_cap = (course->group_capacity == 0) ? INITIAL_CAPACITY : course->group_capacity * 2;
        Group *temp = realloc(course->groups, new_cap * sizeof(Group));
        if (!temp) return NULL;
        course->groups = temp;
        course->group_capacity = new_cap;
    }
    
    Group *new_group = &course->groups[course->group_count];
    group_init(new_group);
    new_group->number = number;
    
    course->group_count++;
    return new_group;
}