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
