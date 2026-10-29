# Especificación: Ctrl+Espacio → Alt+K cuando Konsole está activa

## 0. Manifiesto

| id  | archivo                    | rol      | estado                                                           |
| --- | -------------------------- | -------- | ---------------------------------------------------------------- |
| C1  | `spec-dictado-terminal.md` | contrato | cerrado · 2026-10-29 · opción (a) en modo tap · probado por Hero |

## Decisión (2026-10-29)

Hero eligió la opción (a) con un ajuste: no sostiene la tecla, la usa en **modo tap**
(toque = empieza, dicta, toque = termina). Por eso en Konsole la tecla manda un solo
toque de `Alt+K` en vez de registrarlo mientras se sostiene, y Claude Code quedó con
`"voice": {"mode": "tap"}` en `~/.claude/settings.json`. Solo Konsole por ahora; el
`A(KC_K)` suelto de `_BASE` se queda hasta que esto se pruebe.

Implementado:

- `keymap.c`: `terminal_activa` + `dictado_terminal_presionado`, rama nueva en
  `case C(KC_SPACE)` y comando `T` en `raw_hid_receive`.
- `status_script` (`linux-port`): módulo `utils/dictado_terminal`, que revisa
  `cursor_kwin.ventana()` cada 50 ms y manda `T1`/`T0` al cambiar; se reenvía al
  reconectar el Corne.

Lo que sigue en la sección 4 es el diseño original con sostenido, ya reemplazado
por el toque.

## 1. Objetivo

Usar la misma tecla física de `C(KC_SPACE)` para dos dictados:

- **Konsole activa** → mandar `Alt+K` (dictado nativo de Claude Code, `~/.claude/keybindings.json` → `"meta+k": "voice:pushToTalk"`).
- **Cualquier otra ventana** → `Ctrl+Espacio` como hoy (Handy).

## 2. Lo actual

- `C(KC_SPACE)` está en `_BASE` (keymap.c:1201, ambos lados), `_MOVE` (1216) y otra capa (1338).
- `process_record_user` (keymap.c:405-419) **observa** `C(KC_SPACE)` sin interceptarlo (`return true`) para llevar `handy_recording_active`, que hace que Escape mande Escape+F14 (cancelar Handy).
- `raw_hid_receive` (keymap.c:1405) ya recibe comandos de `status_script`: `R`, `V`, `S`, `M`.
- `status_script` (rama `linux-port`) ya sabe la ventana activa: `utils/cursor_kwin.ventana()` → `(clase, x, y, ancho, alto)`, refrescada ~60 veces por segundo vía KWin. `marca_scroll` y `cursor_texto` ya filtran por `APPS = ("konsole",)`.
- Handy tiene `Ctrl+Espacio` como atajo global: Konsole nunca lo recibe, así que no se puede resolver solo con un atajo de Konsole.
- En la diff sin confirmar de `_BASE` ya hay un `A(KC_K)` suelto (keymap.c:1201, primera columna).

## 3. Opciones

| Camino                                     | Qué es                                                                                                                                                                                   | Pro                                                                                                                                                                                                               | Contra                                                                                                                                     |
| ------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| (a) Firmware + status_script (recomendado) | El script avisa al Corne por raw HID cuando Konsole gana o pierde el foco (`T1` / `T0`). Si la bandera está encendida, `C(KC_SPACE)` registra `Alt+K` al presionar y lo suelta al soltar | ✅ misma tecla física; ✅ el sostenido funciona (push-to-talk); ✅ reutiliza el canal HID y `cursor_kwin` que ya existen; ✅ si el script no corre, la bandera queda apagada → Ctrl+Espacio normal (falla segura) | Toca dos repos; hay que flashear el Corne                                                                                                  |
| (b) Solo software (evdev/uinput)           | El script agarra el teclado, intercepta Ctrl+Espacio en Konsole y reinyecta Alt+K                                                                                                        | No toca el firmware                                                                                                                                                                                               | ❌ exige agarrar el dispositivo (grab) y reinyectar todo; ❌ choca con input-remapper, que ya está corriendo; ❌ frágil y con más latencia |
| (c) input-remapper                         | Regla de remapeo                                                                                                                                                                         | Ya está instalado                                                                                                                                                                                                 | ❌ en Wayland no distingue por aplicación                                                                                                  |
| (d) Tecla aparte                           | Usar el `A(KC_K)` que ya está en `_BASE`                                                                                                                                                 | ✅ cero código                                                                                                                                                                                                    | ❌ no es la tecla cómoda de siempre                                                                                                        |

## 4. Recomendado: (a) — diseño

### Firmware (`keymap.c`)

```c
// Lo enciende status_script por raw HID ('T','1' / 'T','0') cuando Konsole
// gana o pierde el foco. Apagada por defecto: sin script, Ctrl+Espacio normal.
static bool terminal_activa = false;
static bool dictado_terminal_sostenido = false; // Alt+K registrado por esta tecla

case C(KC_SPACE):
    // En Konsole, la tecla de Handy se vuelve el dictado de Claude Code (Alt+K).
    // Se decide al presionar y se recuerda, para soltar lo mismo que se registró
    // aunque el foco cambie en medio.
    if (record->event.pressed && terminal_activa) {
        dictado_terminal_sostenido = true;
        register_code(KC_LALT);
        register_code(KC_K);
        return false;
    }
    if (!record->event.pressed && dictado_terminal_sostenido) {
        dictado_terminal_sostenido = false;
        unregister_code(KC_K);
        unregister_code(KC_LALT);
        return false;
    }
    // ... lógica actual de handy_recording_active, sin cambios
```

```c
// raw_hid_receive
else if (data[0] == 'T' || data[1] == 'T') {
    terminal_activa = (data[1] == '1' || data[2] == '1');
}
```

- En Konsole **no** se toca `handy_recording_active`: Escape sigue mandando Escape solo, que también cancela el dictado de Claude Code.
- Ojo con el desplazamiento de `data[0]`/`data[1]`: copiar el patrón de `R`/`S` (el byte 0 a veces es el report id).

### status_script (`linux-port`)

- Un hilo nuevo (o un callback de `cursor_kwin.on_muestra`) compara la clase de `cursor_kwin.ventana()` con `APPS = ("konsole",)`.
- Solo cuando cambia el estado, escribe `[0, 'T', '1' o '0', ...]` (33 bytes) con `captura.WRITE_LOCK`, como el comando `R`.
- Al reconectar el Corne (`_reconectar_hid`), reenviar el estado actual: la bandera del firmware arranca apagada.

### Pruebas

1. Konsole activa, sostener la tecla → aparece `listening…` en Claude Code; al soltar se finaliza el texto.
2. Otra ventana → Handy graba como siempre.
3. Cambiar de ventana mientras se sostiene → no se queda Alt pegado.
4. Detener el servicio `status-script` → la tecla vuelve a ser Ctrl+Espacio en todos lados.

## 5. Preguntas para Hero

- ¿Solo Konsole, o también otras terminales (y Claude Code en el IDE)?
- ¿Se quita el `A(KC_K)` suelto de `_BASE` cuando esto funcione?
