## 1. Archivo de salida

### 1.1 Formato

El programa en C generará un archivo:

```text
JSON
```

Nombre conceptual:

```text
catalogo_procesado.json
```

La información se organizará jerárquicamente:

```text
Catálogo
└── Cursos
    └── Grupos
        └── Horarios
```

A esta estructura se agregarán:

```text
requisitos
correquisitos
choques
puedeMatricular
```


### 1.2 Estructura general

```json
{
  "schemaVersion": "1.0",

  "carrera": {
    "codigo": "IC",
    "nombre": "Ingeniería en Computadores"
  },

  "cursos": []
}
```


### 1.3 Identificación de versión

El campo:

```json
"schemaVersion": "1.0"
```

permite identificar qué versión del contrato utiliza el archivo.

La primera versión será:

```text
1.0
```


### 1.4 Construcción del JSON

El JSON no se construirá directamente mientras se leen las filas del TSV.

Primero se construirá y procesará el catálogo dentro del programa.

El proceso conceptual será:

```text
TSV
 ↓
cargar información
 ↓
construir catálogo en memoria
 ↓
consolidar cursos
 ↓
consolidar grupos
 ↓
agregar horarios
 ↓
cargar historial
 ↓
validar requisitos
 ↓
identificar correquisitos aprobados
 ↓
identificar correquisitos pendientes
 ↓
determinar puedeMatricular
 ↓
calcular choques
 ↓
serializar catálogo procesado
 ↓
JSON
```

Esto mantiene separadas:

```text
lectura
procesamiento
serialización
```

y evita que la estructura del archivo JSON determine directamente cómo debe
almacenarse la información dentro del programa en C.

### 1.5 Ejemplo completo

El siguiente ejemplo es únicamente ilustrativo para mostrar la estructura
del contrato.

No representa necesariamente los requisitos, correquisitos u horarios
oficiales actuales del TEC.

