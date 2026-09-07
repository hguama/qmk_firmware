# Instrucciones para agentes de IA

> ⚠️ Leer antes de trabajar en este proyecto.

Las skills están en `~/.agents/skills/`. Si vas a gestionar el plan de
trabajo, usa la skill `tareas` (`~/.agents/skills/task-manager/SKILL.md`) — si no
se activa sola, leerla directamente desde ahí antes de continuar.

## Compilar (obligatorio después de cada cambio)

```powershell
& "C:\QMK_MSYS\usr\bin\bash.exe" --login -c "export MSYSTEM=MINGW64 QMK_ALLOW_ANY_ENV=1; export PATH=/usr/local/bin:/opt/qmk/bin:/mingw64/bin:/usr/bin:/bin; cd /c/Users/Usuario/qmk_firmware && qmk compile -kb crkbd/rev1 -km cornekeymap"
```

- Al terminar cualquier cambio: compilar de esta manera. Se entrega solamente código (el `.hex` y `.build/` no se commitean).
- Si hay errores: consultar la documentación en `tasks/Docs/` (ver `guia_compilacion.md`).

## Propio de este proyecto

- [`tasks/Docs/tap_dance_guide.md`](tasks/Docs/tap_dance_guide.md) — referencia de
  usuario sobre los tap dances del keymap. No hace falta leerla para
  trabajar, solo si necesitas entender ese comportamiento.
