# Memory Contract - Etapa 1

## Propósito

Este documento define las reglas de propiedad, inicialización y liberación
de memoria utilizadas por los modelos del programa.

La regla general es:

```text
Cada estructura es responsable de liberar la memoria que posee directamente.
```

---

## Propiedad de memoria

```text
Catalog
├── career_code
├── career_name
└── Course[]
    ├── code
    ├── name
    ├── CodeList requirements
    ├── CodeList corequisites
    ├── CodeList pending_corequisites
    └── Group[]
        └── Schedule[]

StudentHistory
└── CodeList approved_courses
```

Propietarios:

```text
Catalog
→ career_code
→ career_name
→ Course[]

Course
→ code
→ name
→ requirements
→ corequisites
→ pending_corequisites
→ Group[]

Group
→ Schedule[]

CodeList
→ items[]
→ cada string almacenado en items

StudentHistory
→ approved_courses
```

`Schedule` no posee memoria dinámica actualmente.

---

## Inicialización

Toda estructura debe inicializarse antes de utilizarse.

```text
code_list_init()
schedule_init()
group_init()
course_init()
catalog_init()
student_history_init()
```

Estado inicial esperado:

```text
punteros     = NULL
count        = 0
capacity     = 0
booleanos    = false
valores inválidos = valores definidos por el modelo
```

Ejemplo:

```text
Schedule
day = DAY_INVALID
start_minutes = -1
end_minutes = -1
```

---

## Liberación

La liberación ocurre de forma jerárquica.

```text
catalog_free()
    ↓
course_free()
    ↓
group_free()
    ↓
schedule_free()
```

`CodeList` libera:

```text
cada items[i]
↓
items
```

Ejemplo:

```text
CodeList
├── "CE1101"
├── "MA1102"
└── "FI1101"

        ↓ code_list_free()

items = NULL
count = 0
capacity = 0
```

Después de liberar una estructura, esta debe regresar a su estado inicial.

---

## Regla de ownership de strings

Todo string almacenado dentro del modelo debe poseer memoria independiente
del buffer temporal que lo originó.

Ejemplo incorrecto:

```text
Parser buffer
     ↓
items[i] apunta directamente al buffer
```

El parser puede reutilizar ese buffer y sobrescribir el contenido.

Ejemplo correcto:

```text
Parser buffer
"CE1101"
     ↓ copia
memoria propia
"CE1101"
     ↓
CodeList.items[i]
```

Por lo tanto:

```text
CodeList es dueño de los strings almacenados en items.
```

Las futuras funciones como:

```text
code_list_add()
```

deberán copiar el código recibido y almacenar una copia propia.

La misma regla aplica a:

```text
Course.code
Course.name
Catalog.career_code
Catalog.career_name
```

---

## Responsabilidad del parser

El parser puede utilizar buffers temporales.

Ejemplo:

```text
línea TSV
↓
buffer temporal
↓
token
↓
copia al modelo
```

El modelo nunca debe depender de la duración de vida del buffer del parser.

Regla:

```text
Parser lee
→ Modelo copia
→ Parser puede reutilizar su memoria
```

---

## Estado global

No se utilizará estado global mutable.

Incorrecto:

```c
Catalog global_catalog;
StudentHistory global_history;
```

Correcto:

```text
main()
├── Catalog catalog
└── StudentHistory history
```

Los módulos reciben punteros explícitamente:

```text
parse_catalog(..., &catalog)

validate_courses(&catalog, &history)

detect_conflicts(&catalog)
```

Esto permite que:

```text
los datos sean explícitos
los módulos sean reutilizables
las pruebas sean independientes
el estado no quede oculto
```

---

## Regla final

```text
Quien posee memoria
→ es responsable de liberarla.

Quien recibe datos temporales
→ debe copiarlos si necesita conservarlos.

No existen estructuras globales mutables.

Toda estructura:
init → uso → free
```