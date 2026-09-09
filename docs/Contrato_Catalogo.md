v1.0
## 1. Propósito

Este documento define la información que recibe y produce la primera etapa
del proyecto. Su objetivo es establecer una representación de datos
de la implementación en C y clara para ser utilizada por las siguientes etapas del proyecto.

Su responsabilidad es cargar el catálogo correspondiente a una carrera,
cargar el historial académico de un estudiante, validar requisitos y
correquisitos, detectar conflictos entre grupos y producir un catálogo
procesado que pueda ser utilizado por la siguiente etapa del proyecto.

El contrato de datos busca mantener separadas:

- la fuente original de los datos;
- la representación normalizada utilizada por C;
- la representación interna utilizada por el programa;
- la información exportada para las siguientes etapas.

Las estructuras internas implementadas en C no forman parte del contrato.

Por ejemplo, C podrá utilizar `struct`, punteros, arreglos dinámicos u otras
estructuras internas, pero Racket no deberá conocer ninguno de esos detalles.

La comunicación entre las etapas se realizará exclusivamente mediante los
archivos definidos en este documento.


## 2. Entidades del sistema

### 2.1 Catálogo

El catálogo representa el conjunto de cursos correspondientes a una carrera.

Durante una ejecución normal, el catálogo se considera un conjunto de datos
estable y reutilizable.

Por ejemplo, puede existir:

```text
computadores.tsv
```

y utilizarse posteriormente con diferentes historiales de estudiantes.

El catálogo no estará definido directamente dentro del código fuente.

El programa deberá cargarlo desde un archivo previamente recolectado,
limpiado y normalizado.

Conceptualmente:

```text
Catálogo
│
├── Carrera
│
└── Cursos
     ├── Curso 1
     ├── Curso 2
     ├── Curso 3
     └── ...
```

Para incorporar una nueva carrera no será necesario modificar la lógica
principal del programa, siempre que la información de dicha carrera sea
transformada previamente al formato de catálogo definido en este contrato.

La información oficial puede provenir, por ejemplo, de la Guía de Horarios
del TEC o de TEC Digital.

La fuente original podrá encontrarse en HTML u otro formato.

Sin embargo:

```text
HTML ≠ catálogo utilizado directamente por C
```

La información deberá ser recolectada y normalizada previamente:

```text
Fuente oficial
      ↓
Recolección
      ↓
Limpieza y normalización
      ↓
catalogo.tsv
```

En esta etapa la transformación desde la fuente original hacia TSV podrá
realizarse manualmente siguiendo las reglas establecidas en este contrato.


### 2.2 Curso

Un curso representa una asignatura perteneciente al catálogo.

Cada curso posee:

| Campo | Tipo conceptual | Descripción |
|---|---|---|
| codigo | Texto | Código oficial y único del curso dentro del catálogo |
| nombre | Texto | Nombre oficial del curso |
| creditos | Entero | Cantidad de créditos |
| semestre | Entero | Semestre del plan de estudios considerado |
| requisitos | Lista de códigos | Cursos que deben haberse aprobado previamente |
| correquisitos | Lista de códigos | Cursos que deben haberse aprobado previamente o matricularse simultáneamente |
| correquisitosPendientes | Lista de códigos | Correquisitos que aún no han sido aprobados y deberán matricularse simultáneamente |
| grupos | Lista de grupos | Grupos disponibles para dicho curso |
| tieneChoque | Booleano | Indica si al menos uno de sus grupos presenta un conflicto |
| puedeMatricular | Booleano | Indica si el estudiante puede matricular el curso según sus requisitos y correquisitos |

El código funciona como identificador del curso.

Por ejemplo:

```text
CE2201
```

permite identificar un curso sin necesidad de duplicar toda su información
cada vez que aparece como requisito o correquisito.

Por lo tanto:

```text
requisitos:
CE1101
MA1102
```
y:

```text
correquisitos:
FI1101
```

no almacenará dos cursos completos.

Solamente almacenará referencias mediante sus códigos.


### 2.3 Grupo

Un grupo representa una opción específica mediante la cual puede matricularse
un curso.

Un mismo curso puede tener varios grupos:

```text
CE1101
│
├── Grupo 1
├── Grupo 2
└── Grupo 3
```

Cada grupo contiene:

| Campo | Tipo conceptual | Descripción |
|---|---|---|
| numero | Entero | Número oficial del grupo |
| horarios | Lista de horarios | Bloques horarios asociados al grupo |
| tieneChoque | Booleano | Indica si dicho grupo presenta algún conflicto |
| choques | Lista de conflictos | Identifica exactamente los grupos con los que presenta conflictos |

