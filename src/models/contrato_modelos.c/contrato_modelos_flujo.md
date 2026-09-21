# Model Data Flow - Etapa 1

## Modelo principal

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

## Day

```c
Day
```

Representa el día interno del horario.

```text
"LUN"
→ parser
→ MONDAY
→ Schedule.day
```

---

## CodeList

```c
char **items;
size_t count;
size_t capacity;
```

Usado por:

```text
Course.requirements
Course.corequisites
Course.pending_corequisites
StudentHistory.approved_courses
```

Significado:

```text
items
→ códigos almacenados

count
→ cantidad real

capacity
→ memoria reservada
```

---

## Schedule

```c
Day day;
int start_minutes;
int end_minutes;
```

Ejemplo:

```text
LUN 07:30-09:20
↓
MONDAY
450
560
```

Flujo:

```text
TSV
→ parser
→ Schedule
→ Group
```

---

## Group

```c
int number;

Schedule *schedules;
size_t schedule_count;
size_t schedule_capacity;

bool has_conflict;
```

Representa:

```text
Course
└── Group
    └── Schedule[]
```

Identificación:

```text
(course.code, group.number)
```

---

## Course

```c
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
```

Datos base recibidos del catálogo:

```text
code
name
credits
semester
requirements
corequisites
groups
```

Datos calculados:

```text
pending_corequisites
can_enroll
has_conflict
```

Flujos:

```text
requirements
+
StudentHistory.approved_courses
→ can_enroll
```

```text
corequisites
+
StudentHistory.approved_courses
→ pending_corequisites
```

```text
Group[].Schedule[]
→ comparación
→ has_conflict
```

---

## Catalog

```c
char *career_code;
char *career_name;

Course *courses;
size_t course_count;
size_t course_capacity;
```

Cada `Catalog` representa una carrera.

Entrada:

```text
catalogo.tsv
→ Catalog Parser
→ Catalog
```

---

## StudentHistory

```c
CodeList approved_courses;
```

Entrada:

```text
historial.txt
→ History Parser
→ StudentHistory
```

---

## Flujo completo

```text
catalogo.tsv
→ Catalog Parser
→ Catalog
→ Course[]
→ Group[]
→ Schedule[]
```

```text
historial.txt
→ History Parser
→ StudentHistory
```

Procesamiento académico:

```text
Catalog.Course.requirements
+
StudentHistory.approved_courses
→ validar requisitos
→ Course.can_enroll
```

Correquisitos:

```text
Catalog.Course.corequisites
+
StudentHistory.approved_courses
→ Course.pending_corequisites
```

Choques:

```text
Catalog
→ Course[]
→ Group[]
→ Schedule[]
→ comparar horarios
→ Group.has_conflict
→ Course.has_conflict
```

Salida:

```text
Catalog procesado
→ JSON Serializer
→ catalogo_procesado.json
```

---

## Regla de arquitectura

```text
TSV/TXT
= entrada externa

Structs
= modelo interno

JSON
= salida externa
```

El JSON no se genera directamente desde el TSV.

Primero:

```text
entrada
→ modelo
→ procesamiento
→ serialización
```