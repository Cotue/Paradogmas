#include "models/group.h"

#include <stdlib.h>

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

    group_init(group);
}

#include "constants.h"

Schedule* group_add_schedule(Group *group, Day day, int start_minutes, int end_minutes) {
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