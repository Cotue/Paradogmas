#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "constants.h"
#include "models.h" 
#include "constants.h"
#include "models/day.h"
/* 
 * Verifica que la fila procesada por 7A contenga la cantidad exacta de columnas.
 * Retorna SUCCESS o ERROR_INVALID_FORMAT.
 */
Status validate_column_count(int count);

/* 
 * Verifica que los campos obligatorios (código y nombre) no estén vacíos
 * ni contengan únicamente el EMPTY_FIELD ("-").
 * Retorna SUCCESS o ERROR_INVALID_FORMAT.
 */
Status validate_course_identity(const char *code, const char *name);

/* 
 * Verifica que la cadena represente un número de grupo válido (entero positivo).
 * Retorna SUCCESS o ERROR_INVALID_FORMAT.
 */
Status validate_group_format(const char *group_str);

/* 
 * Verifica que el día esté normalizado según las convenciones del proyecto.
 * Retorna SUCCESS o ERROR_INVALID_FORMAT.
 */
Status validate_day_format(const char *day_str);

/* 
 * Verifica que la hora inicial y final cumplan el formato "HH:MM",
 * sean horas válidas del día, y que la hora inicial sea estrictamente menor a la final.
 * Retorna SUCCESS o ERROR_INVALID_FORMAT.
 */
Status validate_time_format(const char *start_time_str, const char *end_time_str);
Status validate_semester_format(const char *sem_str);
Status validate_credits_format(const char *cred_str) ;
Day parse_day_string(const char* day_str);

#endif /* VALIDATOR_H */