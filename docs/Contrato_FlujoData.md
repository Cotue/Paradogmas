## 1. Flujo de datos

Una vez definidos los elementos anteriores, el funcionamiento completo puede
entenderse de la siguiente manera.


### 1.1 Preparación del catálogo

La información se obtiene inicialmente desde una fuente institucional:

```text
Guía de Horarios / TEC Digital / HTML
                 ↓
          recolección manual
                 ↓
         limpieza de datos
                 ↓
          normalización
                 ↓
            catalogo.tsv
```

El catálogo generado puede reutilizarse para diferentes estudiantes.


### 1.2 Entrada del estudiante

Cada estudiante proporciona su historial:

```text
historial.txt
```

Ejemplo:

```text
CE1101
MA1102
FI1101
```


### 1.3 Carga en C

El programa recibe:

```text
catalogo.tsv
      +
historial.txt
```

y construye internamente:

```text
Catalogo
│
└── Cursos
    ├── requisitos
    ├── correquisitos
    └── grupos
        └── horarios

Historial
└── códigos aprobados
```


### 1.4 Procesamiento

El catálogo y el historial participan en dos procesos principales.


#### Validación académica

Cada curso se analiza utilizando su lista de requisitos y correquisitos.

```text
                   Curso
                     │
          ┌──────────┴──────────┐
          ↓                     ↓
     Requisitos            Correquisitos
          │                     │
          ↓                     ↓
      Historial              Historial
          │                     │
          ↓                     ↓
 ¿Todos aprobados?       ¿Está aprobado?
       /     \               /       \
     Sí       No           Sí         No
     ↓         ↓           ↓           ↓
 continuar   puede      satisfecho   correquisito
             Matricular               pendiente
             = false                     │
                                         ↓
                              correquisitosPendientes
```

La regla general será:

```text
Si falta al menos un requisito:
    puedeMatricular = false

Si todos los requisitos fueron aprobados:
    puedeMatricular = true
```

Los correquisitos no aprobados se almacenarán en:

```text
correquisitosPendientes
```

y deberán ser considerados por las siguientes etapas como cursos que deben
matricularse simultáneamente.


#### Detección de choques

La detección de conflictos utiliza únicamente la información del catálogo:

```text
Catálogo
   ↓
Cursos
   ↓
Grupos
   ↓
Horarios
   ↓
Comparación entre grupos
   ↓
Conflictos
```

Los choques no modifican directamente:

```text
puedeMatricular
```

ya que un estudiante puede tener derecho académico a matricular un curso
aunque algunos de sus grupos sean incompatibles con otros cursos.

La selección de grupos compatibles corresponde a las siguientes etapas.


#### Validación académica

```text
Requisitos y correquisitos del catálogo
                  +
       Historial del estudiante
                  ↓
            comparación
                  ↓
          puedeMatricular
```


### 1.5 Resultado

Después del procesamiento:

```text
Catálogo original
       +
Conflictos detectados
       +
Elegibilidad académica
       ↓
Catálogo procesado
       ↓
catalogo_procesado.json
```


### 1.6 Uso por la siguiente etapa

La primera etapa termina al generar:

```text
catalogo_procesado.json
```

C no construye ni recomienda el horario final.

La siguiente etapa recibe el catálogo procesado:

```text
catalogo_procesado.json
            ↓
          Racket
            ↓
Generación y filtrado de combinaciones
            ↓
Posibles horarios
```

Por lo tanto, el flujo general de esta etapa es:

```text
                FUENTE INSTITUCIONAL
                        ↓
          recolección, limpieza y
                normalización
                        ↓
                  catalogo.tsv
                        │
                        │
                        ↓
                   Programa C
                        ↑
                        │
                  historial.txt
                        │
                        ↓
                Catálogo en memoria
                        │
             ┌──────────┴──────────┐
             │                     │
             ↓                     ↓
     Información académica      Horarios
             │                     │
      ┌──────┴──────┐              ↓
      ↓             ↓          Comparación
 Requisitos    Correquisitos     de grupos
      │             │              │
      │             │              ↓
      └──────┬──────┘           Choques
             │                     │
             ↓                     │
         Historial                  │
             │                     │
             ↓                     │
     ┌───────────────────┐         │
     │                   │         │
     ↓                   ↓         │
Requisitos          Correquisitos  │
cumplidos            pendientes    │
     │                   │         │
     └─────────┬─────────┘         │
               ↓                   │
          Elegibilidad             │
               │                   │
               └─────────┬─────────┘
                         ↓
                Catálogo procesado
                         ↓
             catalogo_procesado.json
                         ↓
                       Racket
                         ↓
          Combinaciones posibles
                  de horario
```