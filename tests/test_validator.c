#include <stdio.h>
#include "validator.h"

void test_column_count() {
    printf("--- Test Column Count ---\n");
    printf("11 columnas (Debe ser PASS): %s\n", validate_column_count(11) == SUCCESS ? "PASS" : "FAIL");
    printf("10 columnas (Debe ser PASS): %s\n", validate_column_count(10) == ERROR_INVALID_FORMAT ? "PASS" : "FAIL");
}

void test_course_identity() {
    printf("\n--- Test Course Identity ---\n");
    printf("Campos validos (Debe ser PASS): %s\n", validate_course_identity("CE1101", "Intro") == SUCCESS ? "PASS" : "FAIL");
    printf("Codigo vacio (Debe ser PASS): %s\n", validate_course_identity("", "Intro") == ERROR_INVALID_FORMAT ? "PASS" : "FAIL");
    printf("Codigo es '-' (Debe ser PASS): %s\n", validate_course_identity("-", "Intro") == ERROR_INVALID_FORMAT ? "PASS" : "FAIL");
}

void test_time_format() {
    printf("\n--- Test Time Format ---\n");
    printf("Horario valido 07:30-09:20 (Debe ser PASS): %s\n", validate_time_format("07:30", "09:20") == SUCCESS ? "PASS" : "FAIL");
    printf("Hora invertida 09:20-07:30 (Debe ser PASS): %s\n", validate_time_format("09:20", "07:30") == ERROR_INVALID_FORMAT ? "PASS" : "FAIL");
    printf("Formato incorrecto 25:00 (Debe ser PASS): %s\n", validate_time_format("25:00", "26:00") == ERROR_INVALID_FORMAT ? "PASS" : "FAIL");
}

void test_day_format() {
    printf("\n--- Test Day Format ---\n");
    printf("Dia valido LUN (Debe ser PASS): %s\n", validate_day_format("LUN") == SUCCESS ? "PASS" : "FAIL");
    printf("Dia invalido LUNES (Debe ser PASS): %s\n", validate_day_format("LUNES") == ERROR_INVALID_FORMAT ? "PASS" : "FAIL");
}

int main() {
    printf("INICIANDO PRUEBAS UNITARIAS DE KAN-7B...\n\n");
    
    test_column_count();
    test_course_identity();
    test_time_format();
    test_day_format();
    
    printf("\nPRUEBAS FINALIZADAS.\n");
    return 0;
}