# Contrato KANE3-31: Extensión del catálogo de C para la Etapa 3

## 1. Propósito

Este documento define cómo se extiende el catálogo generado por C para que
las etapas posteriores (Racket y Prolog) tengan los datos que necesitan en la
Etapa 3 sin tener que reconstruirlos.

La extensión agrega dos datos por curso, `carrera` y `area`, amplía los planes
hasta el semestre 8 y sube la versión del JSON a `3.0`.

Este contrato no reemplaza a los anteriores. Complementa a:

```text
docs/Contrato_Catalogo.md
docs/Contrato_Carga_Validacion_Catalogo.md
docs/Contrato_JSON.md
docs/Contrato_Serializacion_Json.md
docs/Contrato_Dataset_Inicial.md
```

Si alguno de ellos contradice lo que dice este documento sobre el formato del
TSV, la procedencia, el área o el schema del JSON, vale este contrato.


## 2. Alcance

Se modifica:

- el formato del TSV de entrada (de 11 a 12 columnas);
- la forma de identificar el plan (ahora llega por línea de comandos);
- el modelo `Course` (campos `career` y `area`);
- la lectura y validación del catálogo;
- la exportación JSON;
- los datos de los planes (semestres 1 a 8).

No se modifica:

```text
requisitos
correquisitos
elegibilidad
cálculo de choques
```

Los cursos nuevos de los semestres 7 y 8 pasan por las mismas reglas que ya
existían. No hay algoritmo nuevo para ellos.


## 3. Plan, carrera y área

Son tres conceptos distintos y no deben mezclarse.

| Concepto | Qué representa | De dónde sale | Dónde se guarda |
|---|---|---|---|
| Plan | El plan que cursa el estudiante (IC o IE) | Cuarto argumento de `main` | `catalog->career_code` y `catalog->career_name` |
| Carrera | La escuela que imparte el curso, como etiqueta corta | Columna `carrera` del TSV, validada contra el prefijo del código | `course->career` |
| Área | Clasificación amplia definida por el equipo | Columna `area` del TSV | `course->area` |

Un mismo plan contiene cursos de muchas escuelas. Por ejemplo, el plan IC
tiene cursos de la Escuela de Matemática, de Física, de Química, etc.

`carrera` alimenta la regla de balance entre carreras en Prolog.
`area` alimenta la regla de límite de cursos por área.

Nota: los campos del catálogo se llaman `career_code` y `career_name` por
herencia de la Etapa 1, pero guardan el plan, no la procedencia.


## 4. Formato del TSV

La primera línea no vacía debe ser exactamente:

```text
carrera	semestre	codigo	nombre	creditos	area	requisitos	correquisitos	grupo	dia	inicio	fin
```

Reglas:

- separador de columnas: tabulador;
- cada fila de datos tiene exactamente 12 columnas;
- un campo sin valor se escribe `-` (solo en requisitos y correquisitos);
- las listas de requisitos y correquisitos se separan con `;`;
- un código dentro de una lista no puede tener comas ni espacios;
- el encabezado viejo de 11 columnas se rechaza.

Ejemplo:

```text
computadores	7	CE1112	taller de señales mixtas	3	computadores	CE1109;CE1110	-	1	LUN	07:30	09:20
```


## 5. Plan por línea de comandos

El plan ya no se toma del TSV. Se recibe como cuarto argumento:

```powershell
.\cemestre.exe <catalogo.tsv> <historial.txt> <salida.json> <plan>
```

Firma del parser:

```c
Status parse_catalog(const char *filepath, Catalog *catalog, const char *plan_code);
```

Planes válidos:

| Código | Nombre |
|---|---|
| `IC` | Ingeniería en Computadores |
| `IE` | Ingeniería Electrónica |

Cualquier otro código devuelve `ERROR_INVALID_FORMAT`.

El parser no verifica que los cursos del TSV pertenezcan al plan indicado.
Elegir el TSV correcto para cada plan es responsabilidad de quien ejecuta.


## 6. Procedencia académica (`carrera`)

### 6.1 Regla

