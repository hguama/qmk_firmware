# ✅ Tareas completadas — Corne Keymap

Histórico del trabajo cerrado, con su detalle y análisis de riesgo intactos.
El plan vivo está en [`pending.md`](pending.md).

---

## Tarea 1 · Revisar capas obsoletas (JetBrains)

**Estado:** [x] ✅ completada · 2026-08-14
**Dueño:** agente
**Riesgo:** ⚠️ medio — eliminar capas toca el enum y los layouts

**Descripción:** Muchas capas fueron creadas pensando en flujos de trabajo con
JetBrains/IntelliJ. Con el uso de IA, algunas resultaron innecesarias.

**Clasificación:**

| Capa | Índice | Decisión | Nota |
|------|--------|----------|------|
| `_BASE` | 0 | ✅ Se queda | Principal |
| `_MOVE` | 1 | ✅ Se queda | Navegación |
| `_ALFA` | 2 | ✅ Se queda | Escritura |
| `_AI` | 3 | ✅ Se queda | Atajos IA |
| `_DEV` | 4 | ❌ Eliminada | Capa fantasma |
| `_DEL` | 5 | ✅ Se queda | Borrar |
| `_SYMB` | 6 | ✅ Se queda | Símbolos |
| `_NUMB` | 7 | ✅ Se queda | Números |
| `_BOOK` | 8 | ✅ Se queda | Marcadores |
| `_BOOK_2` | 9 | ✅ Se queda | Marcadores |
| `_MOVE_H` | 10 | ❌ Eliminada | Vacía + keycode huérfano |
| `_FAST` | 11 | ✅ Se queda | Scroll rápido |
| `_MOVE_WIN` | 12 | ✅ Se queda | Ventanas |
| `_MOVE_L` | 13 | ❌ Eliminada | Mover líneas de código |
| `_RUN` | 14 | ✅ Se queda | Ejecutar (Flutter/Java) |
| `_MODE` | 15 | ❌ Eliminada | Juego/navegación |
| `_COMMIT` | 16 | ❌ Eliminada | Git obsoleto |
| `_MOUSE_KEY` | 17 | ✅ Se queda | Mouse con teclado |
| `_NEW` | 18 | ❌ Eliminada | Not used |

**Resultado:** se eliminaron `_MOVE_H`, `_MOVE_L`, `_MODE` y `_COMMIT`, además
de `_DEV` y `_NEW`.

### Tarea 1.1 · Reordenar índices de capas (contiguos)

**Estado:** [x] ✅ completada · 2026-08-14
**Dueño:** agente
**Riesgo:** ❗ alto — tocar índices de capas puede romper cualquier `LT()`,
`MO()` o `TG()` que dependa de un valor fijo

**Objetivo:** compactar los índices del `enum layer_names` a 0..12 sin huecos.

**Mapeo final:**

| Capa | Índice anterior | Índice final |
|------|-----------------|--------------|
| `_BASE` | 0 | 0 |
| `_MOVE` | 1 | 1 |
| `_ALFA` | 2 | 2 |
| `_AI` | 3 | 3 |
| `_DEL` | 5 | 4 |
| `_SYMB` | 6 | 5 |
| `_NUMB` | 7 | 6 |
| `_BOOK` | 8 | 7 |
| `_BOOK_2` | 9 | 8 |
| `_FAST` | 11 | 9 |
| `_MOVE_WIN` | 12 | 10 |
| `_RUN` | 14 | 11 |
| `_MOUSE_KEY` | 17 | 12 |

**Análisis de riesgo (resultó bajo tras el análisis):**

- ✅ El código vivo usa **nombres** de enum (`LT(_AI, ...)`, `MO(_DEL)`,
  `TG(_MOVE)`), no números fijos; se actualizan solos al reordenar.
- ✅ El orden relativo se conserva, así que la prioridad entre capas no cambia.
- ✅ `_BASE` sigue en el índice 0, por lo que la capa por defecto guardada en
  EEPROM no se ve afectada.
- ⚠️ Quedaban índices fijos en código muerto: `case TG(2):` y `case TG(6):` en
  `process_record_user`, y el bloque RGBLIGHT comentado.
