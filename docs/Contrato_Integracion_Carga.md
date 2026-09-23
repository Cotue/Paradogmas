v1.0

# Contrato KAN-11: Integración de carga de catálogo e historial

## 1. Propósito

Este documento define el comportamiento esperado para la integración entre el cargador del catálogo de cursos y el cargador del historial académico del estudiante.

KAN-11 tiene como objetivo comprobar que ambos módulos, desarrollados de forma independiente, pueden ejecutarse conjuntamente antes de implementar lógica de negocio adicional.

El flujo general es:

```text
catalogo.tsv
     ↓
carga y validación del catálogo
     ↓
Catalog
     ↓
historial.txt
     ↓
carga y validación del historial
     ↓
StudentHistory
```

KAN-11 no redefine las reglas internas de validación de catálogo ni historial.

---

## 2. Entrada del ejecutable

El ejecutable recibe exactamente dos archivos:

```text
cemestre catalogo.tsv historial.txt
```

Por lo tanto:

```text
argc == 3
```

Si la cantidad de argumentos es incorrecta, el programa debe retornar:

```text
ERROR_ARGS
```

y mostrar el uso esperado.

---

## 3. Orden de ejecución

La carga debe realizarse obligatoriamente en este orden:

```text
1. inicializar Catalog
2. inicializar StudentHistory
3. cargar catálogo
4. validar catálogo
5. construir estructuras del catálogo
6. cargar historial
7. validar códigos del historial contra el catálogo
8. informar éxito
9. liberar StudentHistory
10. liberar Catalog
```

El historial no debe cargarse si el catálogo no pudo cargarse correctamente.

---

## 4. Carga del catálogo

KAN-11 utiliza el parser de catálogo existente:

```c
Status parse_catalog(
    const char *file_path,
    Catalog *catalog
);
```

Este módulo ya es responsable de:

- abrir el archivo;
- validar su formato;
- validar los campos;
- validar horarios;
- consolidar cursos y grupos;
- construir las estructuras correspondientes.

KAN-11 no debe duplicar estas validaciones.

Si `parse_catalog()` retorna un estado distinto de:

```text
SUCCESS
```

el flujo debe detenerse y retornar ese mismo estado.

---

## 5. Carga del historial

Una vez cargado correctamente el catálogo, se utiliza:

```c
Status parse_student_history(
    const char *file_path,
    Catalog *catalog,
    StudentHistory *history
);
```

Antes de llamar al parser, la estructura debe haber sido inicializada mediante:

```c
student_history_init(&history);
```

El parser del historial ya es responsable de:

- abrir el archivo;
- leer códigos;
- eliminar espacios externos;
- detectar duplicados;
- validar que cada código exista en el catálogo;
- almacenar los cursos aprobados en `StudentHistory`.

KAN-11 no debe duplicar estas validaciones.

Si la carga del historial falla, debe retornarse el estado producido por dicho módulo.

---

## 6. Resultado exitoso

La integración se considera exitosa únicamente cuando:

```text
parse_catalog(...) == SUCCESS
```

y:

```text
parse_student_history(...) == SUCCESS
```

En ese caso el programa informa:

```text
Carga exitosa.
```

La existencia de una carga exitosa no implica todavía:

- elegibilidad académica;
- ausencia de conflictos;
- recomendación de cursos;
- generación de horarios.

---

## 7. Manejo de memoria

Las estructuras utilizadas deben inicializarse antes de su uso:

```c
catalog_init(&catalog);
student_history_init(&history);
```

Al finalizar el flujo, tanto en éxito como después de un error, deben liberarse correctamente:

```c
student_history_free(&history);
catalog_free(&catalog);
```

KAN-11 debe evitar fugas de memoria durante:

```text
carga exitosa
error de catálogo
error de historial
```

---

## 8. Pruebas de integración

KAN-11 utiliza fixtures específicos dentro de:

```text
tests/fixtures/integration/
```

Los archivos definidos son:

```text
catalog_valid.tsv
catalog_malformed.tsv
catalog_invalid_schedule.tsv
history_valid.txt
history_duplicate.txt
history_unknown.txt
```

La prueba principal se encuentra en:

```text
tests/test_integration.c
```

---

## 9. Casos de prueba

### 9.1 Entrada válida

Se utiliza:

```text
catalog_valid.tsv
history_valid.txt
```

Resultado esperado:

```text
SUCCESS
```

Además, las estructuras deben quedar correctamente pobladas.

En el fixture actual se espera:

```text
catalog.course_count == 2
history.approved_courses.count == 2
```

---

### 9.2 Catálogo mal formado

Se utiliza:

```text
catalog_malformed.tsv
```

El archivo contiene una fila con una cantidad incorrecta de columnas.

