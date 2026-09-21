# Contrato KAN-9: Carga y validación del historial académico

## 1. Propósito

Este documento define las reglas de carga y validación del historial académico
implementadas en KAN-9.

El módulo recibe un archivo con los códigos de los cursos aprobados por un
estudiante y construye la estructura:

```text
StudentHistory
└── approved_courses
```

El historial se valida utilizando el `Catalog` previamente cargado y validado
por el sistema.

KAN-9 no evalúa todavía requisitos, correquisitos ni elegibilidad académica.


## 2. Archivo de entrada

El historial se recibe mediante un archivo de texto.

Cada línea no vacía representa un código de curso aprobado.

Ejemplo:

```text
CE1101
MA1102
FI1101
```

El archivo no utiliza encabezado.


## 3. Tratamiento de códigos

Los archivos de entrada deben entregarse previamente normalizados según las
reglas establecidas para el conjunto de datos.

El programa no corrige códigos automáticamente.

Durante la carga únicamente se eliminan espacios accidentales al inicio y al
final de cada línea.

Ejemplo válido:

```text
   CE1101
```

se interpreta como:

```text
CE1101
```

No se realizan transformaciones como:

```text
ce1101 -> CE1101
CE 1101 -> CE1101
```

La comparación contra el catálogo es exacta.

Si el código recibido no coincide con un código existente en el catálogo, se
considera una inconsistencia.


## 4. Validación contra el catálogo

Cada código leído debe existir dentro del `Catalog` previamente construido.

La búsqueda utiliza:

```text
find_course_by_code
```

Flujo:

```text
código del historial
        ↓
buscar en Catalog
        ↓
existe      -> continuar
no existe   -> ERROR_INVALID_FORMAT
```

KAN-9 únicamente consulta el catálogo.

No crea, modifica ni elimina cursos.


## 5. Duplicados

Un curso aprobado no puede aparecer más de una vez dentro del historial.

La validación se realiza utilizando:

```text
code_list_contains
```

Ejemplo inválido:

```text
CE1101
MA1102
CE1101
```

También se considera duplicado:

```text
CE1101
   CE1101
```

debido a que los espacios externos se eliminan antes de realizar la
comparación.

Un duplicado produce:

```text
ERROR_INVALID_FORMAT
```


## 6. Archivo vacío

Un archivo vacío se maneja explícitamente como un historial sin cursos
aprobados.

Por lo tanto:

```text
archivo vacío
        ↓
StudentHistory.approved_courses.count = 0
        ↓
SUCCESS
```

Las líneas vacías o compuestas únicamente por espacios también son ignoradas.


## 7. Almacenamiento

Los códigos válidos se almacenan en:

```text
StudentHistory.approved_courses
```

mediante:

```text
code_list_add
```

`CodeList` crea una copia propia del código recibido.

Por lo tanto, el historial no depende del buffer temporal utilizado durante la
lectura del archivo.

Ejemplo:

```text
historial.txt
├── CE1101
├── MA1102
└── FI1101

        ↓

StudentHistory
└── approved_courses
    ├── CE1101
    ├── MA1102
    └── FI1101
```


## 8. Errores

KAN-9 utiliza los estados comunes definidos en `constants.h`.

```text
SUCCESS
ERROR_ARGS
ERROR_FILE_NOT_FOUND
ERROR_INVALID_FORMAT
ERROR_MEMORY
ERROR_READ
```

Casos principales:

```text
argumentos inválidos         -> ERROR_ARGS
archivo inexistente          -> ERROR_FILE_NOT_FOUND
duplicado                    -> ERROR_INVALID_FORMAT
código inexistente           -> ERROR_INVALID_FORMAT
línea fuera del formato      -> ERROR_INVALID_FORMAT
fallo de memoria             -> ERROR_MEMORY
fallo durante lectura        -> ERROR_READ
```


## 9. Manejo de memoria

Si ocurre un error después de haber cargado parte del historial:

```text
detener lectura
      ↓
cerrar archivo
      ↓
student_history_free()
      ↓
devolver Status
```

No se conserva un historial parcialmente válido.

Cuando:

```text
parse_student_history()
```

retorna:

```text
SUCCESS
```

quien llamó la función es responsable de liberar posteriormente el historial
mediante:

```text
student_history_free()
```

La liberación sigue:

```text
StudentHistory
        ↓
approved_courses
        ↓
CodeList
        ↓
cada código
        ↓
items
```


## 10. Separación de responsabilidades

### history_parser

Se encarga de:

- abrir y leer el archivo de historial;
- eliminar espacios externos;
- ignorar líneas vacías;
- detectar códigos duplicados;
- verificar que cada código exista en el catálogo;
- almacenar los códigos aprobados;
- manejar errores durante la carga;
- limpiar información parcial cuando ocurre un error.

### Catalog

Se utiliza únicamente para comprobar la existencia de los códigos mediante:

```text
find_course_by_code
```

KAN-9 no modifica el catálogo.

### CodeList

Se utiliza para:

```text
almacenar códigos
detectar códigos existentes
liberar memoria
```


## 11. Flujo de datos

```text
historial.txt
      ↓
history_parser
      ↓
leer línea
      ↓
eliminar espacios externos
      ↓
¿línea vacía?
├── sí -> continuar
└── no
      ↓
¿duplicado?
├── sí -> ERROR_INVALID_FORMAT
└── no
      ↓
¿existe en Catalog?
├── no -> ERROR_INVALID_FORMAT
└── sí
      ↓
code_list_add
      ↓
StudentHistory.approved_courses
```


## 12. Pruebas

KAN-9 incluye pruebas para verificar:

```text
historial válido
almacenamiento de múltiples cursos
eliminación de espacios externos
cursos duplicados
códigos inexistentes
archivo vacío
archivo compuesto únicamente por espacios
código inconsistente sin corrección automática
archivo inexistente
liberación de memoria
```

La suite de pruebas debe retornar:

```text
0
```

cuando todos los casos pasan y un valor distinto de cero cuando existe al
menos un fallo.


## 13. Fuera del alcance

KAN-9 no realiza:

- corrección automática de códigos;
- modificación de mayúsculas o minúsculas;
- creación o modificación de cursos;
- carga del catálogo;
- validación de requisitos académicos;
- validación de correquisitos académicos;
- determinación de `can_enroll`;
- detección de conflictos de horario;
- generación de combinaciones;
- exportación JSON;
- lógica de Racket;
- lógica de Prolog;
- interfaz Java.