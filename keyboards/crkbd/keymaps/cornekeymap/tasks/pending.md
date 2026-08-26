****# 📝 Tareas pendientes — Corne Keymap

Plan de trabajo vivo. Aquí solo vive lo abierto.
Las tareas cerradas se mudan completas a [`completed.md`](completed.md).

---

## 🎯 En foco — Tarea 2.1 · Probar las dos teclas refactorizadas

**Estado:** [?] 👤 esperando a Hero
**Dueño:** Hero
**Riesgo:** △ bajo — es verificación, no cambia código
**Depende de:** Tarea 2 (refactor ya completado)

**Descripción:** El refactor del helper de capa momentánea está hecho pero sin
verificar en el teclado. Va primero porque el trabajo sin probar se acumula como
deuda silenciosa.

**Pruebas:**

1. `qmk compile -kb crkbd/rev1 -km cornekeymap`
2. Flashear y probar:
   - `LT(_AI, MOVE_WIN_TG)` — en `_BASE`, fila 2 col 0:
     - HOLD: entra a `_AI` y vuelve a la capa previa al soltar.
     - TAP: alterna `_MOVE_WIN`.
   - `LT(_DEL, KC_PERC)` — en `_NUMB`, fila 2 col 1:
     - HOLD: entra a `_DEL` y vuelve a la capa previa al soltar.
     - TAP: escribe `%`.

**Resultado esperado:** ambas teclas se comportan igual que antes del refactor.

---

## ➡️ Siguiente — Tarea 6 · Limpiar `LT(_DEV, KC_ENT)` muerto

**Estado:** [ ] ⬜ pendiente · **Dueño:** agente · **Riesgo:** △ bajo

Código muerto en `process_record_user`, no está en ningún layout. Antes de
tocarlo hay que confirmar qué quedó exactamente ahí: la capa `_DEV` se eliminó
del enum en la Tarea 1, así que un `case` que la nombre no debería compilar.

---

## 🕒 En cola

<details>
<summary>[ ] ⬜ <b>Tarea 4</b> · Evaluar keycodes de prueba de teclado — △ riesgo bajo · Hero</summary>

**Riesgo:** △ bajo — solo decidir; eliminar viene después.

**Descripción:** Se conservaron 2 keycodes marcados como "para pruebas con
teclado".

| Keycode | Nota |
|---------|------|
| `MS_ACL0_TOGGLE` | Toggle de aceleración de mouse |
| `LT(MS_ACL2, KC_ENT)` | Mouse + Enter |

**Acción:** Hero confirma si siguen siendo útiles. Si no, se abre la Tarea 4.1
para eliminarlos.

</details>

<details>
<summary>[ ] ⬜ <b>Tarea 3</b> · Evaluar keycodes conservados de JetBrains — △ riesgo bajo · Hero</summary>

**Riesgo:** △ bajo — solo decidir; eliminar viene después.

**Descripción:** Durante la limpieza se conservaron 2 keycodes con el comentario
"Evaluar si se necesita (JetBrains)".

| Keycode | Nota |
|---------|------|
| `COMM` | Atajo de comentarios |
| `SPLIT_WIN` | División de ventanas |

**Acción:** Hero prueba si los usa. Si no, se abre la Tarea 3.1 para eliminarlos.

</details>

<details>
<summary>[ ] ⬜ <b>Tarea 5</b> · Reemplazar MOUSE_HOLD por TDQ_MOUSE_HOLD — ⚠️ riesgo medio · agente</summary>

**Riesgo:** ⚠️ medio — toca layouts de una tecla de uso diario y comparte lógica
con `MS_BTN1` en `process_record_user`.

**Descripción:** El tap dance `TDQ_MOUSE_HOLD` ya existe y replica el
comportamiento de `MOUSE_HOLD` en el tap, pero el keycode original sigue vivo en
el código y en los layouts.

**Por qué es el más riesgoso de los pendientes:** `MOUSE_HOLD` se libera
presionando `MS_BTN1`, no la misma tecla. Esa interacción vive en
`process_record_user` y la comparten ambos keycodes a través de
`toggle_mouse_hold()`. Al retirar el original hay que confirmar que la lógica de
liberación sigue intacta.

**Acción:** sustituir `MOUSE_HOLD` por `TD(TDQ_MOUSE_HOLD)` en los layouts y
eliminar el keycode una vez confirmado.

</details>

<details>
<summary>[ ] ⬜ <b>Tarea 5.1</b> · Probar el reemplazo de MOUSE_HOLD — △ riesgo bajo · Hero</summary>

**Riesgo:** △ bajo — verificación. **Depende de:** Tarea 5.

**Pruebas:**

1. `qmk compile -kb crkbd/rev1 -km cornekeymap`
2. Flashear y probar los cuatro gestos: tap (sostener clic izquierdo), hold
   (Impr Pant), doble tap (`Ctrl+F`), doble tap + hold (`Alt+Impr Pant`).
3. Confirmar que el clic sostenido se libera con `MS_BTN1`, como antes.

**Resultado esperado:** el comportamiento diario no cambia y los tres gestos
extra funcionan.

</details>

---

## 📖 Convenciones

**Estados**

| Corchete | Icono | Estado | Significa |
|----------|-------|--------|-----------|
| `[ ]` | ⬜ | pendiente | Definida, aún no se toca |
| `[~]` | ▶️ | en curso | Es la tarea en foco ahora mismo |
| `[!]` | ⏸️ | en pausa | Se empezó y se dejó; lleva nota de dónde quedó |
| `[?]` | 👤 | esperando a Hero | El agente terminó su parte; falta compilar, probar o aprobar |
| `[x]` | ✅ | completada | Cerrada, con fecha |
| `[-]` | ❌ | descartada | Ya no se hará; lleva el motivo |

**Riesgo**

| Icono | Nivel |
|-------|-------|
| △ | bajo |
| ⚠️ | medio |
| ❗ | alto |

**Reglas**

- Los números de tarea nunca se reusan ni se renumeran.
- Toda tarea lleva dueño (`agente` o `Hero`) y riesgo con su razón, marcado
  con su icono de advertencia graduado.
- El estado siempre lleva corchete + icono juntos, ese orden: el corchete se
  busca y se modifica fácil, el icono se escanea a simple vista.
- Solo una tarea **En foco** y una en **Siguiente**; el resto va colapsado en la cola.
- El orden va de menor a mayor riesgo, con dos excepciones: la verificación
  pendiente va primero, y las dependencias mandan.
- Cambiar de tarea siempre se permite: la que se deja pasa a `[!]` ⏸️ con
  nota y baja a la cola.
- Al cerrar: fecha obligatoria y decidir si el aprendizaje va a `fixes.md` o a
  `implementations.md`.
- En tablas de decisión binaria (se queda / se elimina), la columna lleva ✅
  o ❌ delante del texto.
