## 1. Archivo de entrada del historial

### 1.1 Formato

El historial utilizará un archivo de texto plano.

Nombre conceptual:

```text
historial.txt
```

Cada línea contendrá exactamente un código de curso aprobado.


### 1.2 Ejemplo

```text
CE1101
MA1102
FI1101
```


### 1.3 Reglas

Los códigos:

- no pueden estar duplicados;
- no pueden estar vacíos;
- deben utilizar el mismo formato que los códigos del catálogo;
- deben corresponder a cursos reconocidos;
- podrán ser normalizados eliminando espacios innecesarios al inicio o al final.


### 1.4 Relación con el catálogo

El historial no modifica la definición de los cursos.

Su función es permitir que el programa determine cuáles requisitos y
correquisitos ya fueron aprobados por el estudiante.

La relación entre ambas estructuras se realiza utilizando los códigos de
los cursos.

Ejemplo:

```text
Curso CE2201

Requisitos:
CE1101

Correquisitos:
MA1102
```

Historial:

```text
CE1101
```

Evaluación de requisitos:

```text
CE1101 -> aprobado
```

Por lo tanto:

```text
requisitos cumplidos
```

Evaluación de correquisitos:

```text
MA1102 -> no aprobado
```

Esto no hace que CE2201 sea automáticamente no matriculable.

En cambio:

```text
MA1102
```

se registra como correquisito pendiente y deberá matricularse
simultáneamente con CE2201.

Resultado conceptual:

```text
puedeMatricular = true

correquisitosPendientes:
MA1102
```