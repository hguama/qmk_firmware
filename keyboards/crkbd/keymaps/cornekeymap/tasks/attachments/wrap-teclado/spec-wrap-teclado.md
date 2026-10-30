# Especificación: wrap-around con las teclas de ratón del Corne

## 0. Manifiesto

| id  | archivo                | rol      | estado                                                                      |
| --- | ---------------------- | -------- | --------------------------------------------------------------------------- |
| W1  | `spec-wrap-teclado.md` | contrato | decidido · 2026-10-30 · opción (a) con 2 toques; falta confirmación de Hero |

## 1. Objetivo

Que el puntero movido con `_MOUSE_KEY` y `_MOUSE_FAST` pase al borde opuesto de la pantalla, como pasa con el ratón físico.

## 2. Lo actual

- `status_script/utils/wrap_around` salta cuando hay una racha de empuje contra un borde: `RESISTENCIA_MS` = 90 ms, y una pausa de más de `PAUSA_MAX` = 60 ms corta la racha.
- El empuje viene de `linux/kwin/CursorPos.qml`: KWin rebota el cursor 1 px en los bordes reservados, y cada toque del píxel del borde cuenta.
- El ratón físico manda eventos cada pocos ms. El Corne manda pasos separados, así que la racha se corta.

## 3. Medición (2026-10-30)

Con un dispositivo virtual que imita los pulsos, contra el borde izquierdo:

| Pulsos                                  | ¿Saltó?         |
| --------------------------------------- | --------------- |
| Ratón físico simulado: 8 px cada 8 ms   | ✅ a los 96 ms  |
| `_MOUSE_KEY` sostenido: 8 px cada 40 ms | ✅ a los 128 ms |
| `_MOUSE_FAST` sostenido: cada 120 ms    | ❌              |
| Toques seguidos: cada 150 ms            | ❌              |
| Toques sueltos: cada 500 ms             | ❌              |

**Cada toque contra el borde sí genera exactamente una señal de empuje** (4 toques cada 400 ms → 4 empujes, con x fijo en 1). La señal existe; lo que falla es la regla de tiempo. El firmware no necesita cambiar.

## 4. Opciones

| Camino                                                   | Qué hace                                                                                                                                      | Pro                                                                                                                           | Contra                                                                                                |
| -------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------- |
| (a) Regla por toques en las capas de ratón (recomendado) | status_script ya sabe la capa activa. En `MOUSE*`, en vez de 90 ms seguidos, cuenta N empujes contra el mismo borde con pausas de hasta 0,6 s | ✅ solo cambia status_script, sin flashear; ✅ conserva lo demás: esquinas, recorrer el borde y arrastrar con botón no saltan | Depende de que status_script esté corriendo (igual que hoy)                                           |
| (b) El firmware avisa la dirección por raw HID           | Cada salto manda su dirección; el script salta si el cursor ya está en ese borde                                                              | ✅ no depende del rebote de KWin                                                                                              | ❌ toca dos proyectos; ❌ gasta memoria del firmware; ❌ hay que replicar a mano la regla de arrastre |
| (c) Bajar `MOUSE_SALTO_REPETIR` a 40 ms                  | Repetición más rápida en `_MOUSE_FAST`                                                                                                        | Cambio de una línea                                                                                                           | ❌ recorre la pantalla mucho más rápido; ❌ no arregla los toques                                     |

## 5. Recomendado: (a) — diseño

```python
# utils/wrap_around
TOQUES_TECLADO = 3     # empujes para saltar en las capas de ratón
PAUSA_TECLADO  = 0.6   # s sin empuje que cortan la cuenta

def set_teclado(activo): ...   # lo llama actualizar_ui según la capa
```

- En `_al_muestrear`: la pausa sale de `PAUSA_TECLADO` si está en modo teclado; la condición de tiempo pasa a contar empujes.
- Llegar al borde ya cuenta como un empuje. Con 3: el que llega + 2 toques de más. Así un toque de más no te manda al otro lado cuando vas a hacer clic en algo pegado al borde.
- Al sostener en `_MOUSE_FAST`, salta a ~0,25 s de llegar al borde.

## 6. Preguntas para Hero

1. ¿Va (a)?
2. ¿Cuántos empujes? Recomendado 3: el que llega + 2 toques de más. Con 2 salta más rápido, pero un toque de más ya te cambia de lado.

## 7. Pruebas

1. `_MOUSE_FAST` sostenido contra cada borde: salta.
2. `_MOUSE_KEY` con toques: llegar y un toque de más no salta; el siguiente sí.
3. Recorrer el borde a lo largo no salta. Las esquinas no saltan.
4. Con `MOUSE_HOLD` enganchado no salta.
5. El ratón físico sigue igual que hoy, también con la capa de ratón apagada.

## 8. Decisión de Hero (2026-10-30)

Opción (a) con `TOQUES_TECLADO = 2`: el toque que choca con el borde y uno más. Se agregó `SEPARACION_TOQUE` = 30 ms para que un salto largo, que llega en tramos, no cuente como dos toques. También se corrigió un `KeyError` que ya existía al empujar en una esquina sin racha activa.

**Ajuste tras la prueba de Hero:** con `PAUSA_TECLADO` = 0,6 s el salto era intermitente: los registros mostraban toques de hasta 575 ms y, al pasar de 0,6 s, la cuenta se reiniciaba sin avisar. Se midió que cada toque de `_MOUSE_FAST` da su señal (6 de 6). Se quitó el límite de tiempo: la cuenta dura mientras el cursor siga en ese borde.
