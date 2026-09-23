Claro. A continuación te dejo una versión de la documentación pensada para colocar directamente en el `README.md`, siguiendo exactamente la estructura solicitada para la entrega: **Arquitectura del proyecto, Decisiones de diseño y Estructuras de datos desarrolladas**, y agregando las secciones necesarias para que el proyecto quede entendible y defendible.

# CEmestre — Etapa 1: Paradigma Imperativo

## 1. Descripción del proyecto

CEmestre es un sistema desarrollado de manera incremental durante el curso de Paradigmas de Programación. Su objetivo general es apoyar a un estudiante en la construcción de un horario de matrícula válido, utilizando diferentes paradigmas de programación en cada etapa del proyecto. La primera etapa corresponde al paradigma imperativo y se desarrolla utilizando el lenguaje C. 

La responsabilidad específica de esta etapa consiste en cargar y procesar el catálogo de cursos de una carrera junto con el historial académico de un estudiante. A partir de esta información, el programa determina los conflictos de horario entre grupos, evalúa los requisitos y correquisitos de cada curso y genera un catálogo procesado que será utilizado posteriormente por la etapa desarrollada en Racket. 

El módulo desarrollado en C no construye el horario final ni recomienda combinaciones de cursos. Su resultado consiste únicamente en un archivo con la información necesaria para que las siguientes etapas puedan continuar el procesamiento. 

---

# 2. Arquitectura del proyecto

La arquitectura de CEmestre busca separar claramente cuatro responsabilidades:

```text
Entrada de datos
      ↓
Construcción del modelo interno
      ↓
Procesamiento
      ↓
Serialización
```

El flujo completo de esta primera etapa es:

```text
FUENTES INSTITUCIONALES
Guía de Horarios / TEC Digital
          ↓
  Recolección manual
          ↓
 Limpieza y normalización
          ↓
      catalogo.tsv
          │
          │
          ↓
      PROGRAMA C
          ↑
          │
     historial.txt
          │
          ↓
 ┌───────────────────────┐
 │   Modelo en memoria   │
 │                       │
 │ Catalog               │
 │ StudentHistory        │
 └───────────────────────┘
          │
          ├────────────────────────┐
          ↓                        ↓
 Validación académica      Detección de choques
 requisitos/correquisitos        horarios
          │                        │
          └────────────┬───────────┘
                       ↓
              Catálogo procesado
                       ↓
           catalogo_procesado.json
                       ↓
                     Racket
```

El catálogo se obtiene primero de fuentes institucionales y posteriormente se transforma a un archivo normalizado. El programa en C nunca procesa directamente la fuente HTML o la Guía de Horarios, sino que trabaja únicamente con los datos previamente limpiados. 

Dentro del programa, el catálogo y el historial se convierten a estructuras internas en memoria. Después se ejecutan de forma independiente la evaluación académica y la detección de conflictos. Finalmente, toda la información calculada se serializa a JSON.  

Esta separación evita que la representación del archivo de entrada o del archivo de salida determine directamente la estructura interna utilizada por el programa.

---

## 2.1 Organización del repositorio

El proyecto separa código fuente, archivos de cabecera, pruebas y datos mediante la siguiente organización:

```text
CEmestre/
│
├── src/
│   ├── main.c
│   ├── catalog_parser.c
│   ├── history_parser.c
│   ├── conflict_detector.c
│   ├── eligibility.c
│   ├── json_serializer.c
│   └── models/
│
├── include/
│   ├── constants.h
│   ├── catalog_parser.h
│   ├── history_parser.h
│   ├── conflict_detector.h
│   ├── eligibility.h
│   ├── json_serializer.h
│   └── models/
│
├── tests/
│   └── fixtures/
│
├── data/
│   ├── raw/
│   ├── clean/
│   ├── historiales/
│   └── output/
│
├── docs/
│
├── Makefile
└── README.md
```

