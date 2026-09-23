
# Contrato KAN-14: Serialización a JSON y Exportación del Catálogo Procesado

## 1. Propósito

Este documento define la especificación técnica para el módulo de serialización a JSON (KAN-14) en el sistema **CEmestre**[cite: 8].

El objetivo de este módulo es tomar la estructura interna en memoria de un `Catalog` —enriquecida tras ejecutar la detección de conflictos (KAN-12) y la evaluación de elegibilidad (KAN-13)— y exportarla a un archivo en formato JSON estructurado (`catalogo_procesado.json`)[cite: 8]. Este archivo sirve como interfaz directa y contrato de datos para la siguiente etapa del sistema (desarrollada en Racket)[cite: 3, 8].

El flujo general de ejecución en la etapa C culmina con este módulo[cite: 3, 8]:

```text
Catalog (con choques y elegibilidad calculados)
                     ↓
        serialize_catalog_to_json()
                     ↓
          catalogo_procesado.json
                     ↓
            Procesamiento en Racket

```

---

## 2. Operación Pública e API del Módulo

La interfaz pública del módulo se define en `include/json_serializer.h` y su implementación en `src/json_serializer.c`:

```c
#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include "models/catalog.h"
#include "constants.h"

/*
 * Recibe un catálogo completamente procesado y la ruta de destino.
 * Serializa la estructura jerárquica a formato JSON y la escribe en disco.
 *
 * Retorna:
 *  - SUCCESS: Archivo generado correctamente.
 *  - ERROR_ARGS: Si catalog es NULL o file_path es NULL.
 *  - ERROR_FILE_NOT_FOUND: Si no se pudo crear o abrir el archivo para escritura.
 */
Status serialize_catalog_to_json(const Catalog *catalog, const char *file_path);

#endif /* JSON_SERIALIZER_H */

```

---

## 3. Especificación del Esquema JSON (v1.0)

El JSON producido debe cumplir estrictamente con la versión `1.0` del esquema. La información se organiza jerárquicamente en cuatro niveles:

```text
Catálogo
└── Carrera
└── Cursos []
    ├── Requisitos [] / Correquisitos [] / CorrequisitosPendientes []
    ├── Banderas (tieneChoque, puedeMatricular)
    └── Grupos []
        ├── Banderas (tieneChoque)
        ├── Horarios []
        └── Choques []

```

### 3.1 Estructura del Archivo

* **`schemaVersion`**: Cadena constante `"1.0"` definida por `OUTPUT_SCHEMA_VERSION` en `constants.h`.


* **`carrera`**: Objeto con los campos `codigo` y `nombre` del catálogo.


* **`cursos`**: Arreglo de objetos donde cada elemento representa un `Course`.



### 3.2 Campos Obligatorios por Nivel

#### Nivel Curso (`Course`)

* `codigo` (string)


* `nombre` (string)


* `creditos` (integer)


* `semestre` (integer)


* `requisitos` (array de strings)


* `correquisitos` (array de strings)


* `correquisitosPendientes` (array de strings)


* `tieneChoque` (boolean) — Refleja `course->has_conflict`

* `puedeMatricular` (boolean) — Refleja `course->can_enroll`

* `grupos` (array de objetos `Group`)



#### Nivel Grupo (`Group`)

* `numero` (integer)


* `tieneChoque` (boolean) — Refleja `group->has_conflict`

* `horarios` (array de objetos `Schedule`)


* `choques` (array de objetos `GroupConflict`)



#### Nivel Horario (`Schedule`)

* `dia` (string) — Representación textual del enum `Day` (`"LUN"`, `"MAR"`, `"MIE"`, `"JUE"`, `"VIE"`, `"SAB"`).


* `inicio` (string) — Hora formateada como `"HH:MM"`.


* `fin` (string) — Hora formateada como `"HH:MM"`.



#### Nivel Choque (`GroupConflict`)

* `curso` (string) — Código del curso externo con el que choca.


* `grupo` (integer) — Número de grupo específico afectado.



---

