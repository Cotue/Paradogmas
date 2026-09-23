v1.0

# Contrato KAN-12: Detección de conflictos de horario

## 1. Propósito

Este documento define las reglas implementadas en KAN-12 para detectar
conflictos de horario entre los grupos disponibles dentro de un catálogo de
CEmestre.

KAN-12 trabaja sobre un `Catalog` previamente cargado y validado.

La estructura conceptual de cursos, grupos, horarios y conflictos continúa
definida en:

```text
docs/Contrato_Catalogo.md
```

La carga y validación del catálogo continúa definida en:

```text
docs/Contrato_Carga_Validacion_Catalogo.md
```

KAN-12 no vuelve a leer ni validar el archivo TSV.

Su responsabilidad comienza después de que el catálogo ya se encuentra
correctamente construido en memoria:

```text
catalogo.tsv
      ↓
parse_catalog()
      ↓
Catalog validado
      ↓
detect_catalog_conflicts()
      ↓
Catalog con conflictos calculados
```

## 2. Entrada del detector

La operación pública definida por KAN-12 es:

```c
Status detect_catalog_conflicts(Catalog *catalog);
```

La función recibe un `Catalog` previamente construido.

Se asume que:

- los cursos ya se encuentran consolidados por código;
- los grupos ya se encuentran consolidados por número dentro de cada curso;
- cada grupo contiene sus bloques horarios correspondientes;
- los días ya fueron convertidos al tipo `Day`;
- las horas ya fueron convertidas a minutos desde medianoche;
- cada bloque horario cumple `inicio < fin`.

Estas validaciones pertenecen a KAN-7 y no se repiten en KAN-12.

## 3. Representación de horarios

Cada `Schedule` representa un único bloque horario mediante:

```text
day
start_minutes
end_minutes
```

Un grupo puede contener uno o varios bloques:

```text
Course
└── Group
    ├── Schedule
    ├── Schedule
    └── ...
```

Para determinar si dos grupos presentan un conflicto se deben comparar todos
los bloques de ambos grupos.

## 4. Regla de conflicto entre dos horarios

Dos bloques horarios solamente pueden presentar conflicto cuando pertenecen al
mismo día.

Cuando los bloques pertenecen al mismo día se considera que existe
superposición si:

```text
inicioA < finB
y
inicioB < finA
```

En términos de la representación interna:

```c
schedule_a->start_minutes < schedule_b->end_minutes &&
schedule_b->start_minutes < schedule_a->end_minutes
```

## 5. Horarios consecutivos

Dos bloques que únicamente comparten el instante en que uno termina y el otro
comienza no presentan conflicto.

Ejemplo:

```text
07:30 - 09:20
09:20 - 11:10
```

Resultado:

```text
sin conflicto
```

Por esta razón la comparación utiliza `<` y no `<=`.

La regla también permite detectar correctamente:

- superposición parcial;
- horarios idénticos;
- un intervalo completamente contenido dentro de otro.

## 6. Conflicto entre grupos

Dos grupos presentan conflicto cuando existe al menos una pareja de bloques
horarios que se superpone.

Si al menos una pareja presenta conflicto:

```text
Group A y Group B tienen conflicto
```

No es necesario que todos los bloques de ambos grupos se superpongan.

## 7. Cursos considerados

KAN-12 compara únicamente grupos pertenecientes a cursos diferentes.

Un curso nunca se compara contra sí mismo como conflicto externo.

El recorrido de cursos evita repetir pares mediante:

```text
curso i
vs
curso j, donde j > i
```

Esto garantiza que:

- un curso no se compara consigo mismo;
- un mismo par de cursos no se analiza dos veces.

## 8. Identidad de un grupo

El número de grupo no constituye un identificador global.

Cada conflicto utiliza la identidad compuesta:

```text
(codigoCurso, numeroGrupo)
```

La representación utilizada por KAN-12 es:

```c
typedef struct {
    char *course_code;
    int group_number;
} GroupConflict;
```

## 9. Información almacenada en Group

Cada `Group` mantiene:

```text
has_conflict
conflicts[]
conflict_count
conflict_capacity
```

`has_conflict` indica si el grupo presenta al menos un conflicto.

`conflicts` identifica exactamente los grupos de otros cursos con los que
existe una superposición.

La lista puede contener múltiples conflictos.

## 10. Simetría de los conflictos

Todo conflicto debe almacenarse en ambos sentidos.

Si A presenta conflicto con B, debe almacenarse:

```text
A -> B
B -> A
```

No se considera válido dejar una relación parcialmente almacenada.

## 11. Información almacenada en Course

`Course.has_conflict` representa si al menos uno de sus grupos posee un
conflicto.

Por lo tanto, `Course.has_conflict` no significa que todos sus grupos tengan
conflicto.

## 12. Independencia de la elegibilidad académica

La existencia de conflictos de horario es independiente de la validación de
requisitos y correquisitos.

KAN-12 no modifica:

```text
Course.can_enroll
Course.requirements
Course.corequisites
Course.pending_corequisites
```

## 13. Recálculo de conflictos

Los conflictos son información derivada.

Antes de realizar cada detección, KAN-12 elimina los resultados calculados en
una ejecución anterior.