El número del grupo no es un identificador global.

Por ejemplo:

```text
CE1101 Grupo 1
```

y:

```text
MA1102 Grupo 1
```

representan grupos completamente diferentes.

Por esta razón, un grupo se identifica mediante la combinación:

```text
(códigoCurso, númeroGrupo)
```

Ejemplo:

```text
(CE1101, 1)
```

Esta combinación funciona conceptualmente como una clave compuesta.


### 2.4 Horario

Un horario representa un único bloque de tiempo asociado a un grupo.

Contiene:

| Campo | Tipo conceptual | Descripción |
|---|---|---|
| dia | Texto | Día de la semana |
| inicio | Hora | Hora inicial del bloque |
| fin | Hora | Hora final del bloque |

Un grupo puede poseer más de un horario.

Ejemplo:

```text
CE1101
└── Grupo 1
    ├── LUN 07:30 - 09:20
    └── JUE 07:30 - 09:20
```

Por esta razón, la relación correcta es:

```text
Curso
└── Grupos
    └── Horarios
```

y no:

```text
Curso
└── Horario
```

Las horas deberán utilizar el formato:

```text
HH:MM
```

Ejemplos:

```text
07:30
09:20
13:00
17:50
```


### 2.5 Historial académico

El historial representa únicamente los cursos previamente aprobados por un
estudiante.

A diferencia del catálogo, el historial cambia de acuerdo con el estudiante.

Ejemplo:

```text
CE1101
MA1102
FI1101
```

No es necesario almacenar dentro del historial:

- nombre;
- créditos;
- grupos;
- horarios;
- requisitos.

Toda esa información ya se encuentra disponible dentro del catálogo.

La relación entre catálogo e historial se realiza utilizando el código del
curso.

Por ejemplo, si un curso requiere:

```text
CE1101
MA1102
```

el programa buscará ambos códigos dentro del historial.

Si ambos están presentes:

```text
cumple requisitos = verdadero
```

Si alguno no se encuentra:

```text
cumple requisitos = falso
```

El historial constituye por lo tanto la principal entrada variable asociada
al estudiante.

### 2.6 Requisitos y correquisitos

Los requisitos y correquisitos representan relaciones académicas entre
cursos.

Aunque ambos relacionan un curso con otros cursos del catálogo, poseen
reglas diferentes.

#### Requisitos

Un requisito representa un curso que debe haber sido aprobado previamente
por el estudiante.

Ejemplo:

```text
Curso A
Requisito: Curso X
```

Para poder matricular Curso A, el código correspondiente a Curso X debe
aparecer dentro del historial académico del estudiante.

Si el historial contiene:

```text
Curso X
```

el requisito se considera cumplido.

Si el historial no contiene Curso X:

```text
puedeMatricular = false
```

para Curso A.

Cuando un curso posee múltiples requisitos, todos deben encontrarse
aprobados previamente.

Ejemplo:

```text
Curso A

Requisitos:
Curso X
Curso Y
```

Historial:

```text
Curso X
Curso Y
```

Resultado:

```text
todos los requisitos cumplidos
```

Si falta cualquiera de ellos:

```text
puedeMatricular = false
```


#### Correquisitos

Un correquisito representa un curso que debe acompañar académicamente al
curso que lo referencia.

Un correquisito puede satisfacerse de dos maneras:

1. El curso correquisito ya fue aprobado previamente por el estudiante.

2. El curso correquisito todavía no ha sido aprobado, pero será matriculado
   simultáneamente con el curso que lo requiere.

Ejemplo:

```text
Curso A
Correquisito: Curso B
```

Si el historial contiene:

```text
Curso B
```

el correquisito ya está satisfecho.

Curso A podrá matricularse sin necesidad de volver a matricular Curso B.

Si el historial no contiene Curso B:

```text
Curso A + Curso B
```

deberán matricularse simultáneamente.

En este caso, Curso B será registrado como:

```text
correquisito pendiente
```

dentro de la información procesada de Curso A.


#### Diferencia entre requisito y correquisito

La diferencia principal es:

```text
Requisito
↓
Debe haberse aprobado previamente.
```

Mientras:

```text
Correquisito
↓
Puede haberse aprobado previamente
o
puede matricularse simultáneamente.
```

Por esta razón, un requisito faltante impide matricular el curso.

Un correquisito no aprobado no impide automáticamente matricularlo,
siempre que dicho correquisito pueda ser matriculado simultáneamente.

### 2.7 Conflicto de horario

