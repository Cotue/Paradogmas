#ifndef STUDENT_HISTORY_H
#define STUDENT_HISTORY_H

#include "code_list.h"

typedef struct {
    CodeList approved_courses;
} StudentHistory;

void student_history_init(StudentHistory *history);
void student_history_free(StudentHistory *history);

#endif