Se reinician:

```text
Course.has_conflict
Group.has_conflict
Group.conflicts
Group.conflict_count
Group.conflict_capacity
```

No se eliminan ni modifican los horarios del grupo.

Esto permite ejecutar el detector varias veces sin duplicar conflictos ni
conservar resultados obsoletos.

## 14. Manejo de memoria

Cada `Group` es propietario de:

- su arreglo dinámico de `Schedule`;
- su arreglo dinámico de `GroupConflict`;
- cada copia dinámica de `course_code` almacenada dentro de sus conflictos.

`group_clear_conflicts()` libera únicamente la información de conflictos.

`group_free()` libera tanto los horarios como los conflictos antes de reiniciar
la estructura completa.

Los conflictos almacenan una copia del código de curso en lugar de un puntero
directo a otro `Group`, evitando referencias inválidas después de un `realloc()`.

## 15. Estados de retorno

`detect_catalog_conflicts()` utiliza:

```text
SUCCESS
ERROR_ARGS
ERROR_MEMORY
```

`SUCCESS`: el análisis terminó correctamente.

`ERROR_ARGS`: `catalog == NULL`.

`ERROR_MEMORY`: no fue posible reservar memoria para almacenar un conflicto.

Si ocurre un error de memoria durante el registro, KAN-12 elimina todos los
resultados parciales antes de retornar.

## 16. Flujo del detector

```text
detect_catalog_conflicts()
        ↓
limpiar conflictos anteriores
        ↓
recorrer pares de cursos diferentes
        ↓
recorrer grupos de ambos cursos
        ↓
comparar todos sus Schedule
        ↓
¿existe superposición?
        │
        ├── no → continuar
        │
        └── sí
             ↓
        guardar A -> B
        guardar B -> A
             ↓
        marcar ambos Group
             ↓
        marcar ambos Course
```

## 17. Pruebas unitarias

KAN-12 posee pruebas aisladas construyendo el catálogo directamente en memoria.

Se verifican:

- superposición parcial en el mismo día;
- horarios idénticos;
- un horario contenido dentro de otro;
- horarios consecutivos sin conflicto;
- horarios separados en el mismo día;
- mismas horas en días diferentes;
- grupos del mismo curso no comparados;
- grupos con múltiples bloques;
- solo el grupo realmente afectado queda marcado;
- almacenamiento simétrico;
- un grupo con conflictos contra múltiples cursos;
- ejecución repetida sin duplicados;
- catálogo con un solo curso;
- `Catalog *` nulo.

Resultado esperado:

```text
Fallos totales: 0
```

## 18. Pruebas de integración

KAN-12 también se prueba utilizando los datasets normalizados reales de KAN-8:

```text
data/clean/computadores.tsv
data/clean/electronica.tsv
```

La prueba ejecuta:

```text
archivo clean
      ↓
parse_catalog()
      ↓
detect_catalog_conflicts()
      ↓
validación independiente de resultados
```

La validación comprueba que:

- cada `Group.has_conflict` coincide con sus horarios;
- `conflict_count` coincide con la cantidad real de grupos incompatibles;
- cada curso y grupo referenciado existe;
- ningún grupo se relaciona con otro grupo de su mismo curso;
- cada conflicto almacenado corresponde a una superposición real;
- cada conflicto posee su relación simétrica;
- `Course.has_conflict` coincide con el estado de sus grupos;
- una segunda ejecución del detector no genera duplicados.

La cantidad concreta de conflictos no forma parte del contrato porque depende
del dataset utilizado.

El criterio de éxito es:

```text
Computadores -> PASS
Electrónica  -> PASS
Fallos totales: 0
```

## 19. Separación de responsabilidades

KAN-12 se encarga de:

- comparar horarios de grupos pertenecientes a cursos diferentes;
- detectar superposición de intervalos en el mismo día;
- considerar correctamente grupos con múltiples bloques;
- marcar `Group.has_conflict`;
- almacenar la lista detallada de conflictos;
- mantener las relaciones de conflicto de forma simétrica;
- marcar `Course.has_conflict`;
- permitir recalcular los conflictos sin duplicar resultados;
- liberar correctamente la información derivada de conflictos;
- probar la lógica de forma aislada;
- validar la integración con los datasets reales.

KAN-12 no se encarga de:

- leer o validar el formato TSV;
- corregir datos del catálogo;
- recolectar o normalizar datasets;
- cargar el historial académico;
- evaluar requisitos;
- evaluar correquisitos;
- modificar `can_enroll`;
- decidir qué cursos debe matricular un estudiante;
- generar combinaciones de horario;
- exportar JSON;
- implementar lógica de Racket, Prolog o Java.

## 20. Resultado esperado

Al finalizar KAN-12 deben existir:

```text
include/conflict_detector.h
src/conflict_detector.c
tests/test_conflict_detector.c
tests/test_conflict_integration.c
docs/Contrato_Deteccion_Conflictos_Horario.md
```

Además, el modelo `Group` debe soportar la representación detallada de
conflictos mediante:

```text
GroupConflict[]
```

El catálogo resultante queda preparado para ser utilizado posteriormente por
la serialización de la primera etapa y por las siguientes etapas de CEmestre.
