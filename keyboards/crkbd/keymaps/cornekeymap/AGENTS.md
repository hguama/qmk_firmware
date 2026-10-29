# Instrucciones para agentes de IA

> ⚠️ Leer antes de trabajar en este proyecto.

## Tareas (formato simple)

Al empezar una sesión, lee [`tasks/pending.md`](tasks/pending.md) y recuérdale
al usuario lo que está sin marcar. Trabajen desde ahí.

- **Checklist:** una línea por tarea en `tasks/pending.md`: `- [ ] N · título: descripción corta`.
  La numeración continúa desde el último número usado (el próximo es el 20).
- **Estudio o contrato:** si una tarea necesita análisis, va en
  `tasks/attachments/<nombre>/spec-<nombre>.md` y se enlaza desde su línea.
- **Al cerrar:** quita la línea de `pending.md` y añádela arriba en
  `tasks/completed.md`, sección "Cerradas": `- [x] N · título — fecha — qué quedó → [estudio](...)`.
  El estudio se queda en `attachments/`.
- **Errores y lecciones:** se anotan en `tasks/fixes.md`.
- **Sencilla o con estudio:** ante cada pedido, decide cuál es y dilo en una línea
  antes de actuar ("Es sencilla, la hago directo" / "Esto requiere estudio, porque…").
  El usuario puede corregirte.
  - **Sencilla** → hazla directo en `keymap.c`, compila y reporta. No hace falta
    spec. Ejemplos: quitar o cambiar una tecla, crear una tecla personalizada
    con comportamiento claro, mover algo de capa, ajustar un valor.
    Si piden varias, no las registres en el checklist: solo hazlas.
  - **Con estudio** → no escribas código todavía. Ábrela como tarea con número en
    `pending.md`, escribe el estudio en `attachments/<nombre>/spec-<nombre>.md`
    (situación actual, opciones con pros y contras, recomendación) y espera la
    decisión del usuario. Señales: hay varias formas de hacerlo y no es obvio
    cuál; toca ratón, rueda, tap dances, temporizadores o interacciones entre
    capas; puede romper otras teclas; necesita algo fuera de QMK (script
    externo, configuración del sistema, herramienta aparte); o el pedido es
    ambiguo.
  - Una tarea sencilla que al revisarla resulte más compleja pasa a "con estudio":
    avisa y cambia de ruta.
- No uses el formato de la skill `task-manager` (bloques, riesgos, callouts): aquí se mantiene simple.

## Compilar (obligatorio después de cada cambio)

```bash
qmk compile -kb crkbd/rev1 -km cornekeymap
```

- Al terminar cualquier cambio: compilar de esta manera. Se entrega solamente código (el `.hex` y `.build/` no se commitean).
- Si hay errores: consultar la documentación en `tasks/Docs/` (ver `guia_compilacion.md`).

## Propio de este proyecto

- [`tasks/Docs/tap_dance_guide.md`](tasks/Docs/tap_dance_guide.md) — referencia de
  usuario sobre los tap dances del keymap. No hace falta leerla para
  trabajar, solo si necesitas entender ese comportamiento.
