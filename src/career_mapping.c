#include "career_mapping.h"

#include <stddef.h>
#include <string.h>

typedef struct {
    const char *prefix;
    const char *career;
} CareerMapping;

static const CareerMapping career_mappings[] = {
    {"CE", "computadores"},
    {"CA", "computacion"},
    {"EL", "electronica"},
    {"AE", "administracion"},
    {"CI", "lenguaje"},
    {"CS", "sociales"},
    {"FI", "fisica"},
    {"MA", "matematica"},
    {"MI", "electromecanica"},
    {"PI", "produccion_industrial"},
    {"QU", "quimica"},
    {"SE", "cultura_deporte"},
    {"SO", "seguridad_laboral"},
    {"CM", "materiales"}
};

const char *get_career_from_course_code(const char *course_code)
{
    if (course_code == NULL || strlen(course_code) < 2) {
        return NULL;
    }

    for (size_t i = 0;
         i < sizeof(career_mappings) / sizeof(career_mappings[0]);
         i++) {

        if (strncmp(course_code,
                    career_mappings[i].prefix,
                    2) == 0) {
            return career_mappings[i].career;
        }
    }

    return NULL;
}