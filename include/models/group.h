#ifndef GROUP_H
#define GROUP_H
#include <stddef.h>
#include "schedule.h"
#include <stdbool.h>
typedef struct {
    char *course_code;
    int group_number;
} GroupConflict;

typedef struct {
    int number;

    Schedule *schedules; //lista de horarios para cuando un grupo tenga dos clases por semana con diferentes días
    size_t schedule_count;
    size_t schedule_capacity; //lista dinamica

    bool has_conflict; //conflicto de grupo

    GroupConflict *conflicts;
    size_t conflict_count;
    size_t conflict_capacity;

} Group;
void group_init(Group *group);
void group_free(Group *group);

Schedule* group_add_schedule(Group *group, Day day, int start_minutes, int end_minutes);

int group_add_conflict(Group *group,const char *course_code, int group_number);
void group_clear_conflicts(Group *group);

#endif
