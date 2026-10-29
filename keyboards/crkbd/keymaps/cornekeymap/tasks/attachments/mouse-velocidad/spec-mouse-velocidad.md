# Especificación: velocidades del ratón en `_MOUSE_KEY`

## 0. Manifiesto

| id  | archivo                   | rol      | estado                                                                                          |
| --- | ------------------------- | -------- | ----------------------------------------------------------------------------------------------- |
| V1  | `spec-mouse-velocidad.md` | contrato | decidido · 2026-10-29 · saltos en `_MOUSE_KEY` y `_MOUSE_FAST` (§7); falta confirmación de Hero |

## 1. Objetivo

- Al entrar a `_MOUSE_KEY`, el puntero va **muy lento y constante** (precisión).
- Una tecla **toggle** (toque entra, toque sale) cambia a un modo **rápido y constante**, que recorre más pantalla.

## 2. Lo actual

- `config.h`: `MK_COMBINED`, `MOUSEKEY_MOVE_DELTA 1`. Sin definir: `MAX_SPEED` 10, `INTERVAL` 20 ms, `TIME_TO_MAX` 30. (`MOUSEKEY_BASE_SPEED 150` no hace nada: es del modo cinético.)
- Hoy el puntero **acelera**: arranca a 1 px cada 20 ms (50 px/s) y en ~0,6 s sube a 10 px cada 20 ms (500 px/s).
- Las flechas del ratón (`MS_UP/DOWN/LEFT/RGHT`) **solo existen en `_MOUSE_KEY`** (keymap.c:1459): cambiar la configuración global solo afecta esta capa.
- En el modo combinado de QMK (quantum/mousekey.c `move_unit`):
  - `MS_ACL0` = **1 px por evento, constante**: lo más lento que da QMK por píxel.
  - `MS_ACL1` = `MAX_SPEED / 2` px por evento, constante (hoy 5 px → 250 px/s).
  - `MS_ACL2` = 127 px por evento, constante: demasiado rápido para sostener.
- `mk_interval` y `mk_max_speed` son variables globales de QMK: se pueden cambiar **en marcha** desde el keymap. Así el modo lento puede ir aún más lento que `ACL0` (más ms entre eventos) sin tocar `config.h`.
- Ya existen `MS_ACL0_TOGGLE` y los LT con `MS_ACL0`/`MS_ACL2` (keymap.c:550, 980, 1135), sin relación con esta capa.

## 3. Diseño común (cualquier opción)

- En `layer_state_set_user`: al **entrar** a `_MOUSE_KEY` → modo lento (`register_code(MS_ACL0)` + `mk_interval` mayor). Al **salir** → se suelta todo y se restauran los valores, para que no quede pegado.
- Tecla nueva `MOUSE_RAPIDO` (toggle) en una tecla libre de la capa; propuesta: **al lado de `MS_UP`** (fila del medio izquierda, hoy `XXXXXXX`). Al salir de la capa vuelve siempre a lento.
- LED 8 de la capa: azul en lento, otro color (p. ej. naranja) en rápido, para saber en qué modo estás.

### Modo lento (propuesto)

| ajuste                      | valor        | resultado                                              |
| --------------------------- | ------------ | ------------------------------------------------------ |
| `MS_ACL0` + intervalo 40 ms | 1 px / 40 ms | **25 px/s**, constante (la mitad del actual más lento) |

Se ajusta fácil: 30 ms ≈ 33 px/s, 20 ms = 50 px/s.

## 4. Opciones para el modo rápido

"Constante… toco y se va a un lugar fijo" se puede leer de dos formas:

| Camino                                       | Qué hace                                                                                                                  | Pro                                                         | Contra                                                                        |
| -------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------- | ----------------------------------------------------------------------------- |
| (a) Velocidad rápida constante (recomendado) | Sostener la flecha mueve a velocidad fija, sin aceleración: `MS_ACL1` con `mk_max_speed` 16 → 8 px / 20 ms = **400 px/s** | ✅ simple, todo de QMK; ✅ sigue siendo sostener para mover | Un toque corto mueve poco (8 px)                                              |
| (b) Salto fijo por toque                     | Cada toque de flecha salta una distancia fija (p. ej. **100 px**) y sostener repite el salto cada ~150 ms                 | ✅ predecible: "3 toques = 300 px"                          | Tecla propia por dirección (4 keycodes nuevos); ocupa más bytes (quedan 2686) |
| (c) Mezcla                                   | (b) al tocar + (a) al sostener                                                                                            | lo mejor de ambos                                           | más código y temporizadores                                                   |

## 5. Preguntas para Hero

1. ¿Rápido = (a) moverse rápido mientras sostienes, o (b) cada toque salta una distancia fija?
2. ¿Te sirve la tecla toggle al lado de `MS_UP`, o la quieres en otra posición?
3. ¿Color del LED para el modo rápido?

## 6. Pruebas

1. Entrar a la capa: el puntero va lento y parejo desde el primer instante.
2. Toggle: cambia a rápido; otro toque vuelve a lento.
3. Salir de la capa en modo rápido y volver a entrar: arranca en lento.
4. Fuera de la capa, `MS_ACL0`/`MS_ACL2` de los LT de Enter y Espacio siguen igual.

## 7. Decisión de Hero (2026-10-29)

Las dos capas se mueven **por saltos**: un toque es un salto, y al sostener espera 300 ms y repite. El modo constante con `MS_ACL0` se probó y se descartó, porque se sentía muy lento incluso a 100 px/s.

| Capa                    | Horizontal         | Vertical                      | Repetición al sostener |
| ----------------------- | ------------------ | ----------------------------- | ---------------------- |
| `_MOUSE_KEY` (capa 12)  | 8 px (un carácter) | 16 px (una línea)             | cada 40 ms             |
| `_MOUSE_FAST` (capa 13) | 300 px             | 180 px (proporción 1920×1200) | cada 120 ms            |

- Teclas `MS_SALTO_ARR/ABA/IZQ/DER` en las mismas posiciones de las dos capas. `TG(_MOUSE_FAST)` entra y sale.
- En `_MOUSE_FAST` todo lo demás es transparente: los clics y `TO(_BASE)` funcionan igual.
- Los saltos copian el reporte de mousekey: no sueltan el clic enganchado de `MOUSE_HOLD`.
- LED 8: azul en `_MOUSE_KEY` y naranja en `_MOUSE_FAST`.
- Ajustes en keymap.c: `MOUSE_PASO_X/Y`, `MOUSE_PASO_REPETIR`, `MOUSE_SALTO_X/Y`, `MOUSE_SALTO_REPETIR`, `MOUSE_SALTO_ESPERA`.
- Ojo: la aceleración del puntero de KDE escala los saltos. Para medidas exactas, usar el perfil "plano" en el ratón del Corne.
