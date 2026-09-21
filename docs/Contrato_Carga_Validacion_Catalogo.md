# Contrato KAN-7: Carga y validación del catálogo

## 1. Propósito

Este documento define las reglas de carga y validación implementadas en KAN-7.

La estructura conceptual del catálogo, cursos, grupos, horarios, requisitos y
correquisitos se mantiene definida en:

```text
docs/Contrato_Catalogo.md
```

Este contrato no reemplaza `Contrato_Catalogo.md`.

Su función es documentar cómo se valida y construye el catálogo desde el archivo
TSV antes de continuar con otras etapas del proyecto.


## 2. Archivo de entrada

El catálogo se recibe en formato TSV.

La primera línea no vacía debe contener exactamente:

```text
carrera	semestre	codigo	nombre	creditos	requisitos	correquisitos	grupo	dia	inicio	fin
```

Cada fila de datos debe tener exactamente 11 columnas.

Dos tabulaciones consecutivas representan un campo vacío y no deben desplazar
las columnas siguientes.


## 3. Validaciones

### 3.1 Carrera

El campo `carrera`:

- no puede estar vacío;
- no puede ser `-`;
- debe respetar `MAX_CAREER_CODE_LENGTH`;
- debe mantenerse igual en todas las filas del archivo.

La primera fila válida establece `catalog->career_code`.


### 3.2 Curso

`codigo` y `nombre`:

- son obligatorios;
- no pueden estar vacíos;
- no pueden ser `-`;
- deben respetar los límites definidos en `constants.h`.


### 3.3 Semestre, créditos y grupo

Antes de convertir valores numéricos se validan mediante:

```text
validate_semester_format
validate_credits_format
validate_group_format
```

Las conversiones con `atoi()` se realizan únicamente después de una validación
exitosa.


### 3.4 Día y horario

El día se valida mediante:

```text
validate_day_format
```

y se convierte al tipo `Day` mediante:

```text
parse_day_string
```

Los horarios deben:

- utilizar formato `HH:MM`;
- representar horas válidas;
- cumplir `inicio < fin`.

Después de validarlos se convierten a minutos desde medianoche.


## 4. Requisitos y correquisitos

Los códigos se separan mediante:

```text
;
```

El valor:

```text
-
```

representa una lista vacía.

No se almacena `-` dentro de `CodeList`.

Un campo vacío es inválido.

También son inválidos formatos como:

```text
;A
A;
A;;B
```

En KAN-7 solamente se cargan los códigos.

No se evalúa todavía si el estudiante cumple requisitos o correquisitos.


## 5. Construcción del catálogo

Una fila TSV representa un bloque horario.

Por lo tanto, varias filas pueden corresponder al mismo curso o al mismo grupo.

La construcción sigue:

```text
Catalog
└── Course
    └── Group
        └── Schedule
```


### 5.1 Cursos

El curso se busca mediante su código.

Si no existe:

```text
crear Course
```

Si ya existe:

```text
reutilizar Course
```

No se deben crear cursos duplicados.


### 5.2 Grupos

El grupo se busca dentro del curso correspondiente.

Si no existe:

```text
crear Group
```

Si ya existe:

```text
reutilizar Group
```

El número de grupo no es un identificador global.

La identificación conceptual continúa siendo:

```text
(codigoCurso, numeroGrupo)
```


### 5.3 Horarios

Cada fila válida agrega un bloque `Schedule` al grupo correspondiente.

Un grupo puede contener múltiples horarios.


## 6. Filas contradictorias

Cuando un curso aparece en varias filas deben mantenerse iguales:

```text
carrera
semestre
codigo
nombre
creditos
requisitos
correquisitos
```

Los requisitos y correquisitos se comparan independientemente del orden.

Por ejemplo:

```text
A;B
```

y:

```text
B;A
```

se consideran equivalentes.

Si existe una contradicción:

```text
ERROR_INVALID_FORMAT
```

y se detiene la carga.


## 7. Errores

KAN-7 utiliza los estados definidos en `constants.h`.

```text
SUCCESS
ERROR_FILE_NOT_FOUND
ERROR_INVALID_FORMAT
ERROR_MEMORY
```

Casos principales:

```text
archivo inexistente        -> ERROR_FILE_NOT_FOUND
formato o datos inválidos  -> ERROR_INVALID_FORMAT
fallo de memoria           -> ERROR_MEMORY
```


## 8. Manejo de memoria

Si ocurre un error durante la carga:

```text
detener lectura
      ↓
cerrar archivo
      ↓
catalog_free()
      ↓
devolver Status
```

No se continúa con el análisis después de un error.

Cuando `parse_catalog()` retorna `SUCCESS`, quien llamó la función es
responsable de liberar el catálogo cuando deje de utilizarlo.


## 9. Separación de responsabilidades

### catalog_parser

Se encarga de:

- abrir y leer el TSV;
- separar columnas;
- coordinar validaciones;
- construir y consolidar el catálogo;
- detectar contradicciones;
- manejar errores durante la carga.


### validator

Se encarga de validar:

- cantidad de columnas;
- código y nombre;
- semestre;
- créditos;
- grupo;
- día;
- horario.

El módulo de validación no construye el catálogo.


## 10. Pruebas

KAN-7 incluye pruebas para verificar, entre otros:

```text
archivo inexistente
encabezado inválido
menos o más de 11 columnas
carrera inválida o contradictoria
curso sin código o nombre
semestre inválido
créditos inválidos
grupo inválido
día inválido
horario inválido
requisitos/correquisitos vacíos o mal formados
filas contradictorias
múltiples grupos
múltiples horarios
```

Las suites de pruebas deben retornar:

```text
0
```

cuando todos los casos pasan y un valor distinto de cero cuando existe al
menos un fallo.


## 11. Fuera del alcance

KAN-7 no realiza:

- detección de conflictos entre cursos;
- evaluación académica de requisitos;
- evaluación académica de correquisitos;
- determinación de elegibilidad;
- generación de combinaciones de horarios;
- exportación JSON;
- lógica de Racket;
- lógica de Prolog;
- interfaz Java.