La estructura base distingue explícitamente los directorios de código, cabeceras, pruebas y archivos de datos. Dentro de `data/`, los datos recolectados, los datos normalizados, los historiales y las salidas se mantienen separados.  

---

## 2.2 Flujo de carga

El ejecutable trabaja inicialmente con dos entradas:

```text
catalogo.tsv
historial.txt
```

El orden de procesamiento es importante:

```text
Catalog
   ↑
catalogo.tsv

      ↓

StudentHistory
   ↑
historial.txt
```

Primero se carga y valida el catálogo. Solo cuando esta operación termina correctamente se carga el historial, ya que cada código aprobado debe comprobarse contra los cursos existentes en el catálogo. 

Conceptualmente:

```text
parse_catalog()
      ↓
Catalog válido
      ↓
parse_student_history()
      ↓
StudentHistory válido
```

Un error en la carga del catálogo detiene el proceso antes de intentar cargar el historial.

---

# 3. Decisiones de diseño

## 3.1 Separación entre fuentes originales y datos normalizados

Los datos provenientes de la Guía de Horarios o de TEC Digital no se consumen directamente desde el programa.

El proceso utilizado es:

```text
Fuente institucional
        ↓
Recolección
        ↓
Limpieza
        ↓
Normalización
        ↓
TSV
        ↓
Programa C
```

Esta decisión permite mantener la implementación del programa independiente de cambios en la estructura HTML o en la presentación de las fuentes institucionales. 

También permite conservar dos representaciones diferentes:

```text
data/raw/
```

para trazabilidad de los datos recolectados, y:

```text
data/clean/
```

para la información normalizada que realmente consume el programa. 

---

## 3.2 Uso de TSV para el catálogo

Se decidió utilizar TSV (`Tab-Separated Values`) como formato de entrada del catálogo.

Este formato fue escogido porque:

* puede ser procesado fácilmente utilizando C;
* no requiere bibliotecas externas;
* puede generarse o editarse desde herramientas de hojas de cálculo;
* permite identificar columnas faltantes con facilidad;
* evita problemas producidos por comas dentro de nombres de cursos.

Cada fila del archivo no representa necesariamente un curso completo. Una fila representa un **bloque horario**. 

Por ejemplo:

```text
CE1101  Grupo 1  LUN  07:30  09:20
CE1101  Grupo 1  JUE  07:30  09:20
```

se reconstruye internamente como:

```text
CE1101
└── Grupo 1
    ├── LUN 07:30 - 09:20
    └── JUE 07:30 - 09:20
```

De esta forma, el formato del archivo permanece plano mientras que el programa reconstruye la jerarquía real `Curso → Grupo → Horarios`. 

---

## 3.3 Uso de TXT para el historial académico

El historial académico utiliza un archivo de texto plano porque únicamente necesita almacenar códigos de cursos aprobados.

Ejemplo:

```text
CE1101
MA1102
FI1101
```

Cada línea representa exactamente un curso aprobado. 

Durante la carga:

```text
historial.txt
      ↓
eliminar espacios externos
      ↓
validar duplicados
      ↓
validar existencia en Catalog
      ↓
StudentHistory
```

El programa no transforma códigos automáticamente. Por ejemplo:

```text
ce1101
```

no se convierte en:

```text
CE1101
```

La comparación se mantiene exacta para evitar ocultar inconsistencias en los datos. 

---

## 3.4 Identificación de los grupos

El número de grupo no puede utilizarse como identificador global.

Por ejemplo:

```text
CE1101 Grupo 1
MA1102 Grupo 1
```

representan grupos diferentes.

Por esta razón, la identidad conceptual utilizada es:

```text
(códigoCurso, númeroGrupo)
```

Esta misma representación se utiliza posteriormente para almacenar los conflictos entre grupos. 

---

## 3.5 Separación entre elegibilidad académica y conflictos

Se decidió mantener independientes:

```text
puedeMatricular
```

y:

```text
tieneChoque
```

`puedeMatricular` representa si el estudiante satisface las condiciones académicas requeridas para el curso.

`tieneChoque` representa únicamente la existencia de una superposición entre los horarios de algún grupo del curso y otro grupo externo.

Por lo tanto, es completamente válido tener:

```text
puedeMatricular = true
tieneChoque = true
```

Esto significa que el estudiante cumple académicamente las condiciones del curso, pero deberá seleccionar posteriormente grupos compatibles. 

La selección de grupos compatibles no corresponde a esta etapa.

---

## 3.6 Requisitos y correquisitos

Los requisitos y correquisitos se mantienen separados debido a que representan reglas académicas distintas.

Un requisito debe haber sido aprobado previamente.

```text
Curso A
Requisito: Curso X
```

Si `Curso X` no aparece en el historial:

```text
puedeMatricular = false
```

En cambio, un correquisito puede haber sido aprobado previamente o matricularse simultáneamente.

```text
Curso A
Correquisito: Curso B
```

Si `Curso B` todavía no fue aprobado:

```text
puedeMatricular = true

correquisitosPendientes:
Curso B
```

Esto indica que el estudiante puede considerar Curso A, pero deberá incluir Curso B simultáneamente. 

---

# 4. Decisiones específicas del dataset

El proyecto utiliza dos carreras:

```text
Ingeniería en Computadores
Ingeniería Electrónica
```

y considera los primeros cuatro bloques de cada plan de estudios. 

Para recolectar la oferta de cursos se adoptaron las siguientes decisiones:

```text
Campus:
Campus Tecnológico Central Cartago

Modalidad:
SEMESTRE

Periodo:
se prioriza periodo 1

Periodo 2:
solo cuando un curso requerido no dispone
de una oferta válida en periodo 1

VERANO:
excluido
```

Los números de grupo y horarios se mantienen según la información recolectada; no se inventan grupos ni horarios para completar información ausente. 

Los datos académicos y los datos de oferta también se trataron como fuentes conceptualmente diferentes:

```text
PLAN DE ESTUDIOS
→ semestre
→ código
→ nombre
→ créditos
→ requisitos
→ correquisitos

GUÍA DE HORARIOS
→ sede
→ grupo
→ modalidad
→ periodo
→ horario
```



---

# 5. Caso límite real encontrado

Uno de los casos especiales encontrados durante la construcción del dataset corresponde a los cursos de **Cultura y Deporte**.

Dentro del plan aparecen códigos genéricos:

```text
SE1100
SE1200
SE1400
```

Sin embargo, la oferta real de cursos utiliza códigos concretos pertenecientes a las familias:

```text
SE11xx
SE12xx
```

En lugar de asignar arbitrariamente un curso concreto a cada código genérico, se decidió conservar dentro del catálogo la oferta real disponible con sus códigos, grupos y horarios correspondientes. 

De esta forma:

```text
Plan de estudios
SE1100 / SE1200 / SE1400
        ↓

NO se fuerza una equivalencia artificial
        ↓

Dataset
SE11xx / SE12xx disponibles realmente
```

La regla académica relacionada con estas actividades indica que deben completarse tres actividades en total, dos de un tipo y una del otro. Esa lógica no fue incorporada dentro de la etapa de recolección porque corresponde a una validación académica posterior. 

Otro caso particular corresponde a Formación Humanística. Los cursos `FH` fueron excluidos del dataset normalizado por una decisión explícita de alcance y no se sustituyeron artificialmente por otras opciones. 

---

# 6. Validación académica

La evaluación académica se ejecuta después de que el catálogo y el historial fueron cargados correctamente:

```text
Catalog
        +
StudentHistory
        ↓
evaluate_catalog_eligibility()
        ↓
Catalog enriquecido
```



Para cada curso se aplican tres reglas principales.

### Curso previamente aprobado

