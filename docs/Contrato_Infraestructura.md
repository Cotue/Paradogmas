v1.0

# 1. Propósito

Este documento define la estructura base del repositorio, las constantes comunes y el mecanismo de compilación utilizado por la primera etapa del proyecto CEmestre.

Su objetivo es mantener una organización común para los integrantes del equipo y permitir que el proyecto pueda compilarse y ejecutarse utilizando el mismo procedimiento.

Este contrato describe únicamente la infraestructura definida para el proyecto.

No implica que las funcionalidades asociadas a los directorios de datos, pruebas o salida se encuentren implementadas actualmente.

# 2. Estructura del repositorio

El proyecto utiliza una separación básica entre código fuente, archivos de cabecera, pruebas y datos.

La estructura general es:

```text
CEmestre
│
├── src/
├── include/
├── tests/
│   └── fixtures/
│
├── data/
│   ├── raw/
│   ├── clean/
│   ├── historiales/
│   └── output/
│
├── Makefile
└── README.md
```

Esta estructura permite mantener separadas las diferentes responsabilidades del proyecto.

## 2.1 src/

El directorio `src/` está destinado a contener los archivos fuente escritos en C.

El archivo principal del programa se encuentra dentro de esta estructura:

```text
src/main.c
```

Los diferentes módulos implementados durante el desarrollo podrán organizarse dentro de este directorio.

## 2.2 include/

El directorio `include/` está destinado a contener los archivos de cabecera utilizados por el código fuente.

Dentro de este directorio se encuentra:

```text
include/constants.h
```

donde se centralizan las constantes comunes del proyecto.

## 2.3 tests/

El directorio `tests/` está destinado a contener las pruebas desarrolladas para los diferentes módulos del proyecto.

La subdivisión:

```text
tests/fixtures/
```

está destinada a almacenar archivos de entrada utilizados en pruebas.

La creación de esta estructura no implica que todos los casos de prueba se encuentren implementados actualmente.

## 2.4 data/

El directorio `data/` está destinado a organizar los archivos de datos utilizados durante las diferentes etapas del proyecto.

Se divide en:

```text
data/raw/
data/clean/
data/historiales/
data/output/
```

### data/raw/

Está destinado a contener datos obtenidos desde sus fuentes originales antes de ser limpiados o normalizados.

### data/clean/

Está destinado a contener datos que hayan pasado por un proceso de limpieza o normalización y que puedan ser utilizados posteriormente por el programa.

### data/historiales/

Está destinado a contener archivos correspondientes a historiales de cursos aprobados utilizados como entrada del proyecto.

En esta etapa únicamente se establece su ubicación dentro de la estructura del repositorio.

### data/output/

Está destinado a contener archivos generados como salida por el programa.

En esta etapa únicamente se establece el directorio donde podrán almacenarse estas salidas.

# 3. Constantes comunes

Las constantes comunes del proyecto se encuentran centralizadas en:

```text
include/constants.h
```

Esto permite evitar la repetición de valores dentro de diferentes archivos fuente y proporciona un único punto para modificar valores compartidos.

Las constantes actualmente definidas son:

| Constante | Valor | Propósito |
|---|---:|---|
| `OUTPUT_SCHEMA_VERSION` | `"1.0"` | Identifica la versión del esquema de salida del proyecto |
| `MAX_COURSE_CODE_LENGTH` | `16` | Tamaño máximo reservado para un código de curso |
| `MAX_COURSE_NAME_LENGTH` | `128` | Tamaño máximo reservado para el nombre de un curso |
| `MAX_CAREER_CODE_LENGTH` | `16` | Tamaño máximo reservado para el código de una carrera |
| `MAX_CAREER_NAME_LENGTH` | `128` | Tamaño máximo reservado para el nombre de una carrera |
| `MAX_DAY_LENGTH` | `8` | Tamaño máximo reservado para representar un día |
| `MAX_TIME_LENGTH` | `8` | Tamaño máximo reservado para representar una hora |
| `MAX_LINE_LENGTH` | `1024` | Tamaño máximo definido para una línea de entrada |
| `INITIAL_CAPACITY` | `8` | Capacidad inicial definida para estructuras que puedan crecer dinámicamente |
| `TSV_DELIMITER` | `"\t"` | Delimitador definido para campos de archivos TSV |
| `LIST_DELIMITER` | `";"` | Delimitador definido para representar elementos de una lista |
| `EMPTY_FIELD` | `"-"` | Representación definida para un campo sin información |

