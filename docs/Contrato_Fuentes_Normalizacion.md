# Fuentes y reglas de normalización — KAN-8

## Alcance

Dataset de los primeros cuatro bloques de Ingeniería en Computadores e Ingeniería Electrónica.

## Fuentes

- Planes de estudio usados para semestre, código, nombre, créditos, requisitos y correquisitos.
- Guías de horario MHTML de las escuelas correspondientes para sede, grupo, modalidad, periodo y horario.
- Para CI1407 se incorporaron las dos filas de Cartago/SEMESTRE/periodo 1 suministradas explícitamente durante la revisión del dataset.

## Reglas

- Solo Campus Tecnológico Central Cartago.
- Solo modalidad `SEMESTRE`, salvo que Formación Humanística queda fuera por decisión del alcance.
- Se prioriza periodo 1; periodo 2 solo si un curso requerido no tiene oferta en periodo 1.
- `VERANO` se excluye.
- Horas normalizadas como `HH:MM`.
- Días: `LUN`, `MAR`, `MIE`, `JUE`, `VIE`, `SAB`.
- Una fila limpia representa un bloque horario.
- La ausencia de requisitos/correquisitos se representa con `-`.

## Cultura y Deporte (SE)

Los códigos genéricos del plan `SE1100`, `SE1200` y `SE1400` no se fuerzan a una única equivalencia. En el dataset limpio se conserva la oferta concreta disponible de cursos `SE11xx` (culturales) y `SE12xx` (deportivos), con sus códigos, grupos y horarios reales. Se registran como oferta transversal dentro del dataset; la validación posterior del historial deberá aplicar la regla académica de completar tres actividades en total, con dos de un tipo y una del otro. Esa validación no pertenece a KAN-8.

## Formación Humanística

`FH1000` y las opciones FH se excluyen del dataset por decisión de alcance para KAN-8.