Si el curso ya aparece en el historial:

```text
can_enroll = false
```

porque no debe matricularse nuevamente. 

### Requisitos

Para un curso no aprobado:

```text
todos los requisitos aprobados
        ↓
can_enroll = true
```

Mientras que:

```text
falta al menos un requisito
        ↓
can_enroll = false
```



### Correquisitos

Los correquisitos no aprobados no convierten automáticamente al curso en no matriculable.

En su lugar:

```text
correquisito no aprobado
        ↓
pending_corequisites
```

Estos datos quedan disponibles para que una etapa posterior pueda exigir que ambos cursos aparezcan simultáneamente dentro de una combinación. 

---

# 7. Detección de conflictos de horario

La detección de conflictos trabaja únicamente sobre el catálogo ya cargado:

```text
Catalog
   ↓
Course[]
   ↓
Group[]
   ↓
Schedule[]
   ↓
comparación
```

Dos bloques solo pueden generar un conflicto si pertenecen al mismo día. 

La condición utilizada es:

```c
inicioA < finB &&
inicioB < finA
```

Por esta razón:

```text
07:30 - 09:20
09:20 - 11:10
```

no representa un conflicto, ya que los dos intervalos únicamente comparten el instante `09:20`. 

Un grupo puede contener múltiples bloques horarios. Si cualquier combinación de bloques entre dos grupos se superpone, ambos grupos se consideran incompatibles. 

Los conflictos siempre se almacenan simétricamente:

```text
CE1101 Grupo 1
      ↓
MA1102 Grupo 2
```

y:

```text
MA1102 Grupo 2
      ↓
CE1101 Grupo 1
```



A nivel de curso:

```text
Course.has_conflict = true
```

cuando al menos uno de sus grupos presenta algún conflicto. 

---

# 8. Estructuras de datos desarrolladas

La jerarquía principal utilizada por el programa es:

```text
Catalog
└── Course[]
    ├── CodeList requirements
    ├── CodeList corequisites
    ├── CodeList pending_corequisites
    └── Group[]
        ├── Schedule[]
        └── GroupConflict[]

StudentHistory
└── CodeList approved_courses
```

La estructura interna permite reconstruir en memoria la jerarquía que originalmente se encuentra distribuida entre diferentes filas del TSV. 

---

## 8.1 `Day`

`Day` representa internamente los días permitidos:

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

El parser transforma:

```text
"LUN"
  ↓
MONDAY
```

que posteriormente se almacena en un `Schedule`. 

---

## 8.2 `Schedule`

Un `Schedule` representa un único bloque horario:

```c
typedef struct {
    Day day;
    int start_minutes;
    int end_minutes;
} Schedule;
```

Internamente, las horas se convierten a minutos desde medianoche.

Por ejemplo:

```text
LUN 07:30 - 09:20
```

se representa como:

```text
MONDAY
450
560
```



Esto simplifica las comparaciones necesarias para detectar superposiciones.

---

## 8.3 `CodeList`

`CodeList` representa una lista dinámica de códigos:

```c
typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} CodeList;
```

Se reutiliza para:

```text
Course.requirements
Course.corequisites
Course.pending_corequisites
StudentHistory.approved_courses
```



Esto evita implementar estructuras distintas para conjuntos de información conceptualmente equivalentes.

---

## 8.4 `Group`

Un `Group` representa un grupo específico de un curso.

Contiene:

```text
numero
Schedule[]
has_conflict
GroupConflict[]
```

El número del grupo se interpreta siempre junto con el código de su curso:

```text
(course.code, group.number)
```

Los conflictos detallados agregados durante KAN-12 permiten identificar no solo que existe una incompatibilidad, sino exactamente contra qué curso y grupo ocurre. 

---

## 8.5 `Course`

`Course` representa la información de un curso:

```text
code
name
credits
semester

requirements
corequisites
pending_corequisites

Group[]

has_conflict
can_enroll
```

