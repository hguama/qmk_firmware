# Implementations — Funcionalidades implementadas

---

## Plantilla: Tap Dance (Quad System)

Para agregar un nuevo tap dance al keymap, seguir estos pasos:

### 1. Agregar entrada al enum `TDQ_*` (línea ~155)

```c
enum {
    TDQ_SEL,
    // ... existentes ...
    TDQ_NUEVO,   // ← nueva entrada
};
```

### 2. Forward declaration (línea ~222)

```c
void tdq_nuevo_finished(tap_dance_state_t *state, void *user_data);
```

### 3. Crear función `tdq_nuevo_finished` (línea ~2364+)

```c
void tdq_nuevo_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:
            // acción para 1 tap
            break;
        case TD_SINGLE_HOLD:
            // acción para hold
            break;
        case TD_DOUBLE_TAP:
            // acción para 2 taps
            break;
        case TD_DOUBLE_HOLD:
            // acción para tap+hold
            break;
        case TD_TRIPLE_TAP:
            // acción para 3 taps
            break;
        case TD_TRIPLE_HOLD:
            // acción para 2 taps + hold
            break;
        default: break;
    }
}
```

### 4. Registrar en `tap_dance_actions[]` (línea ~1982)

```c
tap_dance_action_t tap_dance_actions[] = {
    // ... existentes ...
    [TDQ_NUEVO] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_nuevo_finished, x_reset),
};
```

### 5. Tapping term (opcional, línea ~2542)

Si se necesita un tapping term distinto al default (450ms):

```c
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    if (keycode == TD(TDQ_NUEVO))
        { return 300; }   // ms, menor = más rápido, mayor = más margen para doble tap

    if (QK_TAP_DANCE <= keycode && keycode <= QK_TAP_DANCE_MAX)
        { return 450; }   // default

    return TAPPING_TERM;
}
```

### 6. Usar en el keymap

```c
TD(TDQ_NUEVO)   // en lugar de una tecla normal en el layout
```

### Estados disponibles (`td_state_t`)

| Estado | Significado |
|--------|-------------|
| `TD_SINGLE_TAP` | 1 tap |
| `TD_SINGLE_HOLD` | 1 press mantenido |
| `TD_DOUBLE_TAP` | 2 taps rápidos |
| `TD_DOUBLE_HOLD` | 1 tap + hold |
| `TD_DOUBLE_SINGLE_TAP` | 2 taps (variante interrumpida) |
| `TD_TRIPLE_TAP` | 3 taps |
| `TD_TRIPLE_HOLD` | 2 taps + hold |

---

## TDQ_ESC — Tap Dance para la tecla Escape (2026-08-11)

**Archivos modificados:**
- `keymap.c` — enum, forward declaration, función, tap_dance_actions, get_tapping_term, layout _BASE

**Estados:**

| Estado | Acción | Keycode |
|--------|--------|---------|
| `TD_SINGLE_TAP` | Escape | `KC_ESC` |
| `TD_SINGLE_HOLD` | Minimizar todo (Win+D) | `G(KC_D)` |
| `TD_DOUBLE_TAP` | Clic derecho | `MS_BTN2` |

**Uso:** Reemplaza `KC_ESC` en la capa `_BASE`.

> ⚠️ **Nota — no usar `on_each_tap` para acelerar el tap simple**
>
> Si en el futuro se quiere hacer el primer tap más rápido usando `on_each_tap`,
> tener en cuenta que **la primera tecla se envía al presionar**, antes de saber
> si será un doble tap. Por eso, al hacer doble tap, la acción del primer tap
> (`KC_ESC`) se envía igual antes de la acción del doble tap (`Ctrl+F`).
>
> Esto tiene un efecto colateral molesto: en JetBrains, ese `ESC` anticipado
> **quita la selección de la palabra**, por lo que `Ctrl+F` no busca la palabra
> seleccionada. Se revirtió a `on_dance_finished` (ESC con delay) por este motivo.

---

## TDQ_PASTE — Tap Dance para pegar (2026-08-12)

Reemplaza el antiguo keycode personalizado `PASTE` (que solo hacía `Ctrl+V`).

**Archivos modificados:**
- `keymap.c` — enum, forward declaration, función, tap_dance_actions, layouts (_BASE, _MOVE, _ALFA)

**Estados:**

| Estado | Acción | Keycode |
|--------|--------|---------|
| `TD_SINGLE_TAP` | Pegar | `Ctrl+V` (`C(KC_V)`) |
| `TD_SINGLE_HOLD` | Recargar página | `KC_F5` |
| `TD_DOUBLE_TAP` | Pantalla completa | `KC_F11` |
| `TD_DOUBLE_HOLD` | Portapapeles | `Win+V` (`G(KC_V)`) |

**Uso:** Reemplaza `PASTE` en las capas `_BASE`, `_MOVE` y `_ALFA`.

**Nota:** Se eliminó el keycode `PASTE` del enum `custom_keycodes` y sus
handlers en `process_record_user`. También se quitaron dos bloques muertos
que lo usaban como tap keycode: `LT(MS_ACL2, PASTE)` y `LT(_FAST, PASTE)`.

---

## TDQ_MOUSE_HOLD — Tap Dance para sostener clic (2026-08-12)

Replica el comportamiento del keycode `MOUSE_HOLD` en el tap, y agrega
acciones extra. **Aún no reemplaza a `MOUSE_HOLD`** en los layouts.

**Estados:**

| Estado | Acción | Keycode |
|--------|--------|---------|
| `TD_SINGLE_TAP` | Alternar clic izquierdo sostenido | `toggle_mouse_hold()` |
| `TD_SINGLE_HOLD` | Imprimir pantalla | `KC_PSCR` |
| `TD_DOUBLE_TAP` | Buscar palabra | `Ctrl+F` (`C(KC_F)`) |
| `TD_DOUBLE_HOLD` | Captura de ventana activa | `Alt+Print Screen` (`A(KC_PSCR)`) |

**Nota sobre el uso:** El `tap` hace lo mismo que `MOUSE_HOLD`: registra
`MS_BTN1` sostenido. El usuario normalmente lo libera presionando `MS_BTN1`
(no la misma tecla), por lo que la lógica de `MS_BTN1` en `process_record_user`
se mantiene intacta. Se extrajo la lógica a `toggle_mouse_hold()` para que
`MOUSE_HOLD` y `TDQ_MOUSE_HOLD` (tap) queden sincronizados.
