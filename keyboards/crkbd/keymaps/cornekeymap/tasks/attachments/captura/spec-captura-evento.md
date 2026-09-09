# Especificación: Captura en un paso

## 0. Manifiesto

| id | archivo | rol | estado |
|---|---|---|---|
| C1 | `spec-captura-evento.md` | contrato as-built | vigente · 2026-09-09 · §§1-5 históricos, §6 lo implementado, §7 validación+commit |

## 1. Objetivo

El hold de `TDQ_MOUSE_HOLD` (ImprPant + espera fija + clic sostenido) es inestable por tiempos. Validar si Windows avisa cuando el overlay de Recortes está listo para enganchar el clic ahí.

## 2. Hipótesis a validar (verbatim del formato anterior)

- El overlay de Recortes expone ventana detectable (clase/título) vía Win32 desde Python, y el canal `raw_hid` existente (ya usado para `R` y `M`) avisa al teclado a tiempo.
- Alternativas si no: `Win+Shift+S` en vez de `ImprPant` (overlay directo, más determinista) o dos taps explícitos sin tiempos.

## 3. Estrategia validada 2026-09-06, sin implementar (verbatim)

- Diagnóstico: el hold actual (ImprPant + 300 ms fijos + enganche) falla si el overlay tarda distinto; `wait_ms` congela el teclado; ImprPant depende del ajuste de Windows.
- Camino: watcher Python detecta el overlay por su huella (clase/título, por medir en tu PC) y manda letra `S` por el `raw_hid` existente (ya usado para `R` y `M`); el firmware engancha `MS_BTN1` al recibirla. El hold manda `Win+Shift+S` (overlay directo, determinista) y arma sin enganchar.
- Red obligatoria: si la `S` no llega en ~800 ms, enganchar igual — el peor caso queda idéntico a hoy, nunca peor.
- Riesgos: la firma puede cambiar con updates de Windows (mitiga con 2-3 alias + red); helper apagado (cubre la red); overlay cancelado con Esc (se libera con la tecla o `MS_BTN1`, como hoy).
- Veredicto: estable por diseño (fallback determinista). Técnica `EnumWindows` ya probada en tu máquina (313 ventanas listadas).
- Pendiente Fase A: script de huella — Hero presiona `Win+Shift+S` una vez y el script anota clase/título, sin abrir nada solo.

## 4. Hallazgo 2026-09-08: espera fija de 500ms

- Hero subió la espera fija a 500ms: estable en varias pruebas (a ~300ms se notaba inestabilidad).
- Consecuencia: el evento de Windows pasaría a aportar sobre todo velocidad, no estabilidad.
- Decisión: no se justifica implementarlo solo por disminuir la espera; sí se justificaría si reaparece inestabilidad. En monitoreo por Hero.

## 5. Validación 2026-09-09: la estrategia es viable y es la mejor técnica

- Huella medida en tu PC (`evidence/overlay-probe.py --live`, 2 muestras): tras `Win+Shift+S` el overlay es detectable a ~120ms (116 y 132ms, granularidad 20ms) como ventana fullscreen con clase `SnipOverlayRootWindow` + título `Snipping Tool Overlay`, proceso `SnippingTool.exe` (segunda ventana `XamlWindow` del mismo proceso). Excluir el falso positivo permanente `Windows Input Experience` (`CoreWindow` fullscreen).
- Canal: `raw_hid` probado en ambas direcciones (`R`/`M` PC→teclado en uso diario; estados teclado→PC). Sin bloqueante; falta el handler de `S` (Tarea 12.1). El salto PC→teclado se respalda en ese precedente (milisegundos frente a ~120ms del overlay).
- Comparación: (a) espera fija 500ms — estable hoy, cero código nuevo, pero congela el teclado 500ms y adivina el tiempo; (b) evento — enganche a ~150ms, sin congelar, con red de 800ms que lo deja nunca peor que hoy; cuesta watcher siempre activo + handler `S`; (c) dos taps — sin tiempos pero cambia el gesto.
- Recomendación: la estrategia del evento es la mejor técnica (más rápida y sin congelar, con red), pero con 500ms estable hoy su prioridad es baja: implementar (12.1) si vuelve la inestabilidad o si importan esos ~350ms y el teclado responsivo. Confirma §4.

