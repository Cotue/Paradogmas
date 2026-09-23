#include "history_parser.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
static char *trim_whitespace(char *text)
{
    if (text == NULL) {
        return NULL;
    }

    //Avanzar mientras existan espacios al inicio 
    while (isspace((unsigned char)*text)) {
        text++;
    }

    //Si después del trim no queda contenido 
    if (*text == '\0') {
        return text;
    }

   
    char *end = text + strlen(text) - 1;

    while (end > text && isspace((unsigned char)*end)) {
        end--;
    }

   // terminar strg
    end[1] = '\0';

    return text;
}


Status parse_student_history(
    const char *file_path,
    Catalog *catalog,
    StudentHistory *history
)
{
   
    if (file_path == NULL || catalog == NULL || history == NULL) {
        return ERROR_ARGS;
    }

    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
        return ERROR_FILE_NOT_FOUND;
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {

        // para leer dato por dato segun la longitud definida y no saltarse cursos
        if (strchr(line, '\n') == NULL && !feof(file)) {
            fclose(file);
            student_history_free(history);

            return ERROR_INVALID_FORMAT;
        }

        char *code = trim_whitespace(line);

        // si no hay cursos
        if (*code == '\0') {
            continue;
        }

        // si se duplica formato invalido
        if (code_list_contains(&history->approved_courses, code)) {
            fclose(file);
            student_history_free(history);

            return ERROR_INVALID_FORMAT;
        }

        // se comprara con el catalogo para revisar si ese codigo existe
        if (find_course_by_code(catalog, code) == NULL) {
            fclose(file);
            student_history_free(history);

            return ERROR_INVALID_FORMAT;
        }
        
        
        //CodeList crea su propia copia del código.
        
        if (!code_list_add(&history->approved_courses, code)) {
            fclose(file);
            student_history_free(history);

            return ERROR_MEMORY;
        }
    }

    /*
     * fgets() también puede detenerse por un error de lectura,
     * no solamente por EOF.
     */
    if (ferror(file)) {
        fclose(file);
        student_history_free(history);

        return ERROR_INVALID_FORMAT;
    }

    fclose(file);

    return SUCCESS;
}