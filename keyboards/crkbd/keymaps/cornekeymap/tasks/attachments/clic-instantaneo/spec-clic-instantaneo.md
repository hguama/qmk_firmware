# Especificación: clic sostenido instantáneo en la tecla de `TDQ_MOUSE_HOLD`

## 0. Manifiesto

| id  | archivo                    | rol      | estado                                                                                      |
| --- | -------------------------- | -------- | ------------------------------------------------------------------------------------------- |
| C1  | `spec-clic-instantaneo.md` | contrato | cerrado · 2026-10-29 · opción (a), probado por Hero; el tap dance TDQ_MOUSE_HOLD se eliminó |

## 1. Objetivo

- Dejar solo dos gestos: **toque** = clic izquierdo sostenido (alternar) y **sostener** = captura de región.
- Quitar el doble toque (clic del medio) y el doble toque + sostener (Alt+Impr Pant).
- Que el clic sostenido sea **instantáneo**: se engancha apenas se presiona la tecla.

## 2. Lo actual

- `TD(TDQ_MOUSE_HOLD)` en `_BASE` (keymap.c:1217, ambos lados) y `_MOVE` (1232).
- `tdq_mouse_hold_finished` (keymap.c:1649): 4 gestos vía `cur_dance`.
- `get_tapping_term` (keymap.c:1753): **todos los tap dances usan 450 ms**.
- Por eso el toque tarda: el tap dance decide **al soltar más 450 ms**, esperando por si viene un segundo toque.
- `toggle_mouse_hold()` (keymap.c:352) alterna `mouse_held` y `MS_BTN1`.
- La guarda de Alt+Tab en `process_record_user` (keymap.c:388-391) deja pasar `TD(TDQ_MOUSE_HOLD)`: la tecla nueva tiene que ir en esa lista.

**Quitar los gestos dobles no basta:** el motor de tap dance no sabe que ya no hay doble toque y sigue esperando los 450 ms. Para que sea rápido hay que dejar de usar un tap dance.

## 3. Opciones

| Camino                                              | Qué es                                                                                                                                                                          | Demora del toque                   | Pro                                                | Contra                                                                                                                                                                                                                          |
| --------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------- | -------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| (a) Tecla propia, acción al presionar (recomendado) | Keycode nuevo `MOUSE_HOLD`. **Al presionar** llama a `toggle_mouse_hold()` sin esperar. Si se sigue sosteniendo más de ~300 ms (`matrix_scan_user`), lanza la captura de región | **0 ms**                           | ✅ lo que pidió Hero: presiona y ya está sostenido | Al sostener para capturar, el clic se engancha 300 ms antes de la captura. La captura ya empieza soltando y re-enganchando el clic, así que no cambia el resultado, pero puede marcar una selección corta donde esté el puntero |
| (b) Tecla propia, toque decidido al soltar          | Igual que (a), pero el toque se aplica **al soltar** si fue antes de ~200 ms; si no, captura                                                                                    | Lo que dure la pulsación (~100 ms) | ✅ sostener no genera ningún clic previo           | No es instantáneo al presionar                                                                                                                                                                                                  |
| (c) Mantener el tap dance con menos tiempo          | Quitar gestos dobles y bajar el término a ~150 ms                                                                                                                               | Al soltar + 150 ms                 | Cambio mínimo                                      | ❌ sigue habiendo espera                                                                                                                                                                                                        |

## 4. Recomendado: (a) — diseño

```c
// en el enum de keycodes
MOUSE_HOLD,   // toque = alternar clic izquierdo sostenido (al presionar) | sostener = captura de región

#define MOUSE_HOLD_CAPTURA_MS 300
static uint16_t mouse_hold_timer = 0;
static bool     mouse_hold_presionada = false;

// process_record_user
case MOUSE_HOLD:
    if (record->event.pressed) {
        toggle_mouse_hold();             // instantáneo
        mouse_hold_presionada = true;
        mouse_hold_timer = timer_read();
    } else {
        mouse_hold_presionada = false;
    }
    return false;

// matrix_scan_user
if (mouse_hold_presionada && timer_elapsed(mouse_hold_timer) > MOUSE_HOLD_CAPTURA_MS) {
    mouse_hold_presionada = false;       // una sola captura por sostenido
    captura_region();                    // el bloque TD_SINGLE_HOLD de hoy, movido a una función
}
```

- `TD(TDQ_MOUSE_HOLD)` → `MOUSE_HOLD` en las tres posiciones, y en la guarda de Alt+Tab.
- Se borran `TDQ_MOUSE_HOLD`, su entrada en `tap_dance_actions` y `tdq_mouse_hold_finished`. El cuerpo de la captura pasa a `captura_region()`.
- Actualizar `tasks/Docs/tap_dance_guide.md`: la tecla deja de ser tap dance, y la guía ya estaba desactualizada (hold = `Impr Pant`, doble = `Ctrl+F`).

### Pruebas

1. Toque en Konsole: el clic se engancha al instante (selección arrastrando); otro toque lo suelta.
2. `MS_BTN1` y `Esc` siguen soltando el clic enganchado.
3. Sostener más de 300 ms: se abre la captura de región y el clic queda enganchado para seleccionar.
4. Con el selector de Alt+Tab abierto, la tecla sigue eligiendo ventana.

## 5. Fuera de alcance

- Que la captura de región funcione bien en Linux (`Win+Shift+S` y el aviso `S` vienen de Windows; el hilo de captura está desactivado en status_script). Va aparte si Hero lo pide.
