#include "models/group.h"
#include <string.h>
#include <stdlib.h>
#include "constants.h"

void group_init(Group *group)
{
    if (group == NULL) {
        return;
    }

    group->number = 0;

    group->schedules = NULL;
    group->schedule_count = 0;
    group->schedule_capacity = 0;

    group->has_conflict = false;

    group->conflicts = NULL;
    group->conflict_count = 0;
    group->conflict_capacity = 0;
}

void group_free(Group *group)
{
    if (group == NULL) {
        return;
    }

    for (size_t i = 0; i < group->schedule_count; i++) {
        schedule_free(&group->schedules[i]);
    }

    free(group->schedules);

    group_clear_conflicts(group);

    group_init(group);
}

#include "constants.h"

Schedule* group_add_schedule(Group *group, Day day, int start_minutes, int end_minutes) {
    if (group == NULL) {
        return NULL;
    }
    if (group->schedule_count == group->schedule_capacity) {
        size_t new_cap = (group->schedule_capacity == 0) ? INITIAL_CAPACITY : group->schedule_capacity * 2;
        Schedule *temp = realloc(group->schedules, new_cap * sizeof(Schedule));
        if (!temp) return NULL;
        group->schedules = temp;
        group->schedule_capacity = new_cap;
    }
    
    Schedule *new_sched = &group->schedules[group->schedule_count];
    schedule_init(new_sched);
    new_sched->day = day;
    new_sched->start_minutes = start_minutes;
    new_sched->end_minutes = end_minutes;
    
    group->schedule_count++;
    return new_sched;
}

int group_add_conflict(
    Group *group,
    const char *course_code,
    int group_number
)
{
    if (group == NULL || course_code == NULL) {
        return 0;
    }

    if (group->conflict_count == group->conflict_capacity) {
        size_t new_cap =
            (group->conflict_capacity == 0)
                ? INITIAL_CAPACITY
                : group->conflict_capacity * 2;

        GroupConflict *temp =
            realloc(
                group->conflicts,
                new_cap * sizeof(GroupConflict)
            );

        if (temp == NULL) {
            return 0;
        }

        group->conflicts = temp;
        group->conflict_capacity = new_cap;
    }

    char *code_copy =
        malloc(strlen(course_code) + 1);

    if (code_copy == NULL) {
        return 0;
    }

    strcpy(code_copy, course_code);

    GroupConflict *new_conflict =
        &group->conflicts[group->conflict_count];

    new_conflict->course_code = code_copy;
    new_conflict->group_number = group_number;

    group->conflict_count++;
    group->has_conflict = true;

    return 1;
}
void group_clear_conflicts(Group *group)
{
    if (group == NULL) {
        return;
    }

    for (size_t i = 0; i < group->conflict_count; i++) {
        free(group->conflicts[i].course_code);
    }

    free(group->conflicts);

    group->conflicts = NULL;
    group->conflict_count = 0;
    group->conflict_capacity = 0;
    group->has_conflict = false;
}