```json
{
  "schemaVersion": "1.0",

  "carrera": {
    "codigo": "IC",
    "nombre": "Ingeniería en Computadores"
  },

  "cursos": [
    {
      "codigo": "CE1101",
      "nombre": "Introducción a la Programación",
      "creditos": 4,
      "semestre": 1,

      "requisitos": [],
      "correquisitos": [],
      "correquisitosPendientes": [],

      "grupos": [
        {
          "numero": 1,

          "horarios": [
            {
              "dia": "LUN",
              "inicio": "07:30",
              "fin": "09:20"
            },
            {
              "dia": "JUE",
              "inicio": "07:30",
              "fin": "09:20"
            }
          ],

          "tieneChoque": true,

          "choques": [
            {
              "curso": "MA1102",
              "grupo": 1
            }
          ]
        },

        {
          "numero": 2,

          "horarios": [
            {
              "dia": "MAR",
              "inicio": "13:00",
              "fin": "14:50"
            },
            {
              "dia": "VIE",
              "inicio": "13:00",
              "fin": "14:50"
            }
          ],

          "tieneChoque": false,
          "choques": []
        }
      ],

      "tieneChoque": true,
      "puedeMatricular": true
    },

    {
      "codigo": "MA1102",
      "nombre": "Cálculo Diferencial",
      "creditos": 4,
      "semestre": 1,

      "requisitos": ["MA1101"],
      "correquisitos": [],
      "correquisitosPendientes": [],

      "grupos": [
        {
          "numero": 1,

          "horarios": [
            {
              "dia": "LUN",
              "inicio": "08:30",
              "fin": "10:20"
            },
            {
              "dia": "MIE",
              "inicio": "08:30",
              "fin": "10:20"
            }
          ],

          "tieneChoque": true,

          "choques": [
            {
              "curso": "CE1101",
              "grupo": 1
            }
          ]
        },

        {
          "numero": 2,

          "horarios": [
            {
              "dia": "MAR",
              "inicio": "09:30",
              "fin": "11:20"
            },
            {
              "dia": "JUE",
              "inicio": "09:30",
              "fin": "11:20"
            }
          ],

          "tieneChoque": false,
          "choques": []
        }
      ],

      "tieneChoque": true,
      "puedeMatricular": true
    },

    {
      "codigo": "FI1101",
      "nombre": "Física General I",
      "creditos": 3,
      "semestre": 2,

      "requisitos": [],
      "correquisitos": [
        "MA1102"
      ],

      "grupos": [
        {
          "numero": 1,

          "horarios": [
            {
              "dia": "MAR",
              "inicio": "13:30",
              "fin": "15:20"
            },
            {
              "dia": "JUE",
              "inicio": "13:30",
              "fin": "15:20"
            }
          ],

          "tieneChoque": true,

          "choques": [
            {
              "curso": "CE1101",
              "grupo": 2
            }
          ]
        },

        {
          "numero": 2,

          "horarios": [
            {
              "dia": "MIE",
              "inicio": "09:30",
              "fin": "11:20"
            },
            {
              "dia": "VIE",
              "inicio": "13:00",
              "fin": "14:50"
            }
          ],

          "tieneChoque": false,
          "choques": []
        }
      ],

      "tieneChoque": true,
      "puedeMatricular": true
    },

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
            },
            {
              "dia": "MIE",
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
        },

        {
          "numero": 2,

          "horarios": [
            {
              "dia": "MAR",
              "inicio": "09:30",
              "fin": "11:20"
            },
            {
              "dia": "JUE",
              "inicio": "09:30",
              "fin": "11:20"
            }
          ],

          "tieneChoque": true,

          "choques": [
            {
              "curso": "MA1102",
              "grupo": 2
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


## 2. Campos adicionales

Además de los campos mínimos solicitados para la salida, se incluyen campos
adicionales con el objetivo de conservar información útil para las etapas
posteriores.


### 2.1 semestre

```json
"semestre": 2
```

Permite conservar la ubicación del curso dentro del plan de estudios
recolectado.


### 2.2 choques

```json
"choques": [
  {
    "curso": "MA1102",
    "grupo": 1
  }
]
```

El requisito mínimo únicamente solicita indicar si existe un choque.

Sin embargo, almacenar solamente:

```json
"tieneChoque": true
```

no permitiría conocer contra qué grupo se produce el conflicto.

La lista detallada permitirá a las siguientes etapas determinar cuáles
combinaciones de grupos son compatibles.


### 2.3 tieneChoque a nivel de grupo

Cada grupo conserva su propio estado de conflicto.

Esto permite diferenciar:

```text
CE1101 Grupo 1 -> tiene choque
CE1101 Grupo 2 -> no tiene choque
```

aunque ambos pertenezcan al mismo curso.


### 2.4 tieneChoque a nivel de curso

También se conserva:

```json
"tieneChoque": true
```

a nivel del curso.

Su valor será verdadero cuando al menos uno de sus grupos presente un
conflicto.


### 2.5 schemaVersion

Permite identificar la versión de la estructura utilizada para intercambiar
información con las siguientes etapas.
### 2.6 correquisitosPendientes

El campo:

```json
"correquisitosPendientes": [
  "MA1102"
]
```

contiene los códigos de los correquisitos que todavía no han sido aprobados
por el estudiante.

Estos cursos deberán matricularse simultáneamente con el curso que los
requiere.

Por ejemplo:

```text
Curso A
Correquisito: Curso B
```

Si Curso B no aparece en el historial:

```json
"correquisitosPendientes": [
  "Curso B"
]
```

Si Curso B ya fue aprobado:

```json
"correquisitosPendientes": []
```

Este campo se agrega al mínimo solicitado porque permite que la siguiente
etapa conozca qué cursos deben aparecer simultáneamente dentro de una
combinación válida.

Sin este campo, la siguiente etapa conocería cuáles son los correquisitos,
pero tendría que volver a determinar cuáles de ellos ya fueron aprobados por
el estudiante.