Sus datos pueden dividirse conceptualmente en dos grupos.

Datos provenientes del catálogo:

```text
code
name
credits
semester
requirements
corequisites
groups
```

Datos calculados posteriormente:

```text
pending_corequisites
can_enroll
has_conflict
```



---

## 8.6 `Catalog`

`Catalog` representa los cursos de una única carrera:

```c
typedef struct {
    char *career_code;
    char *career_name;

    Course *courses;
    size_t course_count;
    size_t course_capacity;
} Catalog;
```

Cada ejecución trabaja con un catálogo correspondiente a una carrera determinada. 

---

## 8.7 `StudentHistory`

El historial se representa mediante:

```c
typedef struct {
    CodeList approved_courses;
} StudentHistory;
```

No duplica la información de los cursos.

Solo conserva códigos:

```text
CE1101
MA1102
FI1101
```

Toda la información adicional se consulta posteriormente desde el catálogo utilizando esos códigos. 

---

# 9. Manejo de memoria

El proyecto utiliza estructuras dinámicas debido a que no se conoce previamente cuántos cursos, grupos, horarios o códigos deberán almacenarse.

La regla general de propiedad utilizada es:

```text
Cada estructura libera
la memoria que posee directamente.
```



La jerarquía principal es:

```text
Catalog
├── strings de carrera
└── Course[]
    ├── strings del curso
    ├── CodeList
    └── Group[]
        ├── Schedule[]
        └── conflictos
```

Los códigos almacenados dentro de `CodeList` poseen memoria independiente del buffer utilizado por el parser. Esto evita que los datos sean sobrescritos cuando el parser reutiliza sus buffers. 

Toda estructura sigue el ciclo:

```text
init
 ↓
uso
 ↓
free
```

y no se utiliza estado global mutable. 

---

# 10. Formato de salida

La salida de la etapa se realiza mediante:

```text
catalogo_procesado.json
```

La estructura general es:

```json
{
  "schemaVersion": "1.0",

  "carrera": {
    "codigo": "IC",
    "nombre": "Ingeniería en Computadores"
  },

  "cursos": []
}
```



El JSON se genera únicamente después de completar todo el procesamiento:

```text
TSV
 ↓
Catalog
 ↓
StudentHistory
 ↓
Elegibilidad
 ↓
Conflictos
 ↓
Catalog procesado
 ↓
JSON
```



---

# 11. Justificación del formato de salida

Se eligió **JSON** porque la salida del programa posee una estructura naturalmente jerárquica:

```text
Curso
├── requisitos
├── correquisitos
└── grupos
    ├── horarios
    └── choques
```

JSON permite representar directamente esta relación sin duplicar información y sin obligar a la siguiente etapa a reconstruir nuevamente la estructura a partir de filas planas. 

También proporciona una separación entre implementaciones:

```text
C
 ↓
JSON
 ↓
Racket
```

Racket no necesita conocer cómo se implementaron internamente los `struct`, punteros o arreglos dinámicos de C. El archivo constituye el contrato de comunicación entre ambas etapas. 

Además de los campos mínimos solicitados, se agregaron:

```text
semestre
choques detallados
tieneChoque por grupo
correquisitosPendientes
schemaVersion
```

Estos campos conservan información que será útil para las siguientes etapas. Por ejemplo, `choques` permite identificar exactamente cuáles grupos son incompatibles, mientras que `correquisitosPendientes` permite conocer cuáles cursos deberán aparecer simultáneamente sin tener que reevaluar el historial en Racket.  

---

# 12. Esquema JSON generado

Cada curso exportado contiene:

```json
{
  "codigo": "CE2201",
  "nombre": "Estructuras de Datos",
  "creditos": 4,
  "semestre": 2,

  "requisitos": [],
  "correquisitos": [],
  "correquisitosPendientes": [],

  "grupos": [],

  "tieneChoque": false,
  "puedeMatricular": true
}
```

