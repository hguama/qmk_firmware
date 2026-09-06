# cornekeymap — Referencia de teclas personalizadas

Inventario de las teclas custom del keymap, organizado por categoría.

**Convención:** las teclas con `TAP | HOLD` hacen una cosa al tocar rápido y otra al mantener presionada.

> ⚠️ Las teclas marcadas con ⚠️ tienen una **evaluación pendiente**: ver [Estado de las teclas](#estado-de-las-teclas).

---

## 📌 Estado de las teclas

**✅ Se quedan:**

| Keycode        | Razón |
|----------------|-------|
| `COMM`         | Posible uso en otras apps (sin uso en capas por ahora) |
| `SHIFT_TOGGLE` | Plantilla de toggle con auto-release (10 s); `ALT_TAB` usa el mismo patrón (20 s) |

**🔬 Para pruebas (no están en capas):**

| Keycode          | Razón |
|------------------|-------|
| `MS_ACL0_TOGGLE` | Pruebas con teclado (aceleración del mouse) |
| `TG_F22`         | Pruebas (toggle/momentáneo de F22, remapeo externo) |

**⚠️ Pendientes de evaluar (posible eliminación):**

| Keycode          | Razón |
|------------------|-------|
| `SHOW_QUICK_ENT` | ¿Se usa JetBrains? Está en `_MOVE` |
| `CODE_COMPLET`   | ¿Se usa JetBrains? Está en `_MOVE` |
| `CTRL_Z`         | No se usa en ninguna capa; redundante con `UNDO_WIN`/`C(KC_Z)` |
| `SHIFT_2`        | No se usa en ninguna capa |
| `SPLIT_WIN`      | No se usa en ninguna capa |

**Fuera del enum** (cases con lógica pero sin uso en capas): `LT(KC_F22, KC_ENT)` y `LT(MS_ACL2, KC_ENT)` (pruebas).

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
| `EQUAL_DBL`    | `=` | `==`     | `_SYMB` `_NUMB` |
| `DOUBLE_COLON` | `:` | `::`     | `_SYMB`      |
| `LBRC2`        | `[` | `]`      | `_SYMB`      |
| `LLAMBDA`      | `->`| `<-`     | `_SYMB`      |
| `ASTRISK_PLUS` | `*` | `+`      | `_SYMB` `_NUMB` |

## 3. Edición de texto

| Keycode    | TAP | HOLD        | Capas |
|------------|-----|-------------|-------|
| `COPY`     | `Ctrl+C` copiar | `Ctrl+X` cortar (junto con `CUT`) | `_BASE` `_MOVE` |
| `CUT`      | — (capa del LT) | `Ctrl+X` cortar | `_BASE` `_MOVE` |
| `CTRL_Z` ⚠️ | `Ctrl+Z` deshacer | capa `_BOOK` | — |
| `SEL_ALL`  | espacio | `Ctrl+A` seleccionar todo (doble tap + hold: repite espacio) | `_BASE` `_MOVE` `_ALFA` `_FAST` |
| `DEL_WORD` | borra la palabra completa | | `_DEL` |
| `DEL_LINE` | borra la línea completa | | `_DEL` |
| `UNDO_WIN` | `Ctrl+Z` deshacer | tecla Windows | `_BASE` `_MOVE` `_ALFA` `_FAST` |
| `SHIFT_2` ⚠️ | `Ctrl+S` guardar | Shift sostenido | — |

## 4. Mouse y scroll

| Keycode          | Acción | Capas |
|------------------|--------|-------|
| `MOUSE_HOLD`     | mantiene el clic izquierdo sostenido (toggle) | `_MOVE` |
| `CTL_CLICK`      | `Ctrl` + clic izquierdo (abrir enlace en pestaña nueva) | `_BASE` |
| `MS_ACL0_TOGGLE` 🔬 | toggle de aceleración del mouse | pruebas |
| `DOWN_10`        | rueda hacia abajo (8 ticks) | `_FAST` |
| `UP_10`          | rueda hacia arriba (8 ticks) | `_FAST` |

## 5. Ventanas y sistema

| Keycode        | TAP | HOLD | Capas |
|----------------|-----|------|-------|
| `ALT_TAB`      | `Alt+Tab` (cambiar ventana) | | `_BASE` `_MOVE` |
| `CLOSE_WIN`    | `Ctrl+W` cerrar pestaña | `Alt+F4` cerrar ventana | `_BASE` `_MOVE` |
| `SPLIT_WIN` ⚠️ | `F16` split derecha | `F17` split abajo | JetBrains |
| `SLEEP`        | suspender equipo (`Win+X → U → S`) | | `_BASE` `_MOVE` |
| `HIBERNATE`    | hibernar equipo (`Win+X → U → H`) | | `_BASE` `_MOVE` |
| `CS_F15_HOLD`  | mantiene `Ctrl+Shift+F15` (remapeo externo, ej. PowerToys) | | `_FAST` |
| `TG_F22` 🔬     | `F22`: tap corto = toggle, hold largo = momentáneo (remapeo externo) | | — |

## 6. Capas y modificadores

| Keycode        | TAP | HOLD | Capa de uso |
|----------------|-----|------|-------------|
| `TG_ALFA`      | alterna entre `_MOVE` (ratón) y `_ALFA` (letras) | | `_MOVE` |
| `MOVE_WIN_TG`  | toggle `_MOVE_WIN` | capa `_AI` (guarda y restaura al soltar) | `_BASE` |
| `SHIFT_TOGGLE` ✅ | bloquea Shift; se suelta solo tras 10 s (plantilla) | | — |

## 7. IDE / IA (JetBrains y código)

| Keycode          | TAP | HOLD | Capa de uso |
|------------------|-----|------|-------------|
| `SHOW_QUICK_ENT` ⚠️ | `Alt+Enter` acción rápida | `Ctrl+F1` descripción de error | `_MOVE` |
| `CODE_COMPLET` ⚠️ | `Ctrl+Espacio` autocompletado | `Ctrl+Shift+Espacio` avanzado | `_MOVE` |
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