#ifndef GROUP_H
#define GROUP_H
#include <stddef.h>
#include "schedule.h"
#include <stdbool.h>
typedef struct {
    int number;

    Schedule *schedules; //lista de horarios para cuando un grupo tenga dos clases por semana con diferentes días
    size_t schedule_count;
    size_t schedule_capacity; //lista dinamica

    bool has_conflict; //conflicto de grupo
} Group;
void group_init(Group *group);
void group_free(Group *group);
#endif
