---
cssclasses: no-title
active: fast-rclick
---

# 🚀 cornekeymap

> [!focus] **fast-rclick**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-progress">▶</span> <span class="num num-progress">13</span></span>
> > [!task]+ <span class="status-progress">▶</span>**Task 13** · Tap dance `_FAST`: tap togglea, doble tap y hold clic derecho<span class="sep">│</span><span class="risk-med">◆ medium</span><span class="sep">│</span><span class="beat">▰▰▰<span class="beat-hero">▱</span></span><span class="sep">│</span><span class="since" data-since="2026-09-08">0d</span>
> > Reemplaza ambos `TG(_FAST)` de `_BASE` por un tap dance: single tap togglea `_FAST` igual que hoy;
> > doble tap y hold hacen clic derecho (`MS_BTN2`). Hero 2026-09-08: ambos lados y hold como clic simple.
> > - [ ] 👤 Flashear y contrastar en el teclado
> >
> > > [!desc]- completed (3)
> > > - [x] Especificar: `SINGLE_TAP` -> toggle `_FAST` (`layer_invert`); `SINGLE_HOLD`/`DOUBLE_TAP` -> `tap_code16(MS_BTN2)`
> > > - [x] 👤 Aprobar la especificación — aprobada por Hero 2026-09-08 (ambos lados, hold clic simple)
> > > - [x] Implementar: enum `TDQ_FAST` + action + `tdq_fast_finished` + reemplazo en `_BASE` (.hex OK 2026-09-08, 24.082/28.672)

```button
name 👤
type command
action Task Manager Filters: Filtro - Mios (persona)
class queue-filter-button qfb-mine
```
```button
name ⬤
type command
action Task Manager Filters: Filtro - Con avance
class queue-filter-button qfb-with-progress
```
```button
name ◯
type command
action Task Manager Filters: Filtro - Sin avance
class queue-filter-button qfb-no-progress
```
```button
name │
type copy
action |
class queue-filter-button qfb-divider
```
```button
name ⬟
type command
action Task Manager Filters: Filtro - Riesgo alto
class queue-filter-button qfb-risk
```
```button
name ▶
type command
action Task Manager Filters: Filtro - En curso
class queue-filter-button qfb-progress
```
```button
name ❚❚
type command
action Task Manager Filters: Filtro - En pausa
class queue-filter-button qfb-paused
```
```button
name ☐
type command
action Task Manager Filters: Filtro - Tarea sin empezar
class queue-filter-button qfb-not-started
```
```button
name │
type copy
action |
class queue-filter-button qfb-divider
```
```button
name ↩
type command
action Task Manager Filters: Filtro - Restaurar vista
class queue-filter-button qfb-reset
```
```button
name ⇕
type command
action Task Manager Filters: Filtro - Alternar primero de la Queue
class queue-filter-button qfb-toggle-first
```

<!-- queue -->

> [!block]- **captura**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-paused">❚❚</span> <span class="num num-paused">12</span></span><span class="sep">│</span><span class="annex">📎 [contrato](attachments/captura/spec-captura-evento.md)</span>
> > [!task]+ <span class="status-paused">❚❚</span>**Task 12** · Captura en un paso: validar evento de Windows para el clic sostenido<span class="sep">│</span><span class="risk-low">△ low</span><span class="sep">│</span><span class="beat">▰▱▱<span class="beat-hero">▱</span></span><span class="sep">│</span><span class="since" data-since="2026-09-08">0d</span>
> > El hold de `TDQ_MOUSE_HOLD` es inestable por tiempos fijos. Hay que validar si Windows avisa
> > cuando el overlay de Recortes está listo. Estrategia validada en el contrato.
> > Pausa 2026-09-08: Hero pasó a crear el tap dance de `_FAST` (Block fast-rclick).
> > Quedó validada la espera de 500ms (§4 del contrato). Al volver: script de huella Python+ctypes.
> > - [ ] Validar ventana del overlay con Python+ctypes y medir latencia del aviso `raw_hid`
> > - [ ] Comparar contra `Win+Shift+S` y dos taps; dejar recomendación escrita
> > - [ ] 👤 Aprobar la estrategia elegida (la implementación será la Tarea 12.1)
> >
> > > [!desc]- completed (1)
> > > - [x] Probar espera fija de 500ms — estable en varias pruebas (a ~300ms había inestabilidad); el evento de Windows solo aportaría velocidad; seguir monitoreando
