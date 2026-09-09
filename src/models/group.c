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