## 6. Implementación final (as-built 2026-09-09)

Lo que corre en la placa y en el PC. Difiere del plan §3 en los puntos ★ (problemas encontrados al implementar):

- **Firmware, hold (`TD_SINGLE_HOLD`):** `tap_code(MS_BTN1)` (desatasque) → `tap_code16(G(S(KC_S)))` (overlay directo) → `send_layer_status("CAP_ARM")` → `capture_armed=true` → poll-loop hasta 800ms en tramos de 20ms. ★ Sin bombear `raw_hid_task()` en el loop la `S` nunca entraba (`raw_hid_receive` solo corre en el loop principal, que está bloqueado); prototipo propio, QMK no lo expone. Si sigue armado a los 800ms, la red engancha igual (`CAP_NET`). Single tap (toggle), doble tap y doble hold intactos.
- **Firmware, handler de `S` (`raw_hid_receive`, solo si `capture_armed`):** ★ espera 150ms antes de enganchar — sin esto el clic se presionaba a ~19ms de detectarse la ventana, cuando Snipping aún no aceptaba input, y se perdía ("muerto"). Luego enganche con flanco limpio (`unregister`+`register` `MS_BTN1`, `mouse_held=true`) → desarma → `CAP_EVT`. Una `S` sin armar (overlay manual) se ignora y se loguea `CAP_S_SKIP`.
- **Firmware, diagnóstico:** `V`→`VER_CAPTURE_24262` (marcador para probar qué hay flasheado sin adivinar); toggle manual reporta `CAP_TGL_1/0` con el estado resultante (detecta desfases firmware↔Windows).
- **Watcher (`evidence/capture-watcher.py`):** huella §5 por `EnumWindows` (fullscreen + clase + título), manda `S` por `raw_hid` (`0xFF60`) con edge-detection y reconexión. ★ El lector cortaba `data[1:33]` y perdía la 1ª letra (`CAP_ARM`→`AP_ARM`): jamás mostró un aviso; corregido a `data[:32]` y ahora registra todo el tráfico (`KB -> ...`) con reloj en consola y `capture-log.txt`. Flags `--once`/`--log`.
- **Mouse-watch (`evidence/mouse-watch.py`):** inicio/arrastre/fin del cursor con el mismo reloj, para correlacionar enganche→selección.
- **Uso validado:** mover pronto al aparecer el overlay. La cruz es consecuencia del arrastre, no señal previa — esperarla suma ~1.5s innecesarios y el cursor se desplaza del punto de inicio.
- **Flasheo:** canónico `.build/crkbd_rev1_cornekeymap.hex` (la copia de la raíz se elimina; `Docs/guia_compilacion.md` §6).

## 7. Validación y commit

- Medido (watcher+mouse, 7+5+13 holds en varias apps, §6 final): **100% por `CAP_EVT`, 0 `CAP_NET`**. Tiempos del log (sin los ~450ms del tap dance): ARM→overlay ~220ms (Windows), S→EVT ~165ms (150 espera+15 proceso), total ARM→enganche ~365-415ms. Sentido: ~850ms. EVT→primer movimiento ~1.5s (reacción del usuario, no sistema).
- **Commit `83f7e7c464`** (2026-09-09, rama `my-corne`, local): firmware + watcher + mouse-watch + probe + logs de validación + este contrato + plan. Ver con `git show 83f7e7c464`. Para reproducir: flashear el `.hex` canónico (ambas mitades), mandar `V` (responde `VER_CAPTURE_24262`), correr watcher+mouse-watch, hacer hold.
