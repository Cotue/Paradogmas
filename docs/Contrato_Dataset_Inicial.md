v1.0

# Contrato KAN-8: Recolección y normalización del dataset inicial

## 1. Propósito

Este documento define las reglas utilizadas en KAN-8 para recolectar,
organizar, limpiar y normalizar el dataset inicial utilizado por CEmestre.

Su objetivo es mantener una separación clara entre:

```text
fuente original
      ↓
datos recolectados
      ↓
datos normalizados
      ↓
parser de catálogo
```

KAN-8 no modifica la lógica de carga definida en KAN-7.

El formato limpio continúa respetando el contrato definido en:

```text
docs/Contrato_Carga_Validacion_Catalogo.md
```

Las fuentes concretas utilizadas durante la recolección se documentan en:

```text
docs/FUENTES.md
```


## 2. Carreras consideradas

KAN-8 prepara el dataset inicial para:

```text
Ingeniería en Computadores
Ingeniería Electrónica
```

Para ambas carreras se consideran los primeros cuatro bloques del plan de
estudios utilizado como referencia para el proyecto.

Los archivos definitivos se mantienen separados por carrera.


## 3. Organización de archivos

Los datos recolectados y los datos normalizados deben permanecer separados.

La estructura utilizada es:

```text
data/
├── raw/
│   ├── computadores_raw.tsv
│   └── electronica_raw.tsv
│
└── clean/
    ├── computadores.tsv
    └── electronica.tsv
```

Los archivos dentro de `data/raw/` conservan información de trazabilidad
proveniente de las guías de horarios.

Los archivos dentro de `data/clean/` contienen únicamente la información
normalizada que será consumida por el parser del catálogo.


## 4. Formato de recolección raw

El formato utilizado para los archivos raw es:

```text
semestre_plan
codigo
nombre_plan
sede
grupo
tipo
modalidad
periodo
creditos_guia
horas_guia
profesor
horario_original
archivo_fuente
```

El encabezado TSV es:

```text
semestre_plan	codigo	nombre_plan	sede	grupo	tipo	modalidad	periodo	creditos_guia	horas_guia	profesor	horario_original	archivo_fuente
```

Este formato no es consumido directamente por `parse_catalog()`.

Su función es conservar evidencia suficiente para relacionar una fila
normalizada con la oferta recolectada desde la guía correspondiente.


## 5. Formato clean

Los archivos normalizados utilizan exactamente el formato definido para
KAN-7:

```text
carrera	semestre	codigo	nombre	creditos	requisitos	correquisitos	grupo	dia	inicio	fin
```

Cada fila posee exactamente 11 columnas.

Una fila representa un único bloque horario.

Cuando un grupo posee más de un bloque horario, se generan varias filas
manteniendo el mismo curso y número de grupo.


## 6. Reglas de recolección

Para seleccionar la oferta de horarios se aplican las siguientes reglas:

- se utiliza únicamente el Campus Tecnológico Central Cartago;
- se utiliza modalidad `SEMESTRE`;
- se prioriza el periodo 1;
- el periodo 2 se utiliza únicamente cuando un curso requerido no posee
  oferta válida en periodo 1;
- las ofertas de `VERANO` se excluyen;
- se conservan los números de grupo reales de la guía;
- no se inventan horarios, grupos ni códigos para cubrir información ausente.

Los datos académicos del curso y los datos de oferta se mantienen separados
conceptualmente:

```text
Plan de estudios
→ semestre
→ código
→ nombre
→ créditos
→ requisitos
→ correquisitos

Guía de horarios
→ sede
→ grupo
→ modalidad
→ periodo
→ horario
```


## 7. Normalización

### 7.1 Códigos

Los códigos de curso se conservan de forma uniforme para poder ser utilizados
como identificadores dentro del catálogo.

No se sustituyen cursos por otros de nombre parecido únicamente porque
aparezcan disponibles en la guía.


### 7.2 Días

Los días utilizados en los archivos clean son:

