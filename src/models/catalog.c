#include "models/catalog.h"

#include <stdlib.h>

void catalog_init(Catalog *catalog)
{
    if (catalog == NULL) {
        return;
    }

    catalog->career_code = NULL;
    catalog->career_name = NULL;

    catalog->courses = NULL;
    catalog->course_count = 0;
    catalog->course_capacity = 0;
}

void catalog_free(Catalog *catalog)
{
    if (catalog == NULL) {
        return;
    }

    free(catalog->career_code);
    free(catalog->career_name);

    for (size_t i = 0; i < catalog->course_count; i++) {
        course_free(&catalog->courses[i]);
    }

    free(catalog->courses);

    catalog_init(catalog);
}