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

**Estado:** ✅ Completada

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
| `_MOVE_H` | 10 | ❌ Eliminada | Vacía + keycode huérfano |
| `_FAST` | 11 | ✅ Se queda | Scroll rápido |
| `_MOVE_WIN` | 12 | ✅ Se queda | Ventanas |
| `_MOVE_L` | 13 | ❌ Eliminada | Mover líneas de código |
| `_RUN` | 14 | ✅ Se queda | Ejecutar (Flutter/Java) |
| `_MODE` | 15 | ❌ Eliminada | Juego/navegación |
| `_COMMIT` | 16 | ❌ Eliminada | Git obsoleto |
| `_MOUSE_KEY` | 17 | ✅ Se queda | Mouse con teclado |
| `_NEW` | 18 | ❌ Eliminada | Not used |

**Acción:** ✅ Completada. Se eliminaron `_MOVE_H`, `_MOVE_L`, `_MODE` y `_COMMIT` (además de `_DEV` y `_NEW`).

### 1.1 Reordenar índices de capas (contiguos)

**Estado:** ✅ Completada

**Objetivo:** Compactar los índices del `enum layer_names` para que queden
0..12 sin huecos. Hoy hay huecos en los índices 4, 10, 13, 15 y 16.

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

**Análisis de riesgo (bajo):**

- ✅ El código vivo usa **nombres** de enum (`LT(_AI, ...)`, `MO(_DEL)`,
  `TG(_MOVE)`, etc.), no números fijos; se actualizan solos al reordenar.
- ✅ El orden relativo se conserva (solo se compacta), así que la prioridad
  entre capas no cambia.
- ✅ `_BASE` sigue en el índice 0, por lo que la capa por defecto guardada
  en EEPROM no se ve afectada.
- ⚠️ Quedan **índices fijos en código muerto** que conviene limpiar/convertir:
  - `case TG(2):` y `case TG(6):` en `process_record_user` no están en
    ningún layout (código muerto). `TG(2)` apunta a `_ALFA` (se mantiene),
    pero `TG(6)` pasaría de `_SYMB` a `_NUMB` si se usara. Eliminar o
    convertir a `TG(_SYMB)`/`TG(_ALFA)`.
  - Bloque RGBLIGHT comentado (`my_rgb_layers[]` y
    `rgblight_set_layer_state(N, ...)`) usa índices fijos; no afecta en
    runtime pero conviene actualizarlo por si se reactiva.
- ✅ Beneficio: todas las capas quedarían ≤ 12, así que `LT()` funcionaría
  con cualquier capa. Hoy `_MOUSE_KEY = 17` no puede usarse con `LT()`
  (límite de 15).

**Acción:** ✅ Hecho. Se reordenó el enum a 0..12, se eliminaron los
handlers muertos `TG(2)`/`TG(6)` y se actualizó el bloque RGBLIGHT
comentado.

### 1.2 Compilar y probar capas reordenadas (para el usuario)

**Estado:** ✅ Completada

**Resultado:** Pruebas exitosas (`_DEL` y `_MOUSE_KEY` funcionan
correctamente).

**Descripción:** Compilar el firmware y verificar que las capas cuyos
índices se movieron siguen funcionando.

**Pruebas:**
1. `qmk compile -kb crkbd/rev1 -km cornekeymap` (ajusta la revisión a tu
   placa si no es `rev1`).
2. Flashear y probar:
   - `_DEL` (borrar caracteres): antes índice 5 → ahora 4.
   - `_MOUSE_KEY` (mouse con teclado): antes índice 17 → ahora 12.
3. Verificar que las luces RGB indicadoras muestren el color correcto en
   cada capa.

**Resultado esperado:** ambas capas se activan y responden normalmente.

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
