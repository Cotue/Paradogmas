#include "career_mapping.h"

#include <stddef.h>
#include <string.h>

typedef struct {
    const char *prefix;
    const char *career;
} CareerMapping;

static const CareerMapping career_mappings[] = {
    {"CE", "Escuela de Ingeniería en Computadores"},
    {"CA", "Escuela de Ingeniería en Computación"},
    {"EL", "Escuela de Ingeniería Electrónica"},
    {"AE", "Escuela de Administración de Empresas"},
    {"CI", "Escuela de Ciencias del Lenguaje"},
    {"CS", "Escuela de Ciencias Sociales"},
    {"FI", "Escuela de Física"},
    {"MA", "Escuela de Matemática"},
    {"MI", "Escuela de Ingeniería Electromecánica"},
    {"PI", "Escuela de Ingeniería en Producción Industrial"},
    {"QU", "Escuela de Química"},
    {"SE", "Escuela de Cultura y Deporte"},
    {"SO", "Escuela de Ingeniería en Seguridad Laboral e Higiene Ambiental"},
    {"CM", "Escuela de Ciencia e Ingeniería de los Materiales"}
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