La carrera de un curso es la escuela que lo imparte, escrita como una etiqueta
corta en minúsculas, sin tildes ni espacios (por ejemplo `matematica`). Se
obtiene del prefijo de dos letras del código mediante una única tabla central:

```text
src/career_mapping.c
const char *get_career_from_course_code(const char *course_code);
```

El TSV trae la carrera escrita en la columna `carrera`. El parser la compara
con la que da la tabla para ese prefijo:

- si coinciden, se acepta;
- si no coinciden, es error;
- si el prefijo no está en la tabla, es error.

Un prefijo desconocido nunca se asigna a una carrera genérica como `otros`.
Para aceptar un prefijo nuevo hay que agregarlo a la tabla.

Racket y Prolog no deben repetir esta lógica: reciben la carrera ya resuelta.

### 6.2 Tabla de prefijos

| Prefijo | Carrera | Escuela |
|---|---|---|
| `AE` | `administracion` | Escuela de Administración de Empresas |
| `CA` | `computacion` | Escuela de Ingeniería en Computación |
| `CE` | `computadores` | Escuela de Ingeniería en Computadores |
| `CI` | `lenguaje` | Escuela de Ciencias del Lenguaje |
| `CM` | `materiales` | Escuela de Ciencia e Ingeniería de los Materiales |
| `CS` | `sociales` | Escuela de Ciencias Sociales |
| `EL` | `electronica` | Escuela de Ingeniería Electrónica |
| `FI` | `fisica` | Escuela de Física |
| `MA` | `matematica` | Escuela de Matemática |
| `MI` | `electromecanica` | Escuela de Ingeniería Electromecánica |
| `PI` | `produccion_industrial` | Escuela de Ingeniería en Producción Industrial |
| `QU` | `quimica` | Escuela de Química |
| `SE` | `cultura_deporte` | Escuela de Cultura y Deporte |
| `SO` | `seguridad_laboral` | Escuela de Ingeniería en Seguridad Laboral e Higiene Ambiental |

Solo la columna "Carrera" se usa en el TSV y en el JSON. La columna "Escuela"
es informativa.

Se usan etiquetas cortas y sin tildes porque Prolog las usa directamente como
átomos en la política (`max_porcentaje_carrera(matematica, 50).`) y porque así
no hay diferencias de codificación entre etapas. Las de `CE`, `MA`, `FI` y `EL`
son las mismas del contrato maestro de la Etapa 3.

La comparación es exacta: `Matematica` o `matemática` se rechazan.


### 6.3 Consistencia

Como la carrera se valida contra el prefijo y todas las filas de un curso
tienen el mismo código, todas las apariciones de un curso tienen
necesariamente la misma carrera. Además, el parser lo vuelve a comparar en la
verificación de consistencia (sección 9).


## 7. Área (`area`)

### 7.1 Valores válidos

```text
ciencias_naturales
matematicas
comunicacion
computadores
optativas
electronica
```

Se rechaza:

- un área vacía;
- `-`;
- cualquier valor fuera de la lista, incluyendo variantes con mayúscula
  (`Matematicas`) o categorías más específicas (`calculo`).

Validación:

```c
Status validate_area_format(const char *area);
```

### 7.2 El área no se deduce del prefijo

El área es un dato académico que viene escrito en el TSV. C no la calcula ni la
corrige.

Ejemplos reales de los planes donde el prefijo no coincide con el área:

| Curso | Carrera | Área |
|---|---|---|
| CE5701 electiva i | `computadores` | `optativas` |
| CE2201 laboratorio de circuitos eléctricos | `computadores` | `electronica` |
| EL4702 probabilidad y estadística | `electronica` | `matematicas` |
| PI2609 probabilidad y estadística | `produccion_industrial` | `matematicas` |
| SE1201 Atletismo | `cultura_deporte` | `optativas` |


## 8. Semestres

Los semestres válidos son del 1 al 8. Un semestre 0 o 9 o mayor es error.

```c
Status validate_semester_format(const char *sem_str);
```


## 9. Consistencia entre filas de un mismo curso

Un curso aparece en tantas filas como bloques de horario tenga. Todas sus filas
deben repetir exactamente:

