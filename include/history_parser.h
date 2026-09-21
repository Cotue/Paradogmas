#ifndef HISTORY_PARSER_H
#define HISTORY_PARSER_H

#include "constants.h"
#include "models/catalog.h"
#include "models/student_history.h"

Status parse_student_history(
    const char *file_path,
    Catalog *catalog,
    StudentHistory *history
);

#endif