Resultado esperado:

```text
ERROR_INVALID_FORMAT
```

El historial no debe procesarse.

---

### 9.3 Horario inválido

Se utiliza:

```text
catalog_invalid_schedule.tsv
```

Contiene un bloque donde:

```text
inicio >= fin
```

Resultado esperado:

```text
ERROR_INVALID_FORMAT
```

La detección del error corresponde al parser de catálogo existente.

---

### 9.4 Historial duplicado

Se utiliza:

```text
history_duplicate.txt
```

Ejemplo:

```text
CE1101
CE1101
```

Resultado esperado:

```text
ERROR_INVALID_FORMAT
```

La detección corresponde al parser de historial existente.

---

### 9.5 Código desconocido

Se utiliza:

```text
history_unknown.txt
```

Incluye un código que no existe dentro del catálogo cargado.

Resultado esperado:

```text
ERROR_INVALID_FORMAT
```

La validación corresponde al parser de historial existente.

---

## 10. Separación de responsabilidades

KAN-11 se encarga de:

- recibir las rutas del catálogo y del historial;
- inicializar las estructuras necesarias;
- ejecutar primero el parser del catálogo;
- ejecutar después el parser del historial;
- conservar los estados de error producidos por ambos módulos;
- confirmar cuando ambas cargas finalizan exitosamente;
- liberar correctamente las estructuras;
- comprobar mediante pruebas que ambos módulos funcionan juntos.

KAN-11 no se encarga de:

- redefinir la validación del catálogo;
- redefinir la validación del historial;
- modificar `Catalog`;
- modificar `StudentHistory`;
- detectar conflictos de horarios;
- modificar `Group.has_conflict`;
- modificar `Course.has_conflict`;
- comprobar requisitos;
- comprobar correquisitos;
- determinar `can_enroll`;
- generar combinaciones;
- generar horarios;
- recomendar matrícula;
- aplicar preferencias;
- exportar resultados finales.

---

## 11. Relación con otros KAN

### KAN-7

Proporciona la carga y validación del catálogo.

KAN-11 reutiliza:

```text
parse_catalog()
```

sin modificar su responsabilidad.

### KAN-9

Proporciona la carga y validación del historial académico.

KAN-11 reutiliza:

```text
parse_student_history()
```

sin modificar su responsabilidad.

### KAN-12

La detección de conflictos entre grupos se desarrolla de forma independiente.

KAN-11 no debe ejecutar ni implementar esa lógica.

### KAN-13

La evaluación de requisitos, correquisitos y elegibilidad académica se desarrolla de forma independiente.

KAN-11 únicamente deja disponibles:

```text
Catalog
StudentHistory
```

para que etapas posteriores puedan utilizarlos.

---

## 12. Compilación y pruebas

El proyecto permite compilar manualmente los ejecutables necesarios.

Ejemplo para la prueba de integración:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude tests/test_integration.c src/catalog_parser.c src/history_parser.c src/validator.c src/models/catalog.c src/models/code_list.c src/models/course.c src/models/group.c src/models/schedule.c src/models/student_history.c -o test_integration.exe
```

Ejecución:

```bash
./test_integration.exe
```

Resultado esperado:

```text
Entrada valida                                 : PASS
Catalogo mal formado                           : PASS
Horario invalido (catalogo)                    : PASS
Historial duplicado                            : PASS
Codigo desconocido (historial)                 : PASS

Fallos totales: 0
```

El ejecutable principal puede compilarse mediante:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude src/main.c src/catalog_parser.c src/history_parser.c src/validator.c src/models/catalog.c src/models/code_list.c src/models/course.c src/models/group.c src/models/schedule.c src/models/student_history.c -o cemestre.exe
```

Ejemplo de ejecución válida:

```bash
./cemestre.exe tests/fixtures/integration/catalog_valid.tsv tests/fixtures/integration/history_valid.txt
```

Resultado esperado:

```text
Carga exitosa.
```

---

## 13. Resultado esperado

Al finalizar KAN-11 debe existir un flujo funcional:

```text
catalogo.tsv
      ↓
Catalog
      ↓
historial.txt
      ↓
StudentHistory
      ↓
Carga exitosa
```

Además, deben existir:

```text
src/main.c
tests/test_integration.c

tests/fixtures/integration/
├── catalog_valid.tsv
├── catalog_malformed.tsv
├── catalog_invalid_schedule.tsv
├── history_valid.txt
├── history_duplicate.txt
└── history_unknown.txt
```

Las pruebas de integración deben finalizar con:

```text
Fallos totales: 0
```

KAN-11 deja preparados `Catalog` y `StudentHistory` para las etapas posteriores del sistema, sin implementar todavía lógica de negocio.