## 4. Reglas de Formateo y Escape

1. **Inmutabilidad:** El parámetro `const Catalog *catalog` garantiza que la exportación no altera los datos en memoria.


2. **Representación de Cadenas Vueltas/Especiales:** Las cadenas de texto (`nombre`, `codigo`) deben ser escapadas correctamente si contienen comillas dobles (`"`) o caracteres de control (`\n`, `\t`) para mantener la validez sintáctica del JSON.
3. **Formato de Horas:** Los valores numéricos internos en minutos desde medianoche (`start_minutes`, `end_minutes`) deben convertirse a formato de cadena `"HH:MM"` con cero inicial (ej. `07:30`, `13:00`).


4. **Indentación:** Se utilizará una indentación consistente de 2 espacios para garantizar la legibilidad humana del archivo de salida sin inflar innecesariamente el tamaño del archivo.



---

## 5. Ejemplos de Salida

### Ejemplo Mínimo de Curso Procesado

```json
{
  "schemaVersion": "1.0",
  "carrera": {
    "codigo": "IC",
    "nombre": "Ingeniería en Computadores"
  },
  "cursos": [
    {
      "codigo": "CE2201",
      "nombre": "Estructuras de Datos",
      "creditos": 4,
      "semestre": 2,
      "requisitos": [
        "CE1101"
      ],
      "correquisitos": [
        "MA1102"
      ],
      "correquisitosPendientes": [
        "MA1102"
      ],
      "grupos": [
        {
          "numero": 1,
          "horarios": [
            {
              "dia": "LUN",
              "inicio": "09:30",
              "fin": "11:20"
            }
          ],
          "tieneChoque": true,
          "choques": [
            {
              "curso": "MA1102",
              "grupo": 1
            }
          ]
        }
      ],
      "tieneChoque": true,
      "puedeMatricular": true
    }
  ]
}

```

---

## 6. Casos de Prueba Mínimos

Las pruebas unitarias e integrales en `tests/test_json_serializer.c` deberán verificar:

| # | Caso de Prueba | Condición de Entrada | Resultado Esperado |
| --- | --- | --- | --- |
| **1** | Puntero a catálogo nulo | `catalog = NULL` | Devuelve `ERROR_ARGS` |
| **2** | Ruta invalida/inacesible | `file_path = "/ruta_invalida/out.json"` | Devuelve `ERROR_FILE_NOT_FOUND`<br> |
| **3** | Catálogo vacío | `course_count = 0` | Genera JSON válido con `"cursos": []` |
| **4** | Cursos sin choques/requisitos | Listas vacías, `has_conflict = false` | Genera arreglos vacíos `[]` y booleanos `false` |
| **5** | Conversión correcta de horas | `start_minutes = 450` | Formatea a `"07:30"` en el JSON |
| **6** | Validez sintáctica del JSON | Salida generada por la función | El archivo resultante pasa la validación de un parser JSON estándar |

---

## 7. Manejo de Errores y Códigos de Estado

El módulo retorna los siguientes códigos definidos en `include/constants.h`:

* `SUCCESS` (`0`): El archivo JSON se creó y escribió correctamente.


* `ERROR_ARGS` (`1`): Punteros nulos en `catalog` o `file_path`.


* `ERROR_FILE_NOT_FOUND` (`2`): No fue posible abrir el archivo especificado para escritura.



---

## 8. Separación de Responsabilidades

### KAN-14 SÍ se encarga de:

* Recorrer la estructura del `Catalog` procesado y formatearla en sintaxis JSON válida.


* Convertir la representación interna de días (`Day`) y minutos a cadenas legible (`"LUN"`, `"07:30"`).


* Escribir el archivo final en la ruta indicada.



### KAN-14 NO se encarga de:

* Calcular si hay choques (`has_conflict`) (responsabilidad de KAN-12).


* Evaluar si se cumplen requisitos o determinar `can_enroll` (responsabilidad de KAN-13).


* Interpretar o filtrar las combinaciones de horario (responsabilidad de Racket).
