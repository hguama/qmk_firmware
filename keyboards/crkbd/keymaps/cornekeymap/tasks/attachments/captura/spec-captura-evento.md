# Especificación: Captura en un paso

## 0. Manifiesto

| id | archivo | rol | estado |
|---|---|---|---|
| C1 | `spec-captura-evento.md` | contrato | vigente · 2026-09-08 · enmienda §4 |

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

## 6. Implementación 12.1 (2026-09-09, en prueba)

- Firmware: el hold manda `Win+Shift+S` (en vez de ImprPant) y arma (`capture_armed`); poll-loop de 800ms en tramos de 20ms — si llega `S`, el handler engancha con flanco limpio; si no, la red engancha igual. Single tap, doble tap y doble hold de `TDQ_MOUSE_HOLD` intactos.
- Watcher (`evidence/capture-watcher.py`): vigila la huella y manda `S` por `raw_hid` (usage `0xFF60`); sin overlay no hace nada. Una `S` con overlay manual se ignora (solo engancha si está armado).
- Conocido: durante la espera el teclado sigue congelado (típ. ~150ms en vez de 500ms fijos).
- Fix 2026-09-09: la espera no procesaba USB (`raw_hid_receive` solo corre en el loop principal), así que el aviso `S` nunca entraba y siempre disparaba la red — se bombea `raw_hid_task()` en el poll-loop (prototipo propio, QMK no lo expone).
- Logs: el firmware reporta `CAP_ARM` (arma), `CAP_EVT` (enganchó por evento) y `CAP_NET` (enganchó por red); el watcher los registra con reloj en consola y `capture-log.txt` para distinguir qué ruta disparó.
- Flasheo: el canónico es `.build/crkbd_rev1_cornekeymap.hex` (la copia de la raíz se elimina; ver `Docs/guia_compilacion.md` §6).
- Fix 2026-09-09 (watcher ciego): el lector cortaba `data[1:33]` y perdía la 1ª letra (`CAP_ARM`→`AP_ARM`), así que jamás logueó nada — corregido a `data[:32]` y ahora registra todo el tráfico (`KB -> ...`). Diagnóstico: `V`→`VER_CAPTURE_24262`; logs extra `CAP_TGL_1/0` (toggle manual+estado) y `CAP_S_SKIP` (`S` tardía sin armar).
- Fix 2026-09-09 (enganche "muerto"): el clic se perdía porque el overlay aún no aceptaba input a los ~19ms de detectarse — el handler de `S` espera 150ms antes de enganchar. Validado: 5/5 holds por `CAP_EVT` a ~245ms totales (~220ms Windows + ~20ms ida/vuelta).
