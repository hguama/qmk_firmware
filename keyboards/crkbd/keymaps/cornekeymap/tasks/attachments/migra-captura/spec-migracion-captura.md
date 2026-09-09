# Especificación: migrar la captura con evento al script principal

## 0. Manifiesto

| id | archivo | rol | estado |
|---|---|---|---|
| C1 | `spec-migracion-captura.md` | contrato | especificación · 2026-09-09 · pendiente aprobación Hero |

## 1. Objetivo

La captura con evento (Task 12.1, validada: 100% `CAP_EVT`, ~365ms) hoy depende de `capture-watcher.py` suelto, que **compite** con `layer_status_script.py` por el `raw_hid` y por eso no sirve en uso diario. Migrar el aviso del overlay y la lectura de `CAP_*` como un módulo **dentro** del script principal. Related to Task 12.1 (contrato captura §§6-7).

## 2. Punto de partida (no se toca el firmware)

- Firmware congelado (24.390, commit `83f7e7c464`): hold → `Win+Shift+S` + `CAP_ARM` + arma; `S` → espera 150ms → engancha + `CAP_EVT`; sin `S`, red 800ms + `CAP_NET`; `V`→`VER_CAPTURE_24262`. La migración es **solo lado PC**.
- Referencia viva: `attachments/captura/evidence/capture-watcher.py` (detección + envío `S`) y `mouse-watch.py` (correlación). Tras validar, el watcher suelto se retira (se conserva el archivo, no se ejecuta).

## 3. Diseño (aditivo, sin modificar lo existente)

En `D:\scripts\status script\layer_status_script.py` (≈450 líneas, repo git rama `main`, limpio verificado 2026-09-09; la copia en `PyCharmMiscProject` es vieja, no usar):

1. **Módulo nuevo `utils/captura/`** siguiendo el patrón existente (`utils/qmk_calibracion`, `utils/wrap_around`): ahí viven la detección del overlay y el manejador `CAP_*`, no sueltos en el script.
2. **Hilo `overlay_captura` dirigido por eventos (sin poll permanente):** vive bloqueado en un `Event` (cero CPU en reposo). La rama `CAP_*` del lector lo arma al ver `CAP_ARM` (el firmware lo manda justo después de `Win+Shift+S`, ~220ms antes de que aparezca el overlay); el hilo vigila la huella (`EnumWindows` + fullscreen + clase `SnipOverlayRootWindow` + título `Snipping Tool Overlay`) hasta 700ms y manda `S` **una vez** al detectarla, luego se desarma y vuelve a dormir. Sin `CAP_ARM` no se mira nada: un overlay manual nunca dispara `S` (antes ni siquiera se enviaba por la guarda `capture_armed` del firmware; ahora ni se busca).
3. **Rama `CAP_*` en `escuchar_hid`**: si el mensaje empieza con `CAP_`, despacharlo al manejador de captura (log + futuro indicador) **antes** del matching de capas; no toca el matching actual.
4. **Envío `S`** con la convención del script (ver §4). Nada de Tk desde el hilo: UI por `root.after`, como ya hace el lector.

## 4. Protocolo y lenguaje (mapeo exacto, verificado 2026-09-09)

| Dirección | Watcher (origen) | Script principal (destino) | Adaptación |
|---|---|---|---|
| PC→teclado `S` | `bytes([0x00]+[S]+[0]*31)` (33B) | `buf=[0]*33; buf[0]=0x00; buf[1]=ord("S"); dev.write(buf)` | ninguna en el cable: mismos bytes; usar la convención `buf` de `utils/qmk_calibracion` (report ID + comando en `buf[1]`) |
| Teclado→PC `CAP_*` | `data[:32]` + split + `startswith("CAP_")` | `msg` ya viene armado (printables); agregar `if msg.startswith("CAP_")` antes del matching | no reusar el matching de capas por subcadena para `CAP_*` |
| Detección overlay | `EnumWindows` cada 20ms + flag `was` | portar tal cual a hilo propio | ninguna |

Comprobado sin colisiones: ningún nombre de capa (`ALFA/MODE/COMMIT/NUMB/MOUSE_1/MOVE/LAYER_BASE`) es subcadena de `CAP_*`, y ningún `CAP_*` contiene `AT_ON`/`AT_OFF` (no dispara el reset de alt-tab).

## 5. Riesgos y situaciones

1. **Lector único.** Dos procesos leyendo `raw_hid` se pelean (observado: el script principal deja sin nada al watcher). Mitigación: el módulo **reemplaza** al watcher suelto; no ejecutar ambos (el mutex del script no cubre al watcher por ser otro proceso).
2. **`R` de alt-tab vs hold de captura.** `detectar_clic_reset_alt` manda `R` al clic izquierdo físico con alt-tab visible, y el firmware hace `clear_all()` (suelta hasta `MS_BTN1`): rompería un hold activo. Hoy disjuntos (captura no emite `AT_*`), pero validar el borde "captura iniciada en pleno alt-tab".
3. **Contención de escritura.** El hilo nuevo escribiría `S` sobre el mismo `dev` que ya escribe `R` desde otro hilo (precedente existente). Mitigación: `Lock` alrededor de `dev.write`.
4. **Hilo lector.** Toda la indicación de capas depende de `escuchar_hid`; un error en la rama nueva lo tumbaría. Mitigación: cambios solo aditivos + `try/except` (el lector ya lo tiene) + **copia de seguridad del script antes de tocarlo**.
5. **Rendimiento.** Sin poll permanente (el hilo duerme en `Event`): costo cero en reposo; solo trabaja ~700ms por hold.
7. **`CAP_ARM` perdido.** Si el lector no viera el `CAP_ARM`, no habría vigilancia ni `S`. Mitigación: el lector es fiable (validado 25/25) y, si pasara, la red de 800ms engancha igual en firmware.
6. **Red de seguridad intacta.** Sin módulo (o con bug), el firmware engancha igual por red 800ms: el peor caso es la lentitud actual, nunca peor.

## 6. Validación

Repo en `main` limpio (verificado: sin cambios pendientes) → commitear el punto de partida → implementar → reiniciar script → hold de prueba: el log propio debe mostrar `CAP_ARM`→`CAP_EVT` (~365ms) y la selección debe dibujar moviendo pronto; capas, alt-tab y wrap sin cambios; repetir en 2 apps. Solo entonces se deja de usar el watcher suelto.

## 7. Alcance y límites

Sin cambios de firmware. Sin commits del script ajeno desde aquí (su repo, su decisión). Sin prototipo (sin artefacto visible en este repo). Evidencia futura (logs) en `evidence/` de este Block.
