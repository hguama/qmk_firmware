# Especificación: scroll sostenido en `_FAST`

## 0. Manifiesto

| id  | archivo               | rol      | estado                                                                               |
| --- | --------------------- | -------- | ------------------------------------------------------------------------------------ |
| C1  | `spec-fast-scroll.md` | contrato | cerrado · 2026-11-10 · ráfagas repetidas con pausa de 400 ms (§6); validado por Hero |

## 1. Objetivo

Lograr desplazamiento vertical sostenido al **mantener** la tecla en `_FAST`, para moverse rápido entre páginas sin pulsar repetidas veces.

## 2. Lo actual

- `UP_10`/`DOWN_10` (keymap.c:85-86, 434-450): al **presionar** mandan 8 ticks de rueda (`MS_WHLU`/`MS_WHLD`); al mantener **no repiten** (el `if (pressed)` solo dispara una vez). Salto fijo, sin sostenido.
- Detalle: se llaman "_10" pero mandan 8 ticks (inconsistencia nombre/código).
- En la misma capa (keymap.c:1129) ya existen `MS_WHLU`/`MS_WHLD` directos.

## 3. Opciones (corregido 2026-09-10: la rueda directa sola pierde el salto fijo)

| Camino                              | Qué es                                                                                                                                  | Pro                                                                        | Contra                                                                |
| ----------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------- | --------------------------------------------------------------------- |
| (a) Ráfaga + enganche (recomendado) | `UP_10`: al presionar manda los 8 ticks de hoy **y** engancha la rueda (`register`); al soltar la suelta. QMK repite solo lo enganchado | ✅ una tecla hace las dos cosas: tap = salto fijo, hold = salto + continuo | Toca `process_record` (cambio chico y acotado)                        |
| (b) Rueda directa sin código        | Usar los `MS_WHLU/WHLD` ya en `_FAST`                                                                                                   | ✅ cero código                                                             | ❌ el tap da 1 tick: se pierde el "presiono una vez = varios scrolls" |
| (c) Espacios                        | Seguir con `Espacio`/`PgDn` por pulsaciones                                                                                             | Nada que cambiar                                                           | ❌ no hay repetición nativa útil; brusco por páginas                  |

## 4. Recomendado: (a) — código propuesto

```c
// Ráfaga configurable (hoy "10" manda 8: unificar aquí el número real)
#define SCROLL_BURST 8

case UP_10:
    if (record->event.pressed) {
        // 1) salto fijo inmediato: lo de hoy (una pulsación = varios scrolls)
        for (int i = 0; i < SCROLL_BURST; i++) {
            tap_code16(MS_WHLU);
        }
        // 2) engancha la rueda: el motor QMK la repite solo mientras se sostenga
        register_code(MS_WHLU);
    } else {
        // 3) al soltar se desengancha: el sostenido para en seco
        unregister_code(MS_WHLU);
    }
    return false;

case DOWN_10: // espejo con MS_WHLD
    if (record->event.pressed) {
        for (int i = 0; i < SCROLL_BURST; i++) {
            tap_code16(MS_WHLD);
        }
        register_code(MS_WHLD);
    } else {
        unregister_code(MS_WHLD);
    }
    return false;
```

Comportamiento resultante:

| Gesto                 | Hoy            | Con el cambio                                                           |
| --------------------- | -------------- | ----------------------------------------------------------------------- |
| Tap (pulsar y soltar) | 8 ticks        | 8 ticks (+1 del enganche, imperceptible; bajar a 7 si se quiere exacto) |
| Hold (sostener)       | 8 ticks y para | 8 ticks + scroll continuo hasta soltar                                  |

Velocidad del sostenido: la dan `mk_interval`/`mk_max_speed` (ya calibrables desde tu script con `M`). Sin timers a medida ni rebotes: el `return false` y el registro nativo hacen el trabajo.

## 5. Validación

En `_FAST` con el cambio: tap en `UP_10` → un salto fijo (~8 ticks); mantener `UP_10` 3s → salto + scroll continuo; soltar → para en seco; espejo con `DOWN_10`; sin interferencia con `MS_WHLL/WHLR` vecinos ni con el wrap del script.

## 6. Decisión de Hero (2026-11-10)

No se eligió ninguna de las tres opciones del §3: al sostener `UP_10`/`DOWN_10` no hay rueda continua, sino **ráfagas repetidas con una pausa entre cada una** ("se mueve una vez, una pequeña pausa y otra vez"). Es el mismo patrón de los saltos del ratón (`MS_SALTO_*`).

- Al presionar: una ráfaga de 8 ticks, igual que antes (un toque corto no cambia).
- Al sostener: espera `SCROLL_ESPERA` y luego repite una ráfaga cada `SCROLL_REPETIR`.
- Al soltar: para en seco. Lo repite `matrix_scan_user`; no se engancha ninguna tecla de rueda.

| Ajuste (keymap.c) | Valor inicial | Qué cambia                                                                                                  |
| ----------------- | ------------- | ----------------------------------------------------------------------------------------------------------- |
| `SCROLL_RAFAGA`   | 8 ticks       | tamaño de cada ráfaga                                                                                       |
| `SCROLL_ESPERA`   | 300 ms        | sostenido antes de la primera repetición                                                                    |
| `SCROLL_REPETIR`  | 400 ms        | pausa entre ráfagas; subirlo para ir más despacio (era 200 ms, se duplicó el 2026-11-10 para ir a la mitad) |

Pruebas: tap → una ráfaga; mantener 3 s → ráfagas con pausa visible; soltar → para; espejo con `DOWN_10`; `MS_WHLL/WHLR` vecinos y el wrap sin cambios.
