
# Contrato KAN-13: Evaluación de Requisitos, Correquisitos y Elegibilidad Académica

## 1. Propósito

Este documento define la especificación técnica de la lógica de negocio académica para KAN-13 en el sistema **CEmestre**.

El objetivo de este módulo es cruzar la información de un `Catalog` previamente cargado y validado contra la información del `StudentHistory` del alumno para determinar la elegibilidad académica de cada curso (`can_enroll`) y construir la lista de correquisitos pendientes (`pending_corequisites`)[cite: 2, 3, 5, 6].

El flujo de procesamiento en memoria se ubica de la siguiente manera[cite: 3, 6]:

```text
catalogo.tsv + historial.txt
            ↓
  parse_catalog() + parse_student_history()
            ↓
  Catalog  +  StudentHistory
            ↓
  evaluate_catalog_eligibility()
            ↓
  Catalog enriquecido (Elegibilidad)

```

---

## 2. Operación Pública e Implementación del Módulo

La API del módulo se define en `include/eligibility.h` y `src/eligibility.c`, ofreciendo las siguientes funciones:

### 2.1 `evaluate_course_eligibility`

```c
void evaluate_course_eligibility(Course *course, const StudentHistory *history);

```

Determina la elegibilidad e identifica los correquisitos pendientes para un curso individual.

### 2.2 `evaluate_catalog_eligibility`

```c
Status evaluate_catalog_eligibility(Catalog *catalog, const StudentHistory *history);

```

Itera sobre todos los cursos del catálogo y delega la evaluación a `evaluate_course_eligibility`.

---

## 3. Reglas de Negocio Académicas

### 3.1 Cursos Ya Aprobados

* **Regla de Exclusión:** Si el código del curso ya se encuentra dentro de `history->approved_courses`, el estudiante **no puede volver a matricularlo** (`course->can_enroll = false`). La función finaliza la evaluación de dicho curso inmediatamente.



### 3.2 Evaluación de Requisitos (`can_enroll`)

1. **Punto de partida:** Todo curso no aprobado previamente inicia con `course->can_enroll = true` por defecto.


2. **Cursos sin requisitos:** Un curso con `requirements.count == 0` se mantiene como matriculable (`can_enroll = true`).


3. **Cursos con requisitos:** Se recorre la lista `course->requirements` utilizando `code_list_contains()`:


* **Todos aprobados:** Si todos los códigos están en el historial $\rightarrow$ `course->can_enroll = true`.


* **Falta al menos uno:** Si un solo requisito da `0` en `code_list_contains()` $\rightarrow$ `course->can_enroll = false` y se interrumpe la revisión del bucle (`break`).





### 3.3 Evaluación de Correquisitos (`pending_corequisites`)

1. **Independencia de `can_enroll`:** Los correquisitos no aprobados **NO** modifican `can_enroll`. La elegibilidad representa únicamente la suficiencia de requisitos previos.


2. **Reinicio de estado:** Antes de evaluar, se libera y reinicia la lista `course->pending_corequisites` (`code_list_free` y `code_list_init`) para prevenir duplicados en evaluaciones consecutivas.


3. **Cursos con correquisitos:** Se recorre la lista `course->corequisites`:


* **Correquisito aprobado:** Si `code_list_contains()` retorna `1`, se considera satisfecho y **NO** se agrega a pendientes.


* **Correquisito no aprobado:** Si retorna `0`, se añade el código a `course->pending_corequisites` mediante `code_list_add()`.





---

## 4. Invariantes y Restricciones del Modelo

1. **Lectura estricta del historial:** La estructura `StudentHistory` debe tratarse como **de solo lectura** (`const StudentHistory *history`). KAN-13 no añade ni elimina elementos de `approved_courses`.


2. **Aislamiento de Choques:** La existencia de un choque de horarios (`has_conflict = true`) es conceptualmente independiente de la elegibilidad académica (`can_enroll = true`).


3. **Defensiva de Punteros:** Si se reciben punteros `NULL` en `evaluate_catalog_eligibility`, la función retorna `ERROR_ARGS` inmediatamente sin procesar el catálogo.



---

## 5. Casos de Prueba Mínimos (Unidad e Integración)

Las pruebas unitarias del módulo en `tests/test_eligibility.c` deberán verificar obligatoriamente:

| # | Caso de Prueba | Entrada / Condición | Resultado Esperado |
| --- | --- | --- | --- |
| **1** | Curso ya aprobado | `CE1101` presente en `approved_courses` | `can_enroll = false`<br> |
| **2** | Curso sin requisitos | `requirements` vacío | `can_enroll = true`, `pending_corequisites` vacío

 |
| **3** | Todos los requisitos aprobados | `CE1101` en historial | `CE2201` tiene `can_enroll = true`<br> |
| **4** | Requisito faltante | `CE1101` ausente | `can_enroll = false`<br> |
| **5** | Múltiples requisitos, falta uno | Requiere `A` y `B`, solo `A` en historial | `can_enroll = false`<br> |
| **6** | Correquisito aprobado | Correquisito `MA1102` en historial | `pending_corequisites` queda vacío, `can_enroll = true`<br> |
| **7** | Correquisito pendiente | Correquisito `MA1102` ausente | `MA1102` se agrega a `pending_corequisites`, `can_enroll = true`<br> |
| **8** | Historial inalterado | Comparación previa y posterior | `history->approved_courses.count` permanece idéntico

 |
| **9** | Inmunidad a reevaluación | Ejecutar `evaluate_catalog_eligibility` dos veces seguidas | `pending_corequisites` no acumula duplicados

 |

---

## 6. Manejo de Errores y Códigos de Estado

El módulo utiliza los códigos enumerados en `include/constants.h`:

* `SUCCESS` (`0`): Evaluación completada correctamente en todo el catálogo.


* `ERROR_ARGS` (`1`): Puntero `catalog == NULL` o `history == NULL`.



---

## 7. Separación de Responsabilidades

### KAN-13 SÍ se encarga de:

* Marcar como no matriculable un curso que ya fue aprobado.


* Recorrer `requirements` y evaluar contra el historial para asignar `can_enroll`.


* Recorrer `corequisites` y poblar `pending_corequisites` para los que no estén aprobados.


* Reiniciar `pending_corequisites` antes de cada evaluación.



### KAN-13 NO se encarga de:

* Leer o validar archivos TSV o historiales (responsabilidad de KAN-7 y KAN-9).


* Detectar choques de horario o modificar `Group.has_conflict` (responsabilidad de KAN-12).


* Generar combinaciones, recomendar horarios o exportar a JSON (responsabilidad de la serialización y de Racket).
