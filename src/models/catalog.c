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

#include "constants.h"
#include <string.h>

Course* find_course_by_code(Catalog *catalog, const char *code) {
    for (size_t i = 0; i < catalog->course_count; i++) {
        if (strcmp(catalog->courses[i].code, code) == 0) {
            return &catalog->courses[i];
        }
    }
    return NULL;
}

Course* catalog_add_course(Catalog *catalog, const char *code, const char *name, int credits, int semester) {
    if (catalog->course_count == catalog->course_capacity) {
        size_t new_cap = (catalog->course_capacity == 0) ? INITIAL_CAPACITY : catalog->course_capacity * 2;
        Course *temp = realloc(catalog->courses, new_cap * sizeof(Course));
        if (!temp) return NULL;
        catalog->courses = temp;
        catalog->course_capacity = new_cap;
    }
    
    Course *new_course = &catalog->courses[catalog->course_count];
    course_init(new_course);
    
    new_course->code = malloc(strlen(code) + 1);
    new_course->name = malloc(strlen(name) + 1);
    
    if (!new_course->code || !new_course->name) {
        free(new_course->code); // Safe due to free(NULL) behavior
        free(new_course->name);
        return NULL;
    }
    
    strcpy(new_course->code, code);
    strcpy(new_course->name, name);
    new_course->credits = credits;
    new_course->semester = semester;
    
    catalog->course_count++;
    return new_course;
}