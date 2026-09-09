#ifndef CATALOG_H
#define CATALOG_H

#include <stddef.h>
#include "course.h"

typedef struct {
    char *career_code;
    char *career_name;

    Course *courses;
    size_t course_count;
    size_t course_capacity;
} Catalog;

void catalog_init(Catalog *catalog);
void catalog_free(Catalog *catalog);

#endif