```text
LUN
MAR
MIE
JUE
VIE
SAB
```


### 7.3 Horas

Las horas utilizan formato:

```text
HH:MM
```

Cada bloque debe cumplir:

```text
inicio < fin
```


### 7.4 Requisitos y correquisitos

La ausencia de requisitos o correquisitos se representa mediante:

```text
-
```

Cuando existe más de un código se utiliza:

```text
;
```

como separador.

Estas reglas son compatibles con el contrato de KAN-7.


## 8. Cultura y Deporte

Los códigos genéricos del plan:

```text
SE1100
SE1200
SE1400
```

no se fuerzan a una única equivalencia.

En los archivos clean se conserva la oferta real disponible de Cultura y
Deporte mediante sus códigos concretos:

```text
SE11xx
SE12xx
```

junto con sus grupos y horarios reales.

La oferta `SE` se considera transversal al dataset y no representa una
decisión sobre cuál actividad debe matricular un estudiante específico.

La validación posterior deberá considerar la regla académica utilizada para
estas actividades:

```text
tres actividades en total
dos de un tipo
una del otro
```

La evaluación del historial del estudiante y la determinación de cuáles
actividades aún necesita no pertenecen a KAN-8.


## 9. Formación Humanística

Los cursos de Formación Humanística:

```text
FH
```

no se incluyen en el dataset normalizado de KAN-8 por decisión de alcance.

Por lo tanto, `FH1000` no se considera un faltante que deba ser reemplazado
por una opción concreta dentro de este KAN.


## 10. Trazabilidad

Cada curso y grupo recolectado debe poder relacionarse con la fuente utilizada
durante la preparación del dataset.

Los archivos raw conservan, entre otros:

```text
archivo_fuente
sede
grupo
modalidad
periodo
horario_original
```

Las fuentes generales y las decisiones de normalización se documentan en:

```text
docs/FUENTES.md
```

Los archivos clean no necesitan conservar estos campos adicionales porque su
responsabilidad es respetar exclusivamente el contrato de entrada del parser.


## 11. Validación con el parser

Una vez normalizados, los archivos clean deben ser ejecutados mediante el
parser implementado en KAN-7.

Desde la raíz del proyecto:

```bash
./cemestre.exe data/clean/computadores.tsv
./cemestre.exe data/clean/electronica.tsv
```

Ambos archivos deben poder ser cargados sin errores de formato.

KAN-8 no redefine las validaciones internas del parser.

Si el parser detecta una inconsistencia real en el dataset limpio, el dato debe
corregirse en la etapa de limpieza en lugar de modificar el parser para aceptar
un formato incorrecto.


## 12. Separación de responsabilidades

KAN-8 se encarga de:

- documentar las fuentes utilizadas;
- definir la segunda carrera del dataset;
- conservar datos raw separados de los datos clean;
- recolectar la información necesaria;
- normalizar códigos, días y horarios;
- representar explícitamente la ausencia de requisitos y correquisitos;
- mantener trazabilidad hacia las fuentes;
- producir archivos compatibles con el parser;
- ejecutar el parser sobre los archivos clean.

KAN-8 no se encarga de:

- cargar el historial académico del estudiante;
- normalizar códigos provenientes del historial;
- detectar códigos duplicados dentro del historial;
- validar si un curso del historial existe en el catálogo;
- determinar cuáles actividades `SE` necesita todavía un estudiante;
- evaluar requisitos o correquisitos contra el historial;
- detectar conflictos de horario;
- generar combinaciones de matrícula;
- exportar JSON;
- implementar lógica de Racket, Prolog o Java.


## 13. Resultado esperado

Al finalizar KAN-8 deben existir:

```text
data/raw/computadores_raw.tsv
data/raw/electronica_raw.tsv
data/clean/computadores.tsv
data/clean/electronica.tsv
docs/FUENTES.md
docs/Contrato_KAN8.md
```

Los archivos de `data/clean/` constituyen el dataset normalizado que podrá
utilizar la siguiente etapa del proyecto.
