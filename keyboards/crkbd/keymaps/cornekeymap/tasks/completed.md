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

**Aprendizaje:** ninguno aparte — el patrón queda documentado en el propio
código (`save_current_layer()`/`restore_saved_layer()` y su uso en
`LT(_AI, MOVE_WIN_TG)`/`LT(_DEL, KC_PERC)`), sin necesidad de una referencia
protegida aparte en `implementations.md` (retirado, Task 17 de `task-manager`).

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

---

## Tarea 6 · Limpiar `LT(_DEV, KC_ENT)` muerto

**Estado:** [x] ✅ completada · 2026-09-08
**Dueño:** agente + Hero
**Riesgo:** △ bajo — limpieza de código muerto

**Descripción:** código muerto en `process_record_user`, fuera de todo layout. La capa `_DEV` se eliminó del enum en la Tarea 1.

**Resultado:** no quedaba nada que retirar — cero menciones de `_DEV` en el código (ni enum, ni `case`, ni layouts) y compilación OK 24.126/28.672 sin tocar el firmware.

---

## Tarea 3 · Limpieza única de teclas (10 fuera, resto se queda)

**Estado:** [x] ✅ completada · 2026-09-08
**Dueño:** agente + Hero
**Riesgo:** ◆ medio — tocó teclas vivas y código compartido

**Descripción:** salen 10 teclas muertas o reemplazadas y queda registrado por qué se queda el resto. Decisión Hero 2026-09-06: absorbe 4, 5, 5.1, 7, 9 y 10. Decisión Hero 2026-09-08: también fuera `COMM`. Detalle en el contrato (`attachments/limpieza-teclas/spec-limpieza-teclas.md`).

**Eliminadas:** `SPLIT_WIN`, `SHOW_QUICK_ENT`, `CODE_COMPLET`, `SHIFT_TOGGLE`, `TG_F22`, `LT(KC_F22, KC_ENT)`, `CTRL_Z`, `SHIFT_2`, `MOUSE_HOLD` (sustituida por `TD(TDQ_MOUSE_HOLD)` en `_MOVE`) y `COMM`.

**Líneas:** commits `b6d80013` (9 teclas: `keymap.c` −215, README −43) y `fad92177` (`COMM`: enum + `case LT(0,COMM)`). Verificado cero restos en el código salvo el tap dance vivo `TDQ_MOUSE_HOLD`; `KC_COMM` y `C(KC_F22)` intactos.

**Resultado:** firmware 24.046/28.672, flasheado y probado por Hero: 4 gestos de `TDQ_MOUSE_HOLD` + liberación con `MS_BTN1`; `ALT_TAB`, espacio y `Ctrl+Z` intactos.

---

## Tarea 12 · Captura en un paso: validar evento de Windows para el clic sostenido

**Estado:** [x] ✅ completada · 2026-09-09
**Dueño:** agente + Hero
**Riesgo:** △ bajo — solo medición, sin tocar firmware

**Descripción:** validar si Windows avisa cuando el overlay de Recortes está listo, para enganchar ahí el clic sostenido en vez de adivinar con espera fija. Detalle en el contrato (`attachments/captura/spec-captura-evento.md` §§1-5).

**Resultado:** espera fija de 500ms estable (a ~300ms inestable); huella del overlay medida (`SnipOverlayRootWindow`/`Snipping Tool Overlay`/`SnippingTool.exe`, ~120ms, `evidence/overlay-probe.py`); evento recomendado como mejor técnica; estrategia aprobada por Hero → Tarea 12.1.

---

## Tarea 12.1 · Captura con evento: enganche rápido del clic sostenido

**Estado:** [x] ✅ completada · 2026-09-09
**Dueño:** agente + Hero
**Riesgo:** ◆ medio — tap dance vivo + `raw_hid` + timing con Windows

**Descripción:** el hold de `TDQ_MOUSE_HOLD` manda `Win+Shift+S` y arma (`capture_armed`); un watcher en PC detecta el overlay de Recortes y avisa con `S` por `raw_hid`; el firmware engancha `MS_BTN1`, con red de 800ms. Detalle en el contrato (`attachments/captura/spec-captura-evento.md` §§6-7).

**Implementación:** poll-loop de 800ms con bombeo `raw_hid_task()` (sin esto la `S` nunca entraba); espera 150ms en el handler de `S` (el overlay ignoraba el clic inmediato); logs `CAP_ARM`/`CAP_EVT`/`CAP_NET` + `CAP_TGL_*`/`CAP_S_SKIP` + marcador `V`/`VER_CAPTURE_24262`; watcher con huella `SnipOverlayRootWindow` + hilo lector + log; `mouse-watch.py` para correlacionar enganche→selección.

**Validación:** 25 holds en varias apps, 100% por `CAP_EVT`, 0 `CAP_NET`. Tiempos: ARM→overlay ~220ms (Windows), S→EVT ~165ms, total ~365-415ms (sentido ~850ms con tap dance). Estable según Hero.

**Commits:** `83f7e7c464` (implementación+validación), `efaffa101f` (contrato as-built). Hex canónico `.build/crkbd_rev1_cornekeymap.hex` (24.390/28.672).

---

## Tarea 13 · Tap dance `_FAST`: tap togglea, doble tap y hold clic derecho

**Estado:** [-] ❌ cancelada · 2026-09-09
**Dueño:** agente + Hero
**Riesgo:** ◆ medio — tocaba tecla viva de `_BASE`

**Descripción:** reemplazar ambos `TG(_FAST)` de `_BASE` por tap dance `TDQ_FAST` (tap togglea `_FAST`; doble tap y hold clic derecho `MS_BTN2`).

**Motivo de cancelación:** Hero revirtió a `TG(_FAST)` a propósito (trabaja mejor así); el objetivo contradice su preferencia actual. Implementado y compilado (.hex OK 24.082/28.672, commit `6165496ae7`) pero nunca flasheado ni contrastado.

**Aprendizaje:** si se retoma, el código está en `6165496ae7` (enum + action + `tdq_fast_finished`); validar con Hero si mantiene la preferencia por `TG` antes de reimplementar.

---

## Tarea 15 · Migrar la captura con evento al script principal

**Estado:** [x] ✅ completada · 2026-09-09
**Dueño:** agente + Hero
**Riesgo:** ◆ medio — tocó el script de uso diario (aditivo y reversible)

**Descripción:** traer la captura con evento (Task 12.1) a `layer_status_script.py` (`D:\scripts\status script`, repo `main` limpio): módulo `utils/captura/` (hilo dirigido por `CAP_ARM`, vigilancia ≤700ms, envío `S` con `Lock`) + rama `CAP_*` en el lector. Contrato en `attachments/migra-captura/spec-migracion-captura.md`.

**Resultado:** validado por Hero en uso diario ("funciona perfecto"): holds por `CAP_EVT`, capas/alt-tab/wrap sin cambios. Watcher suelto retirado (se conserva el archivo, no se ejecuta). Sin commit en repo ajeno (decisión de Hero).