Cada grupo contiene:

```json
{
  "numero": 1,

  "horarios": [
    {
      "dia": "LUN",
      "inicio": "07:30",
      "fin": "09:20"
    }
  ],

  "tieneChoque": true,

  "choques": [
    {
      "curso": "MA1102",
      "grupo": 2
    }
  ]
}
```

El contrato de serialización exige además convertir la representación interna en minutos nuevamente a `HH:MM`, representar los días mediante las abreviaturas normalizadas y escapar correctamente caracteres especiales de las cadenas. 

---

# 13. Compilación

La compilación está centralizada mediante el `Makefile`.

En Windows con MinGW:

```bash
mingw32-make
```

Para realizar una compilación limpia:

```bash
mingw32-make clean
mingw32-make
```



El ejecutable generado se denomina:

```text
cemestre.exe
```



---

# 14. Ejecución

El flujo de integración recibe un catálogo y un historial:

```text
cemestre catalogo.tsv historial.txt
```

El catálogo debe cargarse antes que el historial porque este último valida sus códigos contra los cursos existentes. 

Un ejemplo de entradas sería:

```text
data/clean/computadores.tsv
data/historiales/historial.txt
```

Después del procesamiento se genera el catálogo utilizado por la siguiente etapa:

```text
catalogo_procesado.json
```

---

# 15. Pruebas

El proyecto incluye pruebas aisladas y de integración para las distintas responsabilidades.

La carga del catálogo verifica, entre otros casos:

```text
encabezados inválidos
columnas faltantes
horarios inválidos
listas mal formadas
cursos contradictorios
múltiples grupos
múltiples horarios
```

La carga del historial verifica:

```text
historial válido
duplicados
espacios externos
códigos inexistentes
archivo vacío
archivo inexistente
```



La detección de conflictos verifica casos como horarios parcialmente superpuestos, horarios idénticos, intervalos contenidos, horarios consecutivos, días diferentes, múltiples bloques y detecciones repetidas sin duplicados. 

También se realizan pruebas de integración utilizando los datasets reales:

```text
data/clean/computadores.tsv
data/clean/electronica.tsv
```

para validar que todos los conflictos almacenados correspondan efectivamente con los horarios cargados. 

---

# 16. Alcance de la Etapa 1

La primera etapa finaliza en:

```text
catalogo_procesado.json
```

El programa en C sí realiza:

```text
carga del catálogo
carga del historial
validación de datos
consolidación de cursos
consolidación de grupos
evaluación de requisitos
identificación de correquisitos pendientes
determinación de elegibilidad
detección de conflictos
serialización JSON
```

No realiza:

```text
selección del horario final
generación de combinaciones
optimización de horarios
aplicación de preferencias del estudiante
interfaz gráfica
```

Estas responsabilidades pertenecen a las siguientes etapas. La etapa 2 desarrollada en Racket recibirá el catálogo procesado y se encargará de generar y filtrar las posibles combinaciones de horario. 

---

# 17. Resumen del flujo final

```text
               FUENTES INSTITUCIONALES
                         ↓
                Recolección manual
                         ↓
             Limpieza y normalización
                         ↓
                   catalogo.tsv
                         │
                         │
historial.txt ───────────┤
                         ↓
                    Programa C
                         ↓
                ┌────────────────┐
                │ Catalog        │
                │ StudentHistory │
                └────────────────┘
                         │
             ┌───────────┴───────────┐
             ↓                       ↓
      Elegibilidad académica    Conflictos horarios
             │                       │
             └───────────┬───────────┘
                         ↓
                Catálogo procesado
                         ↓
               Serialización JSON
                         ↓
              catalogo_procesado.json
                         ↓
                       Racket
```

Esta arquitectura mantiene separadas la entrada externa, la representación interna, la lógica de procesamiento y la salida utilizada entre etapas. 
