# 📝 Tareas Pendientes — Corne Keymap

Documento para llevar un orden claro de las tareas que van surgiendo durante
la revisión del keymap. Cada tarea debe completarse y luego marcarse.

---

## ✅ Cómo usar este documento

- Marcar con `[x]` cuando una tarea esté completa.
- Agregar fecha y nota breve al completarla.
- Si una tarea se descarta, tacharla y explicar por qué.

---

## 1. Revisar capas obsoletas (JetBrains)

**Estado:** 🔄 En progreso

**Descripción:** Muchas capas fueron creadas pensando en flujos de trabajo con
JetBrains/IntelliJ. Con el uso de IA, algunas pueden ser innecesarias.

**Clasificación actual:**

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
| `_MOVE_H` | 10 | 🗑️ Eliminar | Vacía + keycode huérfano |
| `_FAST` | 11 | ✅ Se queda | Scroll rápido |
| `_MOVE_WIN` | 12 | ✅ Se queda | Ventanas |
| `_MOVE_L` | 13 | 🗑️ Eliminar | Mover líneas de código |
| `_RUN` | 14 | ✅ Se queda | Ejecutar (Flutter/Java) |
| `_MODE` | 15 | 🗑️ Eliminar | Juego/navegación |
| `_COMMIT` | 16 | 🗑️ Eliminar | Git obsoleto |
| `_MOUSE_KEY` | 17 | ✅ Se queda | Mouse con teclado |
| `_NEW` | 18 | ❌ Eliminada | Not used |

**Acción:** Eliminar las 3 capas restantes marcadas 🗑️ (`_MOVE_H`, `_MODE`, `_COMMIT`).

---

## 2. Refactorizar helper de capa momentánea

**Estado:** ⬜ Pendiente

**Descripción:** Hay dos patrones repetidos que hacen lo mismo:
- `del_prev_layer` / `del_layer_active` (4 teclas)
- `base_prev_layer` / `base_layer_active` (2 teclas)

**Teclas involucradas:**
| Tecla | Capa objetivo |
|-------|---------------|
| `LT(_AI, TG_0)` | `_AI` |
| `LT(_RUN, MS_BTN2)` | `_RUN` |
| `LT(_NEW, _MOUSE_KEY)` | `_NEW` |
| `LT(_DEL, KC_PERC)` | `_DEL` |
| `LT(_BASE, KC_I)` | `_MOVE` |
| `LT(_BASE, KC_O)` | `_MOVE` |

**Acción:** Extraer a funciones helper (ej. `layer_move_momentary()` y
`layer_restore_previous()`) y refactorizar las 6 teclas.

---

## 3. Evaluar keycodes conservados de JetBrains

**Estado:** ⬜ Pendiente

**Descripción:** Durante la limpieza se conservaron 2 keycodes con el
comentario "Evaluar si se necesita (JetBrains)".

| Keycode | Nota |
|---------|------|
| `COMM` | Atajo de comentarios (JetBrains) |
| `SPLIT_WIN` | División de ventanas (JetBrains) |

**Acción:** Probar si se usan; si no, eliminarlos.

---

## 4. Evaluar keycodes de prueba de teclado

**Estado:** ⬜ Pendiente

**Descripción:** Se conservaron 2 keycodes con el comentario
"Para pruebas con teclado".

| Keycode | Nota |
|---------|------|
| `MS_ACL0_TOGGLE` | Toggle de aceleración de mouse |
| `LT(MS_ACL2, KC_ENT)` | Mouse + Enter |

**Acción:** Confirmar si siguen siendo útiles para pruebas.

---

## 5. Reemplazar `MOUSE_HOLD` por `TDQ_MOUSE_HOLD`

**Estado:** ⬜ Pendiente

**Descripción:** Se creó el tap dance `TDQ_MOUSE_HOLD`, pero el keycode
`MOUSE_HOLD` original **aún existe** en el código y en los layouts. Falta
reemplazarlo definitivamente.

**Acción:** Sustituir `MOUSE_HOLD` por `TD(TDQ_MOUSE_HOLD)` en los layouts y
eliminar el keycode `MOUSE_HOLD` cuando se confirme.

---

## 6. Limpiar `LT(_DEV, KC_ENT)` muerto

**Estado:** ⬜ Pendiente

**Descripción:** El `case LT(_DEV, KC_ENT)` en `process_record_user` existe,
pero `LT(_DEV, KC_ENT)` no está en ningún layout (código muerto).

**Acción:** Eliminarlo (o confirmar si se usará).

---

## 📌 Notas

- Este documento se irá actualizando a medida que surjan nuevas tareas.
- Las tareas 1 y 6 están relacionadas (la capa `_DEV`).