```text
carrera
area
créditos
semestre
nombre
requisitos
correquisitos
```

Los requisitos y correquisitos se comparan como conjuntos: `A;B` y `B;A` son
iguales.

Una inconsistencia es error. El parser nunca la corrige ni elige un valor.


## 10. Orden de validación por fila

Para cada fila de datos el parser revisa, en este orden:

1. cantidad de columnas (12);
2. procedencia: la carrera coincide con el prefijo;
3. área válida;
4. código y nombre, día, horario, semestre, créditos y grupo;
5. requisitos y correquisitos con formato válido;
6. si el curso ya existía, consistencia con sus filas anteriores.

Ante el primer error se imprime un mensaje con el curso y la línea, se libera
el catálogo y se devuelve `ERROR_INVALID_FORMAT`.

| Mensaje | Causa |
|---|---|
| `Encabezado TSV invalido en linea N` | El encabezado no es el de 12 columnas |
| `Cantidad de columnas incorrecta en linea N` | La fila no tiene 12 columnas |
| `Procedencia academica invalida para el curso X en linea N` | Carrera vacía, distinta al prefijo o prefijo desconocido |
| `Area invalida para el curso X en linea N` | Área vacía, `-` o fuera de la lista |
| `Datos invalidos para el curso X en linea N` | Día, horario, semestre, créditos o grupo inválidos |
| `Informacion inconsistente para el curso X en linea N` | Una fila contradice a otra del mismo curso |

Los mensajes de consola y los nombres de los tests se escriben sin tildes para
que se lean bien en la consola de Windows.

Un error de formato en requisitos o correquisitos (por ejemplo `A;;B` o
`CE1101, MA1102`) devuelve `ERROR_INVALID_FORMAT` sin mensaje propio.


## 11. Modelo `Course`

Se agregan dos campos:

```c
char *career;  // escuela de procedencia
char *area;    // clasificación de área
```

Se cargan con:

```c
Course* catalog_add_course(Catalog *catalog, const char *code, const char *name,
                           const char *career, const char *area,
                           int credits, int semester);
```

Reglas de memoria:

- `catalog_add_course` guarda su propia copia de `career` y `area`;
- si `career` o `area` es `NULL`, devuelve `NULL`;
- `course_free` libera ambos campos.


## 12. JSON de salida (schema 3.0)

La versión se define en `include/constants.h`:

```c
#define OUTPUT_SCHEMA_VERSION "3.0"
```

Cambios respecto a la versión anterior:

- `schemaVersion` pasa a `"3.0"`;
- cada curso lleva `"carrera"` (procedencia) y `"area"`.

El objeto `"carrera"` de la raíz no cambia de significado: sigue siendo el plan.
Los nombres del plan y de los cursos conservan sus tildes; el JSON se escribe en
UTF-8.

```json
{
  "schemaVersion": "3.0",
  "carrera": {
    "codigo": "IC",
    "nombre": "Ingeniería en Computadores"
  },
  "cursos": [
    {
      "codigo": "MA1102",
      "nombre": "cálculo diferencial e integral",
      "carrera": "matematica",
      "area": "matematicas",
      "creditos": 4,
      "semestre": 1,
      "requisitos": [],
      "correquisitos": [],
      "correquisitosPendientes": [],
      "grupos": [ ... ],
      "tieneChoque": true,
      "puedeMatricular": false
    }
  ]
}
```

Antes de abrir el archivo de salida, el serializador revisa que ningún curso
tenga `career` o `area` en `NULL`. Si alguno falta, devuelve
`ERROR_INVALID_FORMAT` y no crea el archivo.

Un catálogo con schema anterior a `3.0` no sirve para la Etapa 3.


## 13. Datos oficiales de la Etapa 3

| Plan | Archivo | Cursos | Semestres |
|---|---|---|---|
| IC | `data/clean/compu_V3.tsv` | 61 | 1 a 8 |
| IE | `data/clean/electro_V3.tsv` | 65 | 1 a 8 |

Los demás TSV de `data/clean/` son versiones anteriores y no se usan en esta
etapa.

### 13.1 Correcciones hechas a los datos

