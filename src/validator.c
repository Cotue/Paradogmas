#include "validator.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

Status validate_column_count(int count) {
    if (count != 11) {
        return ERROR_INVALID_FORMAT;
    }
    return SUCCESS;
}

Status validate_course_identity(const char *code, const char *name) {
    if (!code || code[0] == '\0' || strcmp(code, EMPTY_FIELD) == 0) {
        return ERROR_INVALID_FORMAT;
    }
    // Límite de longitud agregado para seguridad
    if (strlen(code) >= MAX_COURSE_CODE_LENGTH) {
        return ERROR_INVALID_FORMAT;
    }

    if (!name || name[0] == '\0' || strcmp(name, EMPTY_FIELD) == 0) {
        return ERROR_INVALID_FORMAT;
    }
    // Límite de longitud agregado para seguridad
    if (strlen(name) >= MAX_COURSE_NAME_LENGTH) {
        return ERROR_INVALID_FORMAT;
    }
    return SUCCESS;
}

Status validate_group_format(const char *group_str) {
    if (!group_str || group_str[0] == '\0') {
        return ERROR_INVALID_FORMAT;
    }
    // Verificar que todos los caracteres sean dígitos
    for (int i = 0; group_str[i] != '\0'; i++) {
        if (!isdigit(group_str[i])) {
            return ERROR_INVALID_FORMAT;
        }
    }
    
    int group = atoi(group_str);
    if (group <= 0) {
        return ERROR_INVALID_FORMAT;
    }
    
    return SUCCESS;
}

Status validate_day_format(const char *day_str) {
    if (!day_str) return ERROR_INVALID_FORMAT;

    const char *valid_days[] = {"LUN", "MAR", "MIE", "JUE", "VIE", "SAB"};
    int num_valid_days = 6;

    for (int i = 0; i < num_valid_days; i++) {
        if (strcmp(day_str, valid_days[i]) == 0) {
            return SUCCESS;
        }
    }
    return ERROR_INVALID_FORMAT;
}

// Función auxiliar privada para convertir "HH:MM" a minutos
static int time_to_minutes(const char *time_str) {
    if (!time_str || strlen(time_str) != 5 || time_str[2] != ':') {
        return -1;
    }
    // Verificar que los demás caracteres sean dígitos
    if (!isdigit(time_str[0]) || !isdigit(time_str[1]) || 
        !isdigit(time_str[3]) || !isdigit(time_str[4])) {
        return -1;
    }

    int h = atoi(time_str);       // Extrae las horas
    int m = atoi(time_str + 3);   // Extrae los minutos (saltando "HH:")

    if (h < 0 || h > 23 || m < 0 || m > 59) {
        return -1;
    }
    
    return (h * 60) + m;
}

Status validate_time_format(const char *start_time_str, const char *end_time_str) {
    int start_min = time_to_minutes(start_time_str);
    int end_min = time_to_minutes(end_time_str);

    if (start_min == -1 || end_min == -1) {
        return ERROR_INVALID_FORMAT;
    }
    if (start_min >= end_min) {
        return ERROR_INVALID_FORMAT; // La hora final debe ser posterior a la de inicio
    }

    return SUCCESS;
}

// =================================================================
// NUEVAS FUNCIONES AGREGADAS PARA INTEGRAR CON CATALOG_PARSER.C
// =================================================================

Status validate_semester_format(const char *sem_str) {
    if (!sem_str || sem_str[0] == '\0') return ERROR_INVALID_FORMAT;
    
    for (int i = 0; sem_str[i] != '\0'; i++) {
        if (!isdigit(sem_str[i])) return ERROR_INVALID_FORMAT;
    }
    
    int sem = atoi(sem_str);
    // La guía restringe a los primeros 4 semestres
    if (sem < 1 || sem > 4) {
        return ERROR_INVALID_FORMAT;
    }
    return SUCCESS;
}

Status validate_credits_format(const char *cred_str) {
    if (!cred_str || cred_str[0] == '\0') return ERROR_INVALID_FORMAT;
    
    for (int i = 0; cred_str[i] != '\0'; i++) {
        if (!isdigit(cred_str[i])) return ERROR_INVALID_FORMAT;
    }
    
    int credits = atoi(cred_str);
    if (credits < 0) { 
        return ERROR_INVALID_FORMAT;
    }
    return SUCCESS;
}

// Corrección: Retorna directamente 'Day' sin usar la palabra 'enum'
Day parse_day_string(const char* day_str) {
    if (day_str == NULL) return DAY_INVALID;
    if (strcmp(day_str, "LUN") == 0) return MONDAY;
    if (strcmp(day_str, "MAR") == 0) return TUESDAY;
    if (strcmp(day_str, "MIE") == 0) return WEDNESDAY;
    if (strcmp(day_str, "JUE") == 0) return THURSDAY;
    if (strcmp(day_str, "VIE") == 0) return FRIDAY;
    if (strcmp(day_str, "SAB") == 0) return SATURDAY;
    return DAY_INVALID;
}