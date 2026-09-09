#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "day.h"

typedef struct {
    Day day;
    int start_minutes; //hora inicial convertida a minutos H:MIN = H * 60 + MIN / 7:30 = 7 * 60 + 30 = 450
    int end_minutes; //hora final convertida a minutos H:MIN = H * 60 + MIN / 9:20 = 9 * 60 + 20 = 560 para detectar
    //esta implementacion es para detectar choques de horas mas facilmente
} Schedule;

void schedule_init(Schedule *schedule);
void schedule_free(Schedule *schedule);
#endif