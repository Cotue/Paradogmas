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

#include "constants.h"
#include <string.h>

int code_list_add(CodeList *list, const char *code) {
    if (list->count == list->capacity) {
        size_t new_cap = (list->capacity == 0) ? INITIAL_CAPACITY : list->capacity * 2;
        char **temp = realloc(list->items, new_cap * sizeof(char *));
        if (!temp) return 0;
        list->items = temp;
        list->capacity = new_cap;
    }
    
    list->items[list->count] = malloc(strlen(code) + 1);
    if (!list->items[list->count]) return 0;
    
    strcpy(list->items[list->count], code);
    list->count++;
    return 1;
}
