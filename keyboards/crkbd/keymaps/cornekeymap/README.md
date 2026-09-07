# cornekeymap — Referencia de teclas personalizadas

Inventario de las teclas custom del keymap, organizado por categoría.

**Convención:** las teclas con `TAP | HOLD` hacen una cosa al tocar rápido y otra al mantener presionada.

---

## 📌 Estado de las teclas

**Limpieza Tarea 3 (2026-09-06):** se eliminaron `SPLIT_WIN`, `SHOW_QUICK_ENT`, `CODE_COMPLET`, `SHIFT_TOGGLE`, `TG_F22`, `LT(KC_F22, KC_ENT)`, `CTRL_Z`, `SHIFT_2` y `MOUSE_HOLD` (este último reemplazado por `TD(TDQ_MOUSE_HOLD)` en `_MOVE`).

**✅ Se quedan (sin uso en capas, con motivo):**

| Keycode        | Razón |
|----------------|-------|
| `COMM`         | Posible uso en otras apps (sin uso en capas por ahora) |
| `MS_ACL0_TOGGLE` | Pruebas de mouse con el teclado |
| `LT(MS_ACL2, KC_ENT)` | Pruebas de mouse con el teclado (fuera del enum) |
| `TDQ_SEL`      | Reservado: futuro reemplazo de `LT(SEL_ALL, KC_SPACE)` |

---

## 1. Letras especiales

| Keycode   | TAP | HOLD        | Capa   |
|-----------|-----|-------------|--------|
| `P_ENIE`  | `p` | `ñ` (Ñ con Shift) | `_ALFA` |

## 2. Puntuación y operadores

| Keycode        | TAP | HOLD     | Capas        |
|----------------|-----|----------|--------------|
| `AMP_DOUBLE`   |     | `&&`     | `_SYMB`      |
| `DOUBLE_PIPE`  |     | `\|\|`   | `_SYMB`      |
| `PIPE_M`       |     | `\|>`    | `_SYMB`      |
| `QUESTION`     | `?` | `¿`      | `_SYMB`      |
| `NOT_EQUAL`    | `!=`| `¡`      | `_SYMB`      |
| `EQUAL_DBL`    | `=` | `==`     | `_SYMB` |
| `DOUBLE_COLON` | `:` | `::`     | `_SYMB`      |
| `LBRC2`        | `[` | `]`      | `_SYMB`      |
| `LLAMBDA`      | `->`| `<-`     | `_SYMB`      |
| `ASTRISK_PLUS` | `*` | `+`      | `_SYMB` `_NUMB` |

## 3. Edición de texto

| Keycode    | TAP | HOLD        | Capas |
|------------|-----|-------------|-------|
| `COPY`     | `Ctrl+C` copiar | `Ctrl+X` cortar (junto con `CUT`) | `_BASE` `_MOVE` |
| `CUT`      | — (capa del LT) | `Ctrl+X` cortar | `_BASE` `_MOVE` |
| `SEL_ALL`  | espacio | `Ctrl+A` seleccionar todo (doble tap + hold: repite espacio) | `_BASE` `_MOVE` `_ALFA` `_FAST` |
| `DEL_WORD` | borra la palabra completa | | `_DEL` |
| `DEL_LINE` | borra la línea completa | | `_DEL` |
| `UNDO_WIN` | `Ctrl+Z` deshacer | tecla Windows | `_BASE` `_MOVE` `_ALFA` `_FAST` |

## 4. Mouse y scroll

| Keycode          | Acción | Capas |
|------------------|--------|-------|
| `CTL_CLICK`      | `Ctrl` + clic izquierdo (abrir enlace en pestaña nueva) | `_BASE` |
| `MS_ACL0_TOGGLE` 🔬 | toggle de aceleración del mouse | pruebas |
| `DOWN_10`        | rueda hacia abajo (8 ticks) | `_FAST` |
| `UP_10`          | rueda hacia arriba (8 ticks) | `_FAST` |

## 5. Ventanas y sistema

| Keycode        | TAP | HOLD | Capas |
|----------------|-----|------|-------|
| `ALT_TAB`      | `Alt+Tab` (cambiar ventana) | | `_BASE` `_MOVE` |
| `CLOSE_WIN`    | `Ctrl+W` cerrar pestaña | `Alt+F4` cerrar ventana | `_BASE` `_MOVE` |
| `SLEEP`        | suspender equipo (`Win+X → U → S`) | | `_BASE` `_MOVE` |
| `HIBERNATE`    | hibernar equipo (`Win+X → U → H`) | | `_BASE` `_MOVE` `_ALFA` |
| `CS_F15_HOLD`  | mantiene `Ctrl+Shift+F15` (remapeo externo, ej. PowerToys) | | `_FAST` |

## 6. Capas y modificadores

| Keycode        | TAP | HOLD | Capa de uso |
|----------------|-----|------|-------------|
| `TG_ALFA`      | alterna entre `_MOVE` (ratón) y `_ALFA` (letras) | | `_MOVE` |
| `MOVE_WIN_TG`  | toggle `_MOVE_WIN` | capa `_AI` (guarda y restaura al soltar) | `_BASE` |

## 7. IDE / IA (JetBrains y código)

| Keycode          | TAP | HOLD | Capa de uso |
|------------------|-----|------|-------------|
| `COMM` ✅         | `Ctrl+/` comentar línea | `Ctrl+Shift+/` comentar bloque | — |

## 8. Marcadores / bookmarks

| Keycode   | TAP | HOLD | Capa de uso |
|-----------|-----|------|-------------|
| `MARKER_B`| `Ctrl+Shift+F21` | capa `_BOOK_2` | `_BOOK` |
| `MARKER_2`| `Ctrl+2` | capa `_BOOK_2` | `_BOOK` |

---

## Tap dances

| Tap dance      | Tap | Hold | Doble tap | Doble hold |
|----------------|-----|------|-----------|------------|
| `TDQ_SEL`      | `Ctrl+A` seleccionar todo | seleccionar línea | seleccionar palabra | — |
| `TDQ_BOOKMARK` | `Shift+F11` lista de marcadores | `Alt+2` lista de marcadores | `F11` crear marcador | `Ctrl+Shift+F11` ir al marcador |
| `TDQ_ESC`      | `Esc` (libera clic sostenido) | `Win+D` minimizar todo | clic derecho | — |
| `TDQ_PASTE`    | `Ctrl+V` pegar | `F5` recargar | `F11` pantalla completa | `Win+V` portapapeles |
| `TDQ_MOUSE_HOLD` | clic izquierdo sostenido (toggle) | recorte de pantalla + clic sostenido | `Ctrl+F` buscar | `Alt+PrtScr` capturar ventana |

---

## Capas

| Capa         | Propósito |
|--------------|-----------|
| `_BASE`      | Capa principal (ratón, navegación, sistema) |
| `_MOVE`      | Movimiento de ratón y edición rápida |
| `_ALFA`      | Letras y acceso a símbolos/números |
| `_AI`        | Atajos para herramientas de IA |
| `_DEL`       | Borrado de texto |
| `_SYMB`      | Símbolos y puntuación |
| `_NUMB`      | Números |
| `_BOOK`      | Marcadores (bookmarks) |
| `_BOOK_2`    | Marcadores nivel 2 |
| `_FAST`      | Acciones rápidas y scroll |
| `_MOVE_WIN`  | Mover ventanas |
| `_RUN`       | Ejecutar/atender (IDE) |
| `_MOUSE_KEY` | Botones extra del ratón |
