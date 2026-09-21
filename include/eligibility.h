#ifndef ELIGIBILITY_H
#define ELIGIBILITY_H

#include "models/catalog.h"
#include "models/student_history.h"
#include "constants.h"

/*
 * Evalúa si un curso individual se puede matricular analizando sus requisitos
 * y registra los correquisitos pendientes que no han sido pasados por el estudiante.
 */
void evaluate_course_eligibility(Course *course, const StudentHistory *history);

/*
 * Recorre todo el catálogo evaluando la elegibilidad académica de cada curso.
 * Retorna SUCCESS o ERROR_ARGS en caso de recibir punteros nulos.
 */
Status evaluate_catalog_eligibility(Catalog *catalog, const StudentHistory *history);

#endif /* ELIGIBILITY_H */