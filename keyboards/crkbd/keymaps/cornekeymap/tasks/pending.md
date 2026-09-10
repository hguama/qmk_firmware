---
cssclasses: no-title
active: poll-eventos
---

# 🚀 cornekeymap

> [!focus] **poll-eventos**<span class="sep">│</span><span class="count">0/1 tasks</span><span class="sep">│</span><span class="tasknums"><span class="status-progress">▶</span> <span class="num num-progress">16</span></span><span class="sep">│</span><span class="annex">📎 [contrato](attachments/poll-eventos/spec-polls-eventos.md)</span>
> > [!task]+ <span class="status-progress">▶</span>**Task 16** · Pasar los polls del script principal a eventos<span class="sep">│</span><span class="risk-high">⬟ high</span><span class="sep">│</span><span class="beat">▰▱▱<span class="beat-hero">▱</span></span><span class="sep">│</span><span class="since" data-since="2026-09-10">0d</span>
> > Pasar los 6 polls permanentes del script principal a eventos, con el modelo de `utils/captura`.
> > Mismo comportamiento, CPU en reposo; excluidos firmware (acotado), captura (ya es eventos) y delays de automatización.
> > - [ ] Ajustar el contrato según los hallazgos de la auditoría (contrato §5)
> > - [ ] Convertir los 6 polls (lector hid, clic-alt, 2× mouse-ui, wrap, scroll-lock) en repo ajeno
> > - [ ] 👤 Contrastar en uso diario (comportamiento idéntico, sin regresiones)
> >
> > > [!desc]- completed (1)
> > > - [x] Auditar polls del repo ajeno: 6 convertibles y 3 excluidos

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
