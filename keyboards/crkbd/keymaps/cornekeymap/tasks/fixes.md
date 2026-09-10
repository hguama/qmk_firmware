# 🔧 Fixes — Errores solucionados

> [!index] Índice de fixes
> | Fix | Síntoma | Ver |
> |---|---|---|
> | 1 | `LT()` no activa la capa al mantener, pero el tap sí funciona | [Fix 1 · LT() no activa la capa correcta](#^fix-1) |

---

> [!fix] **Fix 1 · LT() no activa la capa correcta** ^fix-1
> `LT()` solo codifica la capa en 4 bits (`layer & 0xF`): una capa > 15 se trunca y activa otra capa.
>
> > [!desc]- Ver detalle técnico
> > **Síntoma:** `LT(_CAPA, KC_X)` no activa la capa al mantener la tecla (hold), pero el tap sí funciona. `MO(_CAPA)` y `TG(_CAPA)` sí funcionan con esa misma capa.
> >
> > **Causa:** `LT()` usa solo **4 bits** para codificar la capa (`layer & 0xF`), por lo que solo soporta capas **0 a 15**. Si la capa tiene un valor > 15 en el enum, se trunca:
> >
> > ```
> > LT(_AI, KC_ENT) con _AI = 17  →  17 & 0xF = 1  →  activa _MOVE (capa 1)
> > ```
> >
> > `MO()`, `TG()`, `TO()`, `TT()` y `OSL()` usan 5 bits (`layer & 0x1F`) y soportan capas 0-31 sin problema. `LM()` también está limitado a 0-15.
> >
> > **Solución A — Reordenar capas (preferida):**
> >
> > Mover la capa problemática a una posición ≤ 15 en el enum, usando asignación explícita para no romper el resto:
> >
> > ```c
> > enum layer_names {
> >     _BASE    = 0,
> >     _MOVE    = 1,
> >     _ALFA    = 2,
> >     _AI      = 3,  // ← movida de 17 a 3
> >     _DEV     = 4,
> >     // ... resto con valores explícitos ...
> >     _MOUSE_KEY = 17,  // ← movida de 3 a 17 (no usa LT, seguro)
> > };
> > ```
> >
> > **Importante:** Al cambiar valores del enum, también hay que reordenar las definiciones en el array `keymaps[]` para que coincidan.
> >
> > **Solución B — LT manual en process_record_user:**
> >
> > Crear un keycode personalizado que emule `LT()` para capas > 15:
> >
> > ```c
> > // En custom_keycodes:
> > AI_ENT,
> >
> > // En process_record_user:
> > case AI_ENT:
> >     if (record->event.pressed) {
> >         if (record->tap.count > 0) {
> >             tap_code(KC_ENT);       // TAP
> >         } else {
> >             layer_on(_AI);          // HOLD
> >         }
> >         return false;
> >     } else {
> >         layer_off(_AI);             // RELEASE
> >         return false;
> >     }
> > ```
> >
> > **Referencia rápida de límites:**
> >
> > | Keycode | Bits capa | Capas máx |
> > |---------|-----------|-----------|
> > | `MO()`  | 5         | 0-31      |
> > | `TG()`  | 5         | 0-31      |
> > | `TO()`  | 5         | 0-31      |
> > | `TT()`  | 5         | 0-31      |
> > | `OSL()` | 5         | 0-31      |
> > | `LT()`  | **4**     | **0-15**  |
> > | `LM()`  | **4**     | **0-15**  |
> >
> > **Relacionado:** [Tarea 1 · Revisar capas obsoletas](completed.md#^capas-jetbrains)
