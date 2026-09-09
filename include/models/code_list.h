#ifndef CODE_LIST_H
#define CODE_LIST_H

#include <stddef.h>

typedef struct {
    char **items; // Aqui se almacenaran los codigos de los cursos
    size_t count;
    size_t capacity;
    //lista dinamicas sin límites
} CodeList;

void code_list_init(CodeList *list); // se inicia un codelist
void code_list_free(CodeList *list); // se limpia un codelist
#endif