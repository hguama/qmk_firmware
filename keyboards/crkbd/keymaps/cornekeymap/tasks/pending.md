---
cssclasses: no-title
active: captura
---

# 🚀 cornekeymap

> [!focus] **captura**<span class="sep">│</span><span class="count">1/2 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-progress">▶</span> <span class="num num-progress">12.1</span><span class="status-completed">✓</span> <span class="num num-completed">12</span></span><span class="sep">│</span><span class="annex">📎 [contrato](attachments/captura/spec-captura-evento.md)</span>
> > [!task]+ <span class="status-progress">▶</span>**Task 12.1** · Captura con evento: enganche rápido del clic sostenido<span class="sep">│</span><span class="risk-med">◆ medium</span><span class="sep">│</span><span class="beat">▰▰▰▰▰▰<span class="beat-hero">▱</span></span><span class="sep">│</span><span class="since" data-since="2026-09-09">0d</span>
> > Implementa la estrategia validada (§3 y §5 del contrato): el hold manda `Win+Shift+S` y arma;
> > el watcher avisa con `S` y el firmware engancha, con red de 800ms. Hero 2026-09-09: probar velocidad.
> > - [ ] 👤 Flashear y probar velocidad: 5/5 holds por `CAP_EVT` a ~245ms (vs 800ms red); espera 150ms en `S` corrige el enganche (el overlay no aceptaba el clic inmediato)
> >
> > > [!desc]- completed (6)
> > > - [x] Especificar 12.1: poll-loop de 800ms en el hold + watcher con la huella medida
> > > - [x] Implementar firmware (handler `S` + hold) y watcher (.hex OK 2026-09-09, 24.186/28.672; `raw_hid` verificado con el Corne)
> > > - [x] Corregir bombeo USB en la espera (`raw_hid_task`) + logs `CAP_ARM`/`CAP_EVT`/`CAP_NET` y watcher con reloj, log e hilo lector (.hex OK 24.262/28.672)
> > > - [x] Eliminar duplicado de la raíz: el canónico es `.build/crkbd_rev1_cornekeymap.hex` (guía §6)
> > > - [x] Corregir parseo del watcher (`data[:32]`, perdía la 1ª letra y ocultaba los `CAP_*`) + marcador de versión `V`/`VER_CAPTURE_24262` + logs `CAP_TGL_*`/`CAP_S_SKIP` (.hex OK 24.362/28.672)
> > > - [x] Espera 150ms en el handler de `S` antes de enganchar: el overlay ignoraba el clic inmediato (.hex OK 24.390/28.672, validado 5/5 `CAP_EVT`)
>
> > [!desc]- completed (1) tasks
> > > [!task]- <span class="status-completed">✓</span>**Task 12** · Captura en un paso: validar evento de Windows para el clic sostenido<span class="sep">│</span><span class="risk-low">△ low</span><span class="sep">│</span><span class="beat">▰▰▰▰</span> · <em class="done-note">completed 2026-09-09</em>
> > > > [!desc]- completed (4)
> > > > - [x] Probar espera fija de 500ms — estable en varias pruebas (a ~300ms había inestabilidad); el evento de Windows solo aportaría velocidad; seguir monitoreando
> > > > - [x] Validar ventana del overlay con Python+ctypes — huella `SnipOverlayRootWindow`/`Snipping Tool Overlay`/`SnippingTool.exe`, detectable a ~120ms (`evidence/overlay-probe.py`); hop `raw_hid` respaldado por precedente `R`/`M`
> > > > - [x] Comparar y dejar recomendación — evento es la mejor técnica pero prioridad baja con 500ms estable; ver §5 del contrato
> > > > - [x] 👤 Aprobar la estrategia elegida (la implementación será la Tarea 12.1) — aprobada por Hero 2026-09-09: arrancar 12.1 por velocidad

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

> [!block]- **fast-rclick**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-paused">❚❚</span> <span class="num num-paused">13</span></span>
> > [!task]+ <span class="status-paused">❚❚</span>**Task 13** · Tap dance `_FAST`: tap togglea, doble tap y hold clic derecho<span class="sep">│</span><span class="risk-med">◆ medium</span><span class="sep">│</span><span class="beat">▰▰▰<span class="beat-hero">▱</span></span><span class="sep">│</span><span class="since" data-since="2026-09-08">0d</span>
> > Reemplaza ambos `TG(_FAST)` de `_BASE` por un tap dance: single tap togglea `_FAST` igual que hoy;
> > doble tap y hold hacen clic derecho (`MS_BTN2`). Hero 2026-09-08: ambos lados y hold como clic simple.
> > Pausa 2026-09-08: implementado y compilado (.hex OK 24.082/28.672); falta flashear y contrastar. Hero pasó a validar captura.
> > - [ ] 👤 Flashear y contrastar en el teclado
> >
> > > [!desc]- completed (3)
> > > - [x] Especificar: `SINGLE_TAP` -> toggle `_FAST` (`layer_invert`); `SINGLE_HOLD`/`DOUBLE_TAP` -> `tap_code16(MS_BTN2)`
> > > - [x] 👤 Aprobar la especificación — aprobada por Hero 2026-09-08 (ambos lados, hold clic simple)
> > > - [x] Implementar: enum `TDQ_FAST` + action + `tdq_fast_finished` + reemplazo en `_BASE` (.hex OK 2026-09-08, 24.082/28.672)
