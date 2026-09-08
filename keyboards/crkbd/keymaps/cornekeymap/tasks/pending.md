---
cssclasses: no-title
active: captura
---

# 🚀 cornekeymap

> [!focus] **captura**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-progress">▶</span> <span class="num num-progress">12</span></span><span class="sep">│</span><span class="annex">📎 [contrato](attachments/captura/spec-captura-evento.md)</span>
> > [!task]+ <span class="status-progress">▶</span>**Task 12** · Captura en un paso: validar evento de Windows para el clic sostenido<span class="sep">│</span><span class="risk-low">△ low</span><span class="sep">│</span><span class="beat">▰▱▱<span class="beat-hero">▱</span></span><span class="sep">│</span><span class="since" data-since="2026-09-08">0d</span>
> > El hold de `TDQ_MOUSE_HOLD` es inestable por tiempos fijos. Hay que validar si Windows avisa
> > cuando el overlay de Recortes está listo. Estrategia validada en el contrato.
> > - [ ] Validar ventana del overlay con Python+ctypes y medir latencia del aviso `raw_hid`
> > - [ ] Comparar contra `Win+Shift+S` y dos taps; dejar recomendación escrita
> > - [ ] 👤 Aprobar la estrategia elegida (la implementación será la Tarea 12.1)
> >
> > > [!desc]- completed (1)
> > > - [x] Probar espera fija de 500ms — estable en varias pruebas (a ~300ms había inestabilidad); el evento de Windows solo aportaría velocidad; seguir monitoreando

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
