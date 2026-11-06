# Especificación: turbo del trackball al sostener la tecla de `_AI`

## 0. Manifiesto

| id  | archivo                   | rol      | estado                                                                    |
| --- | ------------------------- | -------- | ------------------------------------------------------------------------- |
| T1  | `spec-turbo-trackball.md` | contrato | cerrado · 2026-11-06 · multiplicar el desplazamiento; confirmado por Hero |

## 1. Objetivo

Con el trackball (Logitech ERGO M575) los movimientos cortos son cómodos, pero los largos hacia el centro de la pantalla, sobre todo horizontales, cansan el pulgar. La velocidad precisa se queda por defecto; **mientras se sostiene `LT(_AI, MOVE_WIN_TG)`** el puntero va más rápido, y al soltar vuelve a la normal.

## 2. Lo actual (medido el 2026-11-06)

- El M575 es otro dispositivo: **QMK no puede cambiar su velocidad**. Hay que hacerlo del lado del sistema.
- KWin expone el M575 como `/org/kde/KWin/InputDevice/event9` con `pointerAcceleration` escribible. Cambiarlo por D-Bus tarda **3 ms** y se aplica al instante.
- **Ya está en el máximo**: `pointerAcceleration = 1.0` (el rango es −1…1), perfil adaptativo. Hoy no hay margen para subir más por KWin.
- El sensor del M575 está en **400 DPI**, y Solaar deja subirlo hasta 2000 (en pasos de 50). Pero `solaar config` tarda **~1,9 s**: sirve para un ajuste fijo, no para cambiar al sostener.
- El firmware ya avisa la capa: al sostener la tecla entra `LAYER_AI`, al soltar vuelve la anterior. `status_script` lo recibe en `actualizar_ui(capa_msg)`. **No hace falta tocar el firmware.**
- Ojo: KWin guarda en `~/.config/kcminputrc` lo que se le cambia por D-Bus. Si el script se cae en modo turbo, la velocidad rápida se queda guardada.

## 3. Opciones

| Camino                                                | Qué hace                                                                                                                                                                                                                    | Pro                                                                          | Contra                                                                                                           |
| ----------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------- |
| (a) Subir el DPI una vez + KWin en vivo (recomendado) | Solaar sube el M575 a 800 DPI, una sola vez. La aceleración base de KWin se baja para que la precisión quede como hoy, por ejemplo 0,3 (se calibra). Al ver `LAYER_AI`, status_script la pone en 1,0; al salir, la devuelve | ✅ instantáneo; ✅ sin flashear; ✅ reutiliza el aviso de capa que ya existe | Hay que calibrar la base hasta que se sienta igual que hoy; el turbo depende de que status_script esté corriendo |
| (b) Cambiar el DPI al sostener                        | status_script llama a Solaar en cada pulsación                                                                                                                                                                              | Más rango (hasta 2000)                                                       | ❌ ~1,9 s de retardo: inservible                                                                                 |
| (c) Pasar a perfil plano al sostener                  | Cambia de adaptativo a plano con 1,0                                                                                                                                                                                        | Sin tocar el DPI                                                             | ❌ no da más velocidad garantizada: puede ir incluso más lento en gestos rápidos                                 |

## 4. Recomendado: (a) — diseño

```python
# utils/turbo_trackball (status_script, rama linux-port)
DISPOSITIVO = "Logitech ERGO M575"   # se busca por nombre: eventN puede cambiar
ACEL_NORMAL = 0.3    # calibrar: que se sienta como hoy
ACEL_TURBO  = 1.0
CAPA_TURBO  = "LAYER_AI"

def set_capa(capa_msg): ...   # lo llama actualizar_ui; solo escribe si cambia el estado
def restaurar(): ...          # al arrancar y al salir (atexit): deja ACEL_NORMAL
```

- Si el trackball no está conectado, no hace nada.
- Si 800 DPI con 1,0 no alcanza, se sube el DPI a 1000-1200 y se baja más la base.

## 5. Antes de programar: prueba manual

1. Subir el M575 a 800 DPI con Solaar.
2. Probar bases de 0,0, 0,3 y 0,5 hasta que los movimientos cortos se sientan como hoy.
3. Probar 1,0 y ver si los movimientos largos ya no cansan.
4. Si no convence, volver a 400 DPI y 1,0 (como está hoy).

## 6. Preguntas para Hero

1. ¿Va (a)?
2. ¿Hacemos primero la prueba manual (sección 5)?
3. Al sostener la tecla también se activan las teclas de `_AI` (escritorios, pestañas). ¿Te sirve así, o la mano izquierda podría tocar alguna sin querer?

## 7. Pruebas

1. Sostener la tecla: el puntero acelera en menos de 0,2 s (175 ms del `TAPPING_TERM`). Al soltar, vuelve.
2. Tocar la tecla sigue alternando `_MOUSE_KEY` como hoy.
3. Cerrar status_script con el turbo puesto: la velocidad queda en la normal.
4. Desconectar y reconectar el M575: sigue funcionando.

## 8. Decisión de Hero (2026-11-06)

- La velocidad de hoy es la de precisión y no se toca. Se descartó (a) y se eligió **multiplicar el desplazamiento**: al entrar en `LAYER_AI`, el script toma el trackball (grab) y reenvía sus movimientos ×N por un dispositivo virtual que copia los ajustes de KWin del real. Fuera de `_AI`, el trackball va directo al sistema.
- Prueba en `evidence/turbo-prueba.py` (permiso con `evidence/72-trackball-turbo.rules` en `/etc/udev/rules.d/`).
- Factores tras probar: horizontal ×6,4 (×4 por la proporción 1,6 de la pantalla 1920×1200), vertical ×8 (girar la bola en vertical cuesta más con el pulgar).
- Se suma el Kensington Expert Wireless (mano izquierda), con los mismos factores para empezar.
- Firmware: el meñique derecho pasa de `TG(_MOVE)` a `LT(_AI, MOVE_TG)`: toque = alterna `_MOVE` como antes, sostener = `_AI`. Igual en `_BASE` y en `_MOVE`.
- Kensington: velocidad normal de KDE subida de 0,6 a 1,0 (se sentía lenta).
- **Integrado en status_script** (rama `linux-port`) como `utils/turbo_trackball`, con los valores en `TRACKBALLS` (velocidad normal y factores de turbo por trackball). Regla copiada a `linux/72-trackball-turbo.rules`. El script de prueba queda en `evidence/` solo como registro: no correrlo a la vez que el servicio.
