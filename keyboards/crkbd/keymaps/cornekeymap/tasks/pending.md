---
cssclasses: no-title
---

# 🚀 cornekeymap

> [!focus] **limpieza-dev**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-untouched">☐</span> <span class="num num-untouched">6</span></span>
> > [!task]+ <span class="status-untouched">☐</span>**Task 6** · Limpiar `LT(_DEV, KC_ENT)` muerto<span class="sep">│</span><span class="risk-low">△ low</span><span class="sep">│</span><span class="beat">▱▱<span class="beat-hero">▱</span></span>
> > Código muerto en `process_record_user`, fuera de todo layout. La capa `_DEV` se eliminó
> > del enum en la Tarea 1, así que hay que confirmar qué quedó ahí antes de tocarlo.
> > - [ ] Confirmar qué quedó en `process_record_user` tras eliminar `_DEV` del enum
> > - [ ] Retirar el `case` muerto de `LT(_DEV, KC_ENT)`
> > - [ ] 👤 Compilar y confirmar que no queda referencia a `_DEV`

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

> [!block]- **limpieza-teclas**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-untouched">☐</span> <span class="num num-untouched">3</span></span><span class="sep">│</span><span class="annex">📎 [contrato](attachments/limpieza-teclas/spec-limpieza-teclas.md)</span>
> > [!task]+ <span class="status-untouched">☐</span>**Task 3** · Limpieza única de teclas (9 fuera, resto se queda)<span class="sep">│</span><span class="risk-med">◆ medium</span><span class="sep">│</span><span class="beat">▰▰▰<span class="beat-hero">▱</span>▱</span>
> > Limpieza única: salen 9 teclas muertas o reemplazadas y queda registrado por qué se queda
> > el resto. Decisión Hero 2026-09-06: absorbe 4, 5, 5.1, 7, 9 y 10. Detalle en el contrato.
> > - [ ] 👤 Flashear y probar: 4 gestos de `TDQ_MOUSE_HOLD` + liberación con `MS_BTN1`; `ALT_TAB`, `COMM`, espacio y `Ctrl+Z` intactos
> > - [ ] Anotar las líneas en `completed.md` al cerrar
> >
> > > [!desc]- completed (3)
> > > - [x] Eliminar las 9 teclas según el alcance y sustituir `MOUSE_HOLD` en `_MOVE`
> > > - [x] Actualizar README (tablas de estado y categorías)
> > > - [x] Compilar con `qmk compile -kb crkbd/rev1 -km cornekeymap` (.hex OK 2026-09-06, 24.126/28.672)

> [!block]- **captura**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-untouched">☐</span> <span class="num num-untouched">12</span></span><span class="sep">│</span><span class="annex">📎 [contrato](attachments/captura/spec-captura-evento.md)</span>
> > [!task]+ <span class="status-untouched">☐</span>**Task 12** · Captura en un paso: validar evento de Windows para el clic sostenido<span class="sep">│</span><span class="risk-low">△ low</span><span class="sep">│</span><span class="beat">▱▱<span class="beat-hero">▱</span></span>
> > El hold de `TDQ_MOUSE_HOLD` es inestable por tiempos fijos. Hay que validar si Windows avisa
> > cuando el overlay de Recortes está listo. Estrategia validada en el contrato.
> > - [ ] Validar ventana del overlay con Python+ctypes y medir latencia del aviso `raw_hid`
> > - [ ] Comparar contra `Win+Shift+S` y dos taps; dejar recomendación escrita
> > - [ ] 👤 Aprobar la estrategia elegida (la implementación será la Tarea 12.1)
