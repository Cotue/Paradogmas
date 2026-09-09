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