## 1. Decisiones de diseño

### 1.1 Separar fuente original y catálogo normalizado

La información recolectada desde la Guía de Horarios o TEC Digital no será
consumida directamente por la lógica principal.

Primero se transformará al formato TSV definido por el proyecto.

Esto permite mantener independiente la lógica de C de posibles cambios en
la estructura de las fuentes institucionales.


### 1.2 Uso de TSV para el catálogo

TSV fue seleccionado porque:

- es sencillo de leer utilizando C;
- no requiere bibliotecas externas;
- puede generarse desde una hoja de cálculo;
- permite inspeccionar fácilmente los datos;
- facilita la detección de columnas faltantes;
- reduce problemas con comas presentes en nombres de cursos.

Cada fila representa un bloque horario.

La jerarquía real se reconstruye posteriormente dentro del programa.


### 1.3 Uso de TXT para el historial

El historial únicamente necesita almacenar códigos de cursos aprobados.

Por esta razón no se requiere un formato jerárquico.

Un código por línea permite una representación sencilla y fácil de validar.


### 1.4 Uso de JSON para la salida

La salida posee una estructura jerárquica:

```text
Curso
├── requisitos
├── correquisitos
└── grupos
    ├── horarios
    └── choques
```

JSON permite representar esta estructura de forma natural y sin duplicar
innecesariamente información.

Además, permite mantener la comunicación entre C y las siguientes etapas
independiente del lenguaje utilizado internamente.


### 1.5 Identificación compuesta de grupos

Un número de grupo puede repetirse entre diferentes cursos.

Por este motivo:

```text
grupo 1
```

no constituye un identificador suficiente.

Los grupos serán identificados conceptualmente mediante:

```text
(códigoCurso, númeroGrupo)
```


### 1.6 Separación entre elegibilidad y conflictos

El sistema diferencia:

```text
puedeMatricular
```

de:

```text
tieneChoque
```

`puedeMatricular` representa la condición académica del estudiante respecto
al curso.

`tieneChoque` representa la existencia de una superposición horaria.

Un curso puede tener:

```text
puedeMatricular = true
tieneChoque = true
```

sin existir contradicción.


### 1.7 Tratamiento de requisitos y correquisitos

Los requisitos y correquisitos se mantienen como relaciones independientes
porque poseen reglas académicas diferentes.

Un requisito debe haber sido aprobado previamente por el estudiante.

Por lo tanto, si falta al menos un requisito:

```text
puedeMatricular = false
```

Un correquisito, en cambio, se considera válido cuando:

- ya fue aprobado previamente; o
- puede matricularse simultáneamente con el curso que lo requiere.

Cuando un correquisito todavía no aparece dentro del historial, su código se
almacenará dentro de:

```text
correquisitosPendientes
```

Ejemplo:

```text
Curso A

Requisito:
Curso X

Correquisito:
Curso B
```

Si el historial contiene:

```text
Curso X
```

pero no:

```text
Curso B
```

el resultado será:

```text
puedeMatricular = true

correquisitosPendientes:
Curso B
```

Esto significa que el estudiante posee los requisitos académicos previos
necesarios para Curso A, pero deberá matricular Curso B simultáneamente.

Esta información se conserva en el archivo de salida para que las etapas
posteriores puedan mantener la relación de correquisito al generar
combinaciones de matrícula.