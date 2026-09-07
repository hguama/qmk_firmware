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
<summary>[ ] ⬜ <b>Tarea 3</b> · Limpieza única de teclas (9 fuera, resto se queda) — ⚠️ riesgo medio · agente</summary>

**Riesgo:** ⚠️ medio — incluye el reemplazo de `MOUSE_HOLD` en un layout de uso
diario con lógica compartida con `MS_BTN1`.

**Decisión Hero (2026-09-06):** una sola tarea de limpieza. Absorbe las Tareas
4, 5, 5.1, 7, 9 y 10 (números retirados, no se reusan).

**Descripción:** Salen 9 teclas muertas o reemplazadas y queda registrado por
qué se queda el resto, incluidas las de pruebas de mouse.

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

- [x] Eliminar las 9 teclas según el alcance y sustituir `MOUSE_HOLD` en `_MOVE`
- [x] Actualizar README (tablas de estado y categorías)
- [x] Compilar con `qmk compile -kb crkbd/rev1 -km cornekeymap` (.hex OK 2026-09-06, 24.126/28.672)
- [ ] 👤 Flashear y probar: 4 gestos de `TDQ_MOUSE_HOLD` + liberación con `MS_BTN1`; `ALT_TAB`, `COMM`, espacio y `Ctrl+Z` intactos

</details>

<details>
<summary>[ ] ⬜ <b>Tarea 11</b> · Confirmar Alt+Tab con el pulgar derecho (espacio → Enter) — ⚠️ riesgo medio · agente</summary>

**Riesgo:** ⚠️ medio — toca el espacio (uso diario) y el estado compartido de
`ALT_TAB`; un error se nota al instante y se revierte fácil.

**Descripción:** Con la izquierda el pulgar confirma la ventana con Enter; con
la derecha el pulgar es espacio y no confirma. Idea de Hero: mientras `ALT_TAB`
está activo, el tap de espacio manda Enter (efecto espejo).

**Sugerencia de implementación:** en `process_record_user`, en los `case` de
espacio (`KC_SPACE`, `LT(MS_ACL0, KC_SPACE)`, `LT(SEL_ALL, KC_SPACE)`), si
`is_alt_tab_active` y es tap: soltar `KC_LALT`, bajar el flag, avisar `AT_OFF`
y mandar `KC_ENT`. Solo el tap se redirige; el hold conserva su conducta. Así
Alt no queda pegado tras confirmar.

**Pruebas:**

1. `qmk compile -kb crkbd/rev1 -km cornekeymap`
2. Flashear y probar: `ALT_TAB` con la derecha + pulgar confirma la ventana;
   espacio normal intacto; Alt no queda pegado tras confirmar.

**Resultado esperado:** el pulgar derecho confirma igual que el izquierdo y
fuera del modo ventana nada cambia.

</details>

<details>
<summary>[ ] ⬜ <b>Tarea 12</b> · Captura en un paso: validar evento de Windows para el clic sostenido — △ riesgo bajo · agente</summary>

**Riesgo:** △ bajo — solo investigar y decidir; implementar viene después.

**Descripción:** El hold de `TDQ_MOUSE_HOLD` (ImprPant + espera fija + clic
sostenido) es inestable por tiempos. Idea de Hero: que Windows avise cuando el
overlay de Recortes esté listo y el teclado enganche el clic ahí. Hay que
validar si ese evento existe.

**Hipótesis a validar:**
- El overlay de Recortes expone ventana detectable (clase/título) vía Win32 desde Python, y el canal `raw_hid` existente (ya usado para `R` y `M`) avisa al teclado a tiempo.
- Alternativas si no: `Win+Shift+S` en vez de `ImprPant` (overlay directo, más determinista) o dos taps explícitos sin tiempos.

- [ ] Validar ventana detectable del overlay con Python+ctypes y medir latencia del aviso `raw_hid`
- [ ] Comparar contra `Win+Shift+S` y dos taps; dejar recomendación escrita
- [ ] 👤 Hero: aprobar la estrategia elegida (la implementación será la Tarea 12.1)

**Adjunto — estrategia validada (2026-09-06, sin implementar):**
- Diagnóstico: el hold actual (ImprPant + 300 ms fijos + enganche) falla si el overlay tarda distinto; `wait_ms` congela el teclado; ImprPant depende del ajuste de Windows.
- Camino: watcher Python detecta el overlay por su huella (clase/título, por medir en tu PC) y manda letra `S` por el `raw_hid` existente (ya usado para `R` y `M`); el firmware engancha `MS_BTN1` al recibirla. El hold manda `Win+Shift+S` (overlay directo, determinista) y arma sin enganchar.
- Red obligatoria: si la `S` no llega en ~800 ms, enganchar igual — el peor caso queda idéntico a hoy, nunca peor.
- Riesgos: la firma puede cambiar con updates de Windows (mitiga con 2-3 alias + red); helper apagado (cubre la red); overlay cancelado con Esc (se libera con la tecla o `MS_BTN1`, como hoy).
- Veredicto: estable por diseño (fallback determinista). Técnica `EnumWindows` ya probada en tu máquina (313 ventanas listadas).
- Pendiente Fase A: script de huella — Hero presiona `Win+Shift+S` una vez y el script anota clase/título, sin abrir nada solo.

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
