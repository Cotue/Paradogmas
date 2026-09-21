#include "models/schedule.h"

void schedule_init(Schedule *schedule)
{
    if (schedule == NULL) {
        return;
    }

    schedule->day = DAY_INVALID;
    schedule->start_minutes = -1;
    schedule->end_minutes = -1;
}

void schedule_free(Schedule *schedule)
{
    if (schedule == NULL) {
        return;
    }

    schedule_init(schedule);
}
//existe schedule_free() por si a futuro hay que usar memoria dinamica
//Si en el futuro Schedule necesita almacenar algo dinámico, los módulos que lo utilizan no tendrán que cambiar.