- Separador de listas: había requisitos separados con `, ` en lugar de `;`
  (CE1112, CE1116, CE5301 y CE5701 en IC; EL3217, EL3313, EL4201, EL4514,
  EL4607, EL5513 y PI3512 en IE). Se corrigieron a `;`. El parser ahora
  rechaza ese formato para que no vuelva a pasar inadvertido.
- Horario de CE5701 (electiva i): no tenía horario en la fuente. Se le asignó
  el mismo horario de CE1113 (grupo 1, MAR y JUE de 15:00 a 16:50).
  **Este horario no es un dato oficial**, lo puso el equipo para que el curso
  pueda cargarse. Por eso CE5701 siempre choca con CE1113.
- Columna `carrera`: se pasó del nombre largo de la escuela
  (`Escuela de Matemática`) a la etiqueta corta (`matematica`) de la sección 6.2.

### 13.2 Particularidades conocidas

- Requisitos que no están en el catálogo: CE5701 pide CI3203, CI1230 pide
  CI0200 y CI0202, y PI3512 pide PI3308. Como el historial solo acepta códigos
  del catálogo, esos cursos nunca podrán matricularse. No es error de carga.
- Los cursos SE (Cultura y Deporte) tienen 0 créditos. Sirven como caso límite
  para la regla de balance en Prolog.


## 14. Pruebas

Comandos (desde la raíz del repo):

```powershell
gcc tests/test_validator.c src/validator.c -Iinclude -Iinclude/models -Wall -Wextra -Wpedantic -std=c11 -o test_validator.exe
.\test_validator.exe

gcc tests/test_catalog_parser.c src/catalog_parser.c src/validator.c src/career_mapping.c src/models/catalog.c src/models/code_list.c src/models/course.c src/models/group.c src/models/schedule.c -Iinclude -Iinclude/models -Wall -Wextra -Wpedantic -std=c11 -o test_catalog_parser.exe
.\test_catalog_parser.exe
```

Relación entre criterios de aceptación y pruebas:

| Criterio | Cómo se verifica |
|---|---|
| 1. Planes con 8 semestres | `test_conflict_integration` carga `compu_V3.tsv` y `electro_V3.tsv`; casos "Semestre 7/8 válido" |
| 2. Carrera válida | "Carrera vacía", "Carrera '-'", "Carrera distinta al prefijo" |
| 3. Tabla central de prefijos | `career_mapping.c`; casos de procedencia |
| 4. Prefijo desconocido no se asigna | "Prefijo desconocido" (ZZ1101) |
| 5 a 7. Área presente, válida y no vacía | `test_area` en `test_validator`; casos "Área vacía", "Área '-'", "Área 'otros'", "Área con mayúscula" |
| 8. Misma carrera por curso | Garantizado por la sección 6.3 |
| 9. Misma área por curso | "Área contradictoria en mismo curso" |
| 10. Mismos créditos por curso | "Créditos contradictorios" |
| 11. Área no inferida del prefijo | "Curso CE con área optativas válido" |
| 12. Modelo con carrera y área | "Campos career y area del curso cargado" |
| 13. JSON con ambos campos | Pendiente |
| 14. Schema 3.0 | `constants.h`; prueba pendiente |
| 15 a 18. Sin cambios en requisitos, correquisitos, elegibilidad y choques | `test_conflict_detector`, `test_conflict_integration`, `test_history_parser`, `test_integration` sin cambios de lógica |
| 19. Pruebas anteriores pasan | Todos los tests existentes adaptados y en verde |
| 20. Pruebas nuevas | Casos nuevos en `test_catalog_parser` y `test_validator` |


## 15. Pendientes

- Agregar las pruebas del JSON: `schemaVersion "3.0"`, `carrera` y `area` por
  curso (criterios 13 y 14).
- Actualizar los contratos anteriores para que remitan a este.
- Actualizar el lector de Racket al schema 3.0 (fuera de esta historia).
- Observaciones menores: los nombres de los planes están escritos dentro de
  `set_catalog_plan` en lugar de junto a la tabla de prefijos, y `main` no
  imprime nada cuando falla el historial, no encuentra un archivo o el plan es
  inválido.
