---
cssclasses: no-title
active: migra-captura
---

# 🚀 cornekeymap

> [!focus] **migra-captura**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-progress">▶</span> <span class="num num-progress">15</span></span><span class="sep">│</span><span class="annex">📎 [contrato](attachments/migra-captura/spec-migracion-captura.md)</span>
> > [!task]+ <span class="status-progress">▶</span>**Task 15** · Migrar la captura con evento al script principal<span class="sep">│</span><span class="risk-med">◆ medium</span><span class="sep">│</span><span class="beat">▰▰▱<span class="beat-hero">▱</span></span><span class="sep">│</span><span class="since" data-since="2026-09-09">0d</span>
> > Traer la captura con evento (Task 12.1, ya validada) al script principal: módulo que detecta
> > el overlay y manda `S`, más rama que interpreta los `CAP_*`. Related to Task 12.1.
> > Sin esto el watcher suelto compite por el `raw_hid` y no hay captura rápida en uso diario.
> > - [ ] Implementar el módulo en `layer_status_script.py` (hilo overlay + rama `CAP_*` + envío `S`; repo ajeno en `main` limpio)
> > - [ ] 👤 Contrastar holds contra el watcher y retirar el watcher suelto
> >
> > > [!desc]- completed (2)
> > > - [x] Especificar el módulo — contrato en `attachments/migra-captura/spec-migracion-captura.md`
> > > - [x] 👤 Aprobar la especificación — aprobada por Hero 2026-09-09

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

> [!block]- **actualizar-skill**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-untouched">☐</span> <span class="num num-untouched">14</span></span>
> > [!task]+ <span class="status-untouched">☐</span>**Task 14** · Aplicar la versión vigente de `task-manager` a este proyecto<span class="sep">│</span><span class="risk-low">△ low</span><span class="sep">│</span><span class="beat">▱▱<span class="beat-hero">▱</span></span>
> > Este `pending.md`/`completed.md`/`fixes.md` quedó desalineado del formato vigente de la skill (Blocks,
> > índice de `completed.md`, filtros retirados). Releer `SKILL.md` y `markdown-format.md` de `task-manager`
> > y aplicar lo que falte.
> > - [ ] Comparar este `pending.md`/`completed.md`/`fixes.md` contra `SKILL.md` y `markdown-format.md` vigentes, y listar las diferencias
> > - [ ] Aplicar el formato vigente en los tres archivos
> > - [ ] 👤 Aprobar el resultado