Un conflicto representa la superposición entre horarios pertenecientes a dos
grupos de cursos diferentes.

Ejemplo:

```text
CE1101 Grupo 1
LUN 08:00 - 10:00

MA1102 Grupo 2
LUN 09:00 - 11:00
```

Existe un conflicto porque ambos grupos se imparten parcialmente durante el
mismo intervalo de tiempo.

El conflicto se almacena principalmente a nivel de grupo.

Ejemplo conceptual:

```text
CE1101
└── Grupo 1
    └── choca con MA1102 Grupo 2
```

La relación también deberá conservarse en sentido contrario:

```text
MA1102
└── Grupo 2
    └── choca con CE1101 Grupo 1
```

Un conflicto no significa automáticamente que el estudiante no pueda
matricular el curso.

Por ejemplo:

```text
CE1101
├── Grupo 1 -> presenta choque
└── Grupo 2 -> no presenta choque
```

El estudiante puede cumplir todos los requisitos académicos para CE1101,
aunque uno de sus grupos presente conflictos.

Por esta razón:

```text
puedeMatricular
```

y:

```text
tieneChoque
```

representan conceptos diferentes.

La construcción y selección de combinaciones compatibles de grupos no
corresponde a esta etapa.

## 3. Archivo de entrada del catálogo

### 3.1 Formato

El catálogo normalizado utilizará:

```text
TSV
```

Tab-Separated Values.

Nombre conceptual:

```text
catalogo.tsv
```

Cada columna se separará mediante un tabulador.

Cada fila representará un bloque horario correspondiente a un grupo.


### 3.2 Columnas

El archivo contendrá:

```text
carrera
semestre
codigo
nombre
creditos
requisitos
correquisitos
grupo
dia
inicio
fin
```


### 3.3 Ejemplo

```text
carrera	semestre	codigo	nombre	                        creditos	requisitos	correquisitos	grupo	dia	inicio	fin
IC	1	        CE1101	Introducción a la Programación	4	        -	        -	            1	    LUN	07:30	09:20
IC	1	        CE1101	Introducción a la Programación	4	        -	        -	            1	    JUE	07:30	09:20
IC	1	        CE1101	Introducción a la Programación	4	        -	        -	            2	    MAR	13:00	14:50
IC	1	        CE1101	Introducción a la Programación	4	        -	        -	            2	    VIE	13:00	14:50
IC	2	        CE2201	Estructuras de Datos	        4	        CE1101	    MA1102	        1	    MAR	09:30	11:20
```


### 3.4 Consolidación de filas

Las primeras cuatro filas del ejemplo anterior no representan cuatro cursos.

Representan:

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

Durante la carga, el programa deberá realizar conceptualmente las siguientes
comprobaciones:

```text
¿Existe el curso?
```

Si no existe:

```text
crear curso
```

Después:

```text
¿Existe el grupo dentro de dicho curso?
```

Si no existe:

```text
crear grupo
```

Finalmente:

```text
agregar horario al grupo
```

Si el curso y el grupo ya existen, únicamente deberá agregarse el nuevo
bloque horario.


### 3.5 Reglas de consistencia

Cuando varias filas correspondan al mismo curso, deberán permanecer iguales:

```text
carrera
semestre
codigo
nombre
creditos
requisitos
correquisitos
```

Ejemplo inconsistente:

```text
IC	1	CE1101	Introducción a la Programación	4	-	-	1	LUN	07:30	09:20
IC	1	CE1101	Introducción a la Programación	3	-	-	1	JUE	07:30	09:20
```

El mismo curso aparece primero con 4 créditos y posteriormente con 3.

El programa deberá detectar esta inconsistencia.


### 3.6 Requisitos y correquisitos múltiples

Cuando exista más de un requisito:

```text
CE1101;MA1102
```

Cuando no existan:

```text
-
```

La misma regla se aplica a los correquisitos.


### 3.7 Días permitidos

Se utilizará una representación normalizada:

```text
LUN
MAR
MIE
JUE
VIE
SAB
```

No deberán mezclarse formas como:

```text
Lunes
lunes
LUNES
L
```


### 3.8 Origen de los datos

El catálogo TSV se construirá a partir de datos recolectados desde las
fuentes institucionales correspondientes.

Por ejemplo:

```text
Guía de Horarios / TEC Digital
              ↓
       información original
              ↓
      revisión y limpieza
              ↓
         catalogo.tsv
```

La fuente original no será utilizada directamente por el núcleo del programa.

Esto permite que en el futuro pueda incorporarse un importador automático
de HTML u otras fuentes sin modificar la lógica del análisis.