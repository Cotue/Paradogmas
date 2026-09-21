#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "constants.h"
#include "models.h"
#include "models/day.h"

Status validate_column_count(int count);
Status validate_course_identity(const char *code, const char *name);
Status validate_group_format(const char *group_str);
Status validate_day_format(const char *day_str);
Status validate_time_format(const char *start_time_str, const char *end_time_str);
Status validate_semester_format(const char *sem_str);
Status validate_credits_format(const char *cred_str);
Day parse_day_string(const char *day_str);

#endif