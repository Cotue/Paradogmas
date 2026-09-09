# Model Flow - Etapa 1

## Estructura general

```text
Catalog
└── Course[]
    ├── CodeList requirements
    ├── CodeList corequisites
    ├── CodeList pending_corequisites
    └── Group[]
        └── Schedule[]
            └── Day

StudentHistory
└── CodeList approved_courses
```

---

## day.h

```c
typedef enum {
    DAY_INVALID = -1,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
} Day;
```

Uso:

```text
"LUN" -> parser -> MONDAY -> Schedule.day
"DAY_INVALID = -1," Funciona para iniciar un horario en 0
Horario recién inicializado puede tener:
day = DAY_INVALID
start_minutes = -1
end_minutes = -1
```

---

## code_list.h

```c
typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} CodeList;
```

- `items`: códigos almacenados.
- `count`: cantidad real.
- `capacity`: memoria reservada.

Usado por:

```text
Course.requirements
Course.corequisites
Course.pending_corequisites
StudentHistory.approved_courses
```

---

## schedule.h

```c
typedef struct {
    Day day;
    int start_minutes;
    int end_minutes;
} Schedule;
```

- `day`: día.
- `start_minutes`: hora inicial en minutos.
- `end_minutes`: hora final en minutos.

Ejemplo:

```text
LUN 07:30-09:20
↓
MONDAY, 450, 560
```

Flujo:

```text
TSV -> parser -> Schedule -> Group
```

---

## group.h

```c
typedef struct {
    int number;

    Schedule *schedules;
    size_t schedule_count;
    size_t schedule_capacity;
    bool has_conflict;
} Group;
```

- `number`: número del grupo.
- `schedules`: horarios del grupo.
- `schedule_count`: cantidad real de horarios.
- `schedule_capacity`: memoria reservada.
- `has_conflict`: conflicto de grupo.

Identificación:

```text
(course.code, group.number)
```

Flujo:

```text
Schedule[] -> Group -> Course
```

---

## course.h

```c
typedef struct {
    char *code;
    char *name;

    int credits;
    int semester;

    CodeList requirements;
    CodeList corequisites;
    CodeList pending_corequisites;

    Group *groups;
    size_t group_count;
    size_t group_capacity;

    bool has_conflict;
    bool can_enroll;
} Course;
```

- `code`: código del curso.
- `name`: nombre.
- `credits`: créditos.
- `semester`: semestre.
- `requirements`: requisitos obligatorios previos.
- `corequisites`: correquisitos definidos.
- `pending_corequisites`: correquisitos no aprobados.
- `groups`: grupos disponibles.
- `group_count`: cantidad real de grupos.
- `group_capacity`: memoria reservada.
- `has_conflict`: existe al menos un choque.
- `can_enroll`: puede matricularse académicamente.

Flujos:

```text
Course.requirements
+ StudentHistory.approved_courses
-> can_enroll
```

```text
Course.corequisites
+ StudentHistory.approved_courses
-> pending_corequisites
```

---

## catalog.h

```c
typedef struct {
    char *career_code;
    char *career_name;

    Course *courses;
    size_t course_count;
    size_t course_capacity;
} Catalog;
```

- `career_code`: código de carrera.
- `career_name`: nombre de carrera.
- `courses`: cursos.
- `course_count`: cantidad real.
- `course_capacity`: memoria reservada.

Cada `Catalog` representa una carrera.

Flujo:

```text
catalogo.tsv -> Catalog Parser -> Catalog
```

---

## student_history.h

```c
typedef struct {
    CodeList approved_courses;
} StudentHistory;
```

- `approved_courses`: códigos aprobados por el estudiante.

Flujo:

```text
historial.txt -> History Parser -> StudentHistory
```

---

## models.h

Agrupa todos los modelos:

```c
#include "models/day.h"
#include "models/code_list.h"
#include "models/schedule.h"
#include "models/group.h"
#include "models/course.h"
#include "models/catalog.h"
#include "models/student_history.h"
```

Otros módulos pueden usar:

```c
#include "models.h"
```

---

## Flujo completo

```text
catalogo.tsv
-> Catalog Parser
-> Catalog
-> Course[]
-> Group[]
-> Schedule[]

historial.txt
-> History Parser
-> StudentHistory
```

Luego:

```text
Catalog + StudentHistory
-> validar requisitos
-> identificar correquisitos pendientes
-> calcular can_enroll
```

```text
Catalog
-> comparar Schedule[]
-> calcular choques
-> has_conflict
```

Finalmente:

```text
Catalog procesado
-> JSON Serializer
-> catalogo_procesado.json
```

Regla clave:

```text
TSV/TXT = entrada
Structs = modelo interno
JSON = salida
```