#include "models/code_list.h"

#include <stdlib.h>

void code_list_init(CodeList *list) // iniciar una lista de codigos de cursos
{
    if (list == NULL) {
        return;
    }

    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void code_list_free(CodeList *list) //liberar la memoria de una lista
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        free(list->items[i]);
    }

    free(list->items);

    code_list_init(list);
}