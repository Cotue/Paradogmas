#include "models/student_history.h"

void student_history_init(StudentHistory *history)
{
    if (history == NULL) {
        return;
    }

    code_list_init(&history->approved_courses);
}

void student_history_free(StudentHistory *history)
{
    if (history == NULL) {
        return;
    }

    code_list_free(&history->approved_courses);

    student_history_init(history);
}