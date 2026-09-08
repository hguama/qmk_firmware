# Especificación: Limpieza única de teclas

## 0. Manifiesto

| id | archivo | rol | estado |
|---|---|---|---|
| C1 | `spec-limpieza-teclas.md` | contrato | vigente · 2026-09-08 |

## 1. Objetivo

Salen 9 teclas muertas o reemplazadas y queda registrado por qué se queda el resto, incluidas las de pruebas de mouse.

Decisión Hero (2026-09-06): una sola tarea de limpieza. Absorbe las Tareas 4, 5, 5.1, 7, 9 y 10 (números retirados, no se reusan).

## 2. Detalle movido del formato anterior (verbatim)

| Tecla | Por qué se elimina |
|-------|--------------------|
| ❌ `SPLIT_WIN` | JetBrains sin uso en capas; ya no se usa JetBrains |
| ❌ `SHOW_QUICK_ENT` | JetBrains (`Alt+Enter`/`Ctrl+F1`); ya no se usa |
| ❌ `CODE_COMPLET` | JetBrains (autocompletado); ya no se usa |
| ❌ `SHIFT_TOGGLE` | Patrón duplicado con `ALT_TAB`; sin uso en capas |
| ❌ `TG_F22` | Pruebas de remapeo externo sin dueño; lógica frágil |
| ❌ `LT(KC_F22, KC_ENT)` | Pruebas F22/Enter con comentario rancio; fuera del enum |
| ❌ `CTRL_Z` | Sin uso; `Ctrl+Z` ya cubierto por `UNDO_WIN` y `C(KC_Z)` |
| ❌ `SHIFT_2` | Sin uso; `Ctrl+S` ya existe como `C(KC_S)` en `_FAST` |
| ❌ `MOUSE_HOLD` | Duplicada: el tap de `TD(TDQ_MOUSE_HOLD)` hace lo mismo + 3 gestos |

| Tecla(s) | Por qué se queda(n) |
|----------|---------------------|
| ✅ `P_ENIE`, `AMP_DOUBLE`, `DOUBLE_PIPE`, `PIPE_M`, `QUESTION`, `NOT_EQUAL`, `EQUAL_DBL`, `DOUBLE_COLON`, `LBRC2`, `LLAMBDA`, `ASTRISK_PLUS` | En uso en `_SYMB`/`_NUMB`/`_ALFA` |
| ✅ `COPY`, `CUT`, `SEL_ALL`, `DEL_WORD`, `DEL_LINE`, `UNDO_WIN`, `CTL_CLICK` | En uso (edición de texto) |
| ✅ `DOWN_10`, `UP_10`, `ALT_TAB`, `CLOSE_WIN`, `SLEEP`, `HIBERNATE`, `CS_F15_HOLD`, `TG_ALFA`, `MOVE_WIN_TG`, `MARKER_B`, `MARKER_2` | En uso (mouse, sistema, capas, marcadores) |
| ✅ `COMM` | Comodín de comentarios para otras apps |
| ✅ `MS_ACL0_TOGGLE`, `LT(MS_ACL2, KC_ENT)` | Pruebas de mouse: se necesitan después |
| ✅ `TDQ_SEL`, `TDQ_ESC`, `TDQ_PASTE`, `TDQ_MOUSE_HOLD`, `TDQ_BOOKMARK` | Tap dances vivos; `TDQ_SEL` reservado para el futuro reemplazo del espacio |

**Alcance al eliminar:**
- JetBrains: enum + `case` de las 3 y sus comentarios TODO.
- `SHIFT_TOGGLE`: enum + 2 `case` + `shift_active`/`shift_toggle_timer` + bloques en `matrix_scan_user`/`clear_all` + los 4 `if (shift_active)` de `COMM`.
- F22: enum `TG_F22` + 2 `case` + comentarios muertos de combos (`:113, :227-228`); `C(KC_F22)` de `_BOOK` intacto.
- `CTRL_Z`/`SHIFT_2`: enum + 2 `case`.
- `MOUSE_HOLD`: sustituir por `TD(TDQ_MOUSE_HOLD)` en `_MOVE` y eliminar el keycode, sin tocar `toggle_mouse_hold()` ni el `case MS_BTN1`.
- Actualizar README y anotar las líneas en `completed.md` al cerrar.
- `SEL_ALL = SAFE_RANGE + 14` quedó fijo en el enum: se usa como layer en `LT()` y renumerarlo colisiona con `MS_ACL0` (lo vimos en la compilación).