- ✅ Beneficio: todas las capas quedan ≤ 12, así que `LT()` funciona con
  cualquiera. Antes `_MOUSE_KEY = 17` no podía usarse con `LT()`.

**Resultado:** enum reordenado a 0..12, handlers muertos `TG(2)`/`TG(6)`
eliminados y bloque RGBLIGHT comentado actualizado.

**Aprendizaje guardado:** el límite de 4 bits de `LT()` quedó documentado en
[`fixes.md`](fixes.md) → *"LT() no activa la capa correcta (capas > 15)"*.

### Tarea 1.2 · Compilar y probar capas reordenadas

**Estado:** [x] ✅ completada · 2026-08-14
**Dueño:** Hero
**Riesgo:** △ bajo — verificación

**Pruebas realizadas:** `_DEL` (índice 5 → 4) y `_MOUSE_KEY` (17 → 12).

**Resultado:** pruebas exitosas, ambas capas funcionan correctamente.

---

## Tarea 2 · Refactorizar helper de capa momentánea

**Estado:** [x] ✅ completada · 2026-08-14
**Dueño:** agente
**Riesgo:** ⚠️ medio — toca teclas vivas del layout base

**Descripción:** se extrajo el patrón repetido de "guardar capa previa → mover a
capa exclusiva → restaurar al soltar" a dos funciones helper:

- `save_current_layer()` — guarda
- `restore_saved_layer()` — restaura

El "mover" se hace con `layer_move()` de QMK directamente.

**Resultado:**

- Se refactorizaron las 2 teclas vivas que usaban el patrón:
  `LT(_AI, MOVE_WIN_TG)` → `_AI` y `LT(_DEL, KC_PERC)` → `_DEL`.
- Se eliminaron los handlers muertos que no estaban en ningún layout:
  `LT(_RUN, MS_BTN2)`, `LT(_BASE, KC_I)`, `LT(_BASE, KC_O)` y `LT(_DEL, TG_0)`.
- Se eliminaron las variables muertas `base_prev_layer` / `base_layer_active`.
- Se corrigió el comentario de `LT(_AI, MOVE_WIN_TG)`: togglea `_MOVE_WIN`,
  no `_MOVE`.

**Aprendizaje guardado:** el patrón quedó como **referencia protegida** en
[`implementations.md`](implementations.md) → *"Patrón: Tap alterna capa / Hold
capa momentánea"*.

**Verificación:** completada en la Tarea 2.1 (2026-09-08).

### Tarea 2.1 · Probar las dos teclas refactorizadas

**Estado:** [x] ✅ completada · 2026-09-08
**Dueño:** Hero
**Riesgo:** △ bajo — verificación

**Pruebas realizadas:** HOLD/TAP de `LT(_AI, MOVE_WIN_TG)` y `LT(_DEL, KC_PERC)`.

**Resultado:** ambas teclas se comportan igual que antes del refactor. Deuda de verificación saldada.

---

## Tarea 11 · Confirmar Alt+Tab con el pulgar derecho (espacio → Enter)

**Estado:** [-] ❌ cancelada · 2026-09-08
**Dueño:** agente
**Riesgo:** ⚠️ medio — tocaba el espacio (uso diario) y el estado compartido de `ALT_TAB`

**Descripción:** Con la izquierda el pulgar confirma la ventana con Enter; con la derecha el pulgar es espacio y no confirma. Se iba a implementar un espejo espacio→Enter en el firmware (solo el tap redirigido si `is_alt_tab_active`, el hold intacto).

**Motivo de cancelación:** no hizo falta ningún cambio de firmware. El espacio ya selecciona la ventana en el selector: una funcionalidad de PowerToys interceptaba el espacio sobre las ventanas abiertas y, al desactivarla, el flujo funciona (Alt+Tab, desplazarse, espacio selecciona).

**Aprendizaje:** si el espacio hace cosas raras en el selector de ventanas, revisar PowerToys antes de tocar el firmware. Queda aquí, en el histórico de la tarea.
