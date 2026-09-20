#include <stdio.h>
#include <string.h>
#include "validator.h"

static int test_column_count(void) {
    int fails = 0;

    fails += validate_column_count(11) != SUCCESS;
    fails += validate_column_count(10) != ERROR_INVALID_FORMAT;
    fails += validate_column_count(12) != ERROR_INVALID_FORMAT;

    return fails;
}

static int test_course_identity(void) {
    int fails = 0;

    fails += validate_course_identity("CE1101", "Intro") != SUCCESS;
    fails += validate_course_identity("", "Intro") != ERROR_INVALID_FORMAT;
    fails += validate_course_identity("-", "Intro") != ERROR_INVALID_FORMAT;
    fails += validate_course_identity("CE1101", "") != ERROR_INVALID_FORMAT;

    char long_code[MAX_COURSE_CODE_LENGTH + 5];
    memset(long_code, 'A', sizeof(long_code) - 1);
    long_code[sizeof(long_code) - 1] = '\0';

    char long_name[MAX_COURSE_NAME_LENGTH + 5];
    memset(long_name, 'A', sizeof(long_name) - 1);
    long_name[sizeof(long_name) - 1] = '\0';

    fails += validate_course_identity(long_code, "Intro") != ERROR_INVALID_FORMAT;
    fails += validate_course_identity("CE1101", long_name) != ERROR_INVALID_FORMAT;

    return fails;
}

static int test_semester(void) {
    int fails = 0;

    fails += validate_semester_format("1") != SUCCESS;
    fails += validate_semester_format("4") != SUCCESS;
    fails += validate_semester_format("5") != ERROR_INVALID_FORMAT;
    fails += validate_semester_format("A") != ERROR_INVALID_FORMAT;

    return fails;
}

static int test_credits(void) {
    int fails = 0;

    fails += validate_credits_format("4") != SUCCESS;
    fails += validate_credits_format("0") != SUCCESS;
    fails += validate_credits_format("-1") != ERROR_INVALID_FORMAT;
    fails += validate_credits_format("X") != ERROR_INVALID_FORMAT;

    return fails;
}

static int test_group(void) {
    int fails = 0;

    fails += validate_group_format("10") != SUCCESS;
    fails += validate_group_format("0") != ERROR_INVALID_FORMAT;
    fails += validate_group_format("G1") != ERROR_INVALID_FORMAT;

    return fails;
}

static int test_day(void) {
    int fails = 0;

    fails += validate_day_format("LUN") != SUCCESS;
    fails += validate_day_format("DOM") != ERROR_INVALID_FORMAT;
    fails += parse_day_string("MAR") != TUESDAY;
    fails += parse_day_string("XXX") != DAY_INVALID;

    return fails;
}

static int test_time(void) {
    int fails = 0;

    fails += validate_time_format("07:30", "09:20") != SUCCESS;
    fails += validate_time_format("09:20", "07:30") != ERROR_INVALID_FORMAT;
    fails += validate_time_format("25:00", "26:00") != ERROR_INVALID_FORMAT;
    fails += validate_time_format("7:30", "09:20") != ERROR_INVALID_FORMAT;

    return fails;
}

int main(void) {
    int fails = 0;

    fails += test_column_count();
    fails += test_course_identity();
    fails += test_semester();
    fails += test_credits();
    fails += test_group();
    fails += test_day();
    fails += test_time();

    printf("Pruebas validator - Fallos: %d\n", fails);

    return fails ? 1 : 0;
}