# 4. Códigos de estado

El archivo `include/constants.h` también define el tipo enumerado:

```c
Status
```

Este tipo proporciona códigos comunes para representar el resultado de operaciones realizadas por los diferentes módulos.

Los estados definidos son:

| Estado | Valor | Significado |
|---|---:|---|
| `SUCCESS` | `0` | La operación finalizó correctamente |
| `ERROR_ARGS` | `1` | Se produjo un error relacionado con los argumentos recibidos |
| `ERROR_FILE_NOT_FOUND` | `2` | No se encontró un archivo requerido |
| `ERROR_INVALID_FORMAT` | `3` | La información recibida posee un formato inválido |
| `ERROR_MEMORY` | `4` | Se produjo un error relacionado con la gestión o reserva de memoria |

La existencia de estos códigos permite utilizar una representación común de errores entre los diferentes módulos del programa.

# 5. Compilación

La compilación del proyecto se centraliza mediante `Makefile`, ubicado en la raíz del repositorio.

El objetivo es evitar que cada integrante tenga que utilizar instrucciones de compilación diferentes.

## 5.1 Compilar en Windows

Utilizando MinGW, desde la raíz del proyecto se ejecuta:

```bash
mingw32-make
```

Este comando utiliza las reglas definidas en el `Makefile` para realizar la compilación del proyecto.

## 5.2 Limpiar la compilación

Para eliminar los archivos generados por una compilación anterior se utiliza:

```bash
mingw32-make clean
```

Esto permite realizar posteriormente una compilación limpia utilizando nuevamente:

```bash
mingw32-make
```

# 6. Ejecución

Después de una compilación exitosa, el ejecutable generado se denomina:

```text
cemestre.exe
```

Para ejecutarlo desde la raíz del proyecto se utiliza:

```bash
./cemestre.exe
```

# 7. Procedimiento común

El procedimiento establecido para trabajar con una compilación limpia en Windows es:

```text
mingw32-make clean
        ↓
mingw32-make
        ↓
./cemestre.exe
```

Este procedimiento proporciona una forma común de limpiar, compilar y ejecutar el proyecto.

# 8. Decisiones de infraestructura

## 8.1 Centralización de constantes

Las constantes compartidas se mantienen en `include/constants.h` para evitar la existencia de valores repetidos o definidos de manera independiente por diferentes módulos.

## 8.2 Compilación mediante Makefile

Se utiliza un único `Makefile` para establecer un procedimiento común de compilación.

De esta forma, los integrantes no necesitan mantener comandos de compilación independientes para cada módulo.

## 8.3 Separación de directorios

El repositorio separa código fuente, cabeceras, pruebas y datos mediante los directorios:

```text
src/
include/
tests/
data/
```

Las subdivisiones existentes establecen desde el inicio la ubicación prevista para los diferentes tipos de archivo, sin implicar que todas las funcionalidades relacionadas se encuentren implementadas.

# 9. Alcance actual

Este contrato corresponde a la configuración inicial de infraestructura del proyecto.

Actualmente establece:

- la estructura común del repositorio;
- la ubicación de las constantes compartidas;
- los códigos de estado comunes;
- el mecanismo común de compilación;
- los comandos de limpieza, compilación y ejecución.

La carga de archivos, procesamiento de datos, detección de conflictos, validación académica, generación de salidas y demás funcionalidades del sistema corresponden a otros módulos e historias de usuario y no forman parte de este contrato de infraestructura.
