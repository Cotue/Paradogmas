## 1. Ejemplos

### 1.1 Un curso con dos grupos

Entrada TSV:

```text
IC	1	CE1101	Introducción a la Programación	4	-	-	1	LUN	07:30	09:20
IC	1	CE1101	Introducción a la Programación	4	-	-	1	JUE	07:30	09:20
IC	1	CE1101	Introducción a la Programación	4	-	-	2	MAR	13:00	14:50
IC	1	CE1101	Introducción a la Programación	4	-	-	2	VIE	13:00	14:50
```

Representación conceptual:

```text
CE1101
│
├── Grupo 1
│   ├── LUN 07:30 - 09:20
│   └── JUE 07:30 - 09:20
│
└── Grupo 2
    ├── MAR 13:00 - 14:50
    └── VIE 13:00 - 14:50
```


### 1.2 Curso con requisito

Curso:

```text
CE2201
```

Requisito:

```text
CE1101
```

Historial:

```text
CE1101
MA1102
```

Resultado:

```text
CE1101 pertenece al historial
↓
requisito satisfecho
↓
CE2201 cumple requisitos
```


### 1.3 Curso con requisito faltante

Curso:

```text
CE2201
```

Requisitos:

```text
CE1101
MA1102
```

Historial:

```text
CE1101
```

Resultado:

```text
CE1101 -> encontrado
MA1102 -> no encontrado

puedeMatricular = false
```


### 1.4 Conflicto entre grupos

```text
CE1101 Grupo 1
LUN 08:00 - 10:00

MA1102 Grupo 2
LUN 09:00 - 11:00
```

Resultado:

```text
CE1101 Grupo 1
    ↕
  choque
    ↕
MA1102 Grupo 2
```

Esto no implica que CE1101 sea académicamente no matriculable.

Únicamente indica que esa combinación particular de grupos es incompatible.

### 1.5 Correquisito ya aprobado

Curso:

```text
Curso A
```

Correquisito:

```text
Curso B
```

Historial:

```text
Curso B
```

Evaluación:

```text
Curso B -> encontrado en historial
```

Resultado:

```text
correquisitosPendientes = []
puedeMatricular = true
```

El estudiante no necesita matricular nuevamente Curso B.

### 1.6 Correquisito pendiente

Curso:

```text
Curso A
```

Correquisito:

```text
Curso B
```

Historial:

```text
Curso B -> no aparece
```

Resultado:

```text
puedeMatricular = true
```

pero:

```text
correquisitosPendientes:
Curso B
```

Por lo tanto:

```text
Curso A + Curso B
```

deberán matricularse simultáneamente.

La selección específica de grupos compatibles entre ambos cursos será
realizada por etapas posteriores utilizando también la información de
choques producida por C.