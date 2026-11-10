# Estudio — símbolos en `_AI` y `_SYMB`

Fecha: 2026-11-10 (segunda versión, con `_AI`) · Estado: **propuesta, esperando decisión de Hero**. Lo único hecho en `keymap.c` es la limpieza de `_AI` (ver su sección).

## Pedido

La capa `_SYMB` se diseñó para programar a mano. Hoy Hero trabaja en la terminal con Claude Code y dicta por voz
(el dictado ya pone la puntuación), así que casi no teclea símbolos. Quiere:

1. Clasificar los símbolos por importancia para trabajar con un agente de terminal.
2. Dejar solo los importantes, los de más uso en la fila central.
3. Mandar los "supremamente extraños" a una sola tecla que escriba una cadena con todos, para copiarlos.
4. Valorar `(` y `)` en dos teclas.
5. **Nuevo:** `_AI` ya tiene `/`, `@` y `#` en la mano derecha y Hero la usa para la barra. Validar si `#` es un símbolo de IA y comparar
   dos caminos: **(1)** repartir entre `_AI` y `_SYMB`, o **(2)** dejar todo en `_SYMB`.

## Situación actual

Columnas c0..c5 = mitad izquierda (c0 exterior), c6..c11 = mitad derecha (c6 interior). Tap / hold.

`_SYMB` (`keymap.c:1451-1464`): 25 teclas de símbolos (38 salidas), dos `QK_BOOT` y `TO(_BASE)` (y `C(S(KC_ENT))`, quitada el 2026-11-10).

```
          c0       c1        c2       c3          c4     c5   |  c6    c7    c8   c9       c10      c11
arriba    ·        * / +     - / _    !           ~      ·    |  ·     `     %    : / ::   [ / ]    ·
centro    ? / ¿    " / '     ,        ·           ||     ·    |  ·     &&    .    = / ==   ( / )    { / }
abajo     |>       -> / <-   < / <=   > / >=      ·      BOOT |  BOOT  ·     $    ;        != / ¡   ^
```

`_AI` **antes de la limpieza** (`keymap.c:1423-1433`): izquierda con atajos de ventanas, pestañas y escritorios; derecha con 3 símbolos y 12 teclas libres.

```
          c0        c1       c2        c3        c4       c5   |  c6    c7    c8        c9        c10   c11
arriba    ·         Alt+←    Alt+→     Sup+↓     ·        ·    |  libre libre #         / / \     @     libre
centro    ·         ·        C+S+F12   C+S+Tab   C+Tab    ·    |  libre libre libre    libre     libre libre
abajo     C+Sup+D   ·        Sup+C+←   Sup+C+→   ·        ·    |  libre libre Sup+C+←  Sup+C+→   libre C+Sup+D
pulgares  ·  ·  ·                                              |  TO(_BASE)  ·  ·
```

## Limpieza de `_AI` (hecha el 2026-11-10)

Hero notó que los atajos de ventanas, pestañas y escritorios de `_AI` ya viven en `_MOVE_WIN` (se abre con los pulgares de `_BASE` y con el izquierdo de `_MOVE`).
Se validó cruzando cada tecla de `_AI` contra las otras 13 capas del keymap:

| Tecla de `_AI`               | Dónde más existe                                                                |
| ---------------------------- | ------------------------------------------------------------------------------- |
| `Alt+←`                      | `_MOVE_WIN` (centro c1 y c10)                                                   |
| `Alt+→`                      | **ninguna otra capa**, pero sobra: `Alt+←` ya recorre los terminales de Konsole |
| `Super+↓`                    | `_MOVE_WIN` (centro c0 y c11)                                                   |
| `Ctrl+Shift+F12`             | `_MOVE_WIN` (arriba c1 y c10)                                                   |
| `Ctrl+Shift+Tab`             | `_MOVE_WIN` (centro c2 y c8)                                                    |
| `Ctrl+Tab`                   | `_MOVE_WIN` (centro c3 y c9)                                                    |
| `Ctrl+Super+D` (×2 en `_AI`) | `_MOVE_WIN` (abajo c7)                                                          |
| `Super+Ctrl+←` (×2 en `_AI`) | `_MOVE_WIN` (abajo c2 y c8)                                                     |
| `Super+Ctrl+→` (×2 en `_AI`) | `_MOVE_WIN` (abajo c3 y c9)                                                     |

Se quitaron las 12 teclas. `Alt+→` no estaba en otra capa, pero sobra: en Konsole es `focus-view-next` ("Focus Next Terminal") y `Alt+←` hace lo mismo hacia el otro lado, así que Hero dejó solo la izquierda a propósito. Se había perdido de `_MOVE_WIN` en la reorganización del 2026-10-29 (commit `4ba96fc062`) sin consecuencias.
Compilado sin errores: 26.452 de 28.672 bytes (92 %, 2.220 libres). El tamaño no cambia porque cada tecla ocupa lo mismo.

```
          c0  c1  c2     c3  c4  c5   |  c6        c7  c8  c9      c10  c11
arriba    ·   ·   ·      ·   ·   ·    |  ·         ·   #   / / \   @    ·
centro    ·   ·   ·      ·   ·   ·    |  ·         ·   ·   ·       ·    ·
abajo     ·   ·   ·      ·   ·   ·    |  ·         ·   ·   ·       ·    ·
pulgares  ·  ·  ·                      |  TO(_BASE)  ·  ·
```

`_AI` queda con 38 teclas libres de 42 (solo `#`, `/`+`\`, `@` y `TO(_BASE)`), y las dos mitades. Ya no hace falta limitarse a la mano derecha.

Consecuencias:

- `_ALFA` no abre `_MOVE_WIN`. Mientras se escribe en `_ALFA`, cambiar de pestaña o de escritorio ya no se puede con `A` sostenida: hay que pasar antes a `_BASE`.
- Desaparece el roce con los atajos de la mano izquierda al sostener `_AI` (la pregunta 3 abierta de la tarea del turbo del trackball).
- `Alt+→` no se pasa a `_MOVE_WIN`: no hace falta (Hero, 2026-11-10).

Entradas: `_SYMB` se abre sosteniendo `E` (anular izquierdo) o `S` (anular derecho) en `_ALFA`. `_AI` se abre sosteniendo `A` (meñique izquierdo)
en `_ALFA`, o con los dos `LT(_AI, …)` de los meñiques en `_BASE`. Sostener `_AI` por cualquiera de esas vías manda `LAYER_AI` a status_script,
que activa el turbo del trackball (tarea 23); no estorba al teclear.

Problemas vistos en `_SYMB`:

- De las 10 teclas de la fila central, solo 4 son de uso real. `-`, `_`, `~`, `:`, `!` están una fila más arriba y `/` no está.
- Falta un `|` solo y un `&` solo: hay `||`, `&&` y `|>`.
- Hay muchas teclas tap/hold para símbolos de lenguajes de programación (`||`, `|>`, `-> <-`, `::`, `!=`, `<= >=`, `{ }`), cada una con su manejador.
- `¿`, `¡` y `ñ` usan códigos Alt numéricos (`Alt+0191`, `Alt+0161`, `Alt+0241`): solo funcionan en Windows. En Linux, con layout `us` y sin AltGr
  (comprobado con `setxkbmap`), no deberían escribir nada. **Deducido del código, no probado en el teclado.**

## Validación: qué símbolos tienen función en Claude Code

Fuente: documentación oficial (`code.claude.com/docs/en/interactive-mode`, tablas "Quick commands" y "Multiline input", y la guía de memoria),
consultada el 2026-11-10. La versión instalada es 2.1.322.

| Símbolo                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   | Función en Claude Code                                                                                                                                                                       | Uso de Hero (48 días)                                                                   | Veredicto                        |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------- | -------------------------------- |
| `/` al inicio                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             | Abre comandos y skills; también completa a mitad de prompt si va tras un espacio                                                                                                             | 190 prompts empiezan con `/` (≈4 por día activo, 24 comandos)                           | Imprescindible, ya está en `_AI` |
| `!` al inicio                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             | Modo shell: ejecuta el comando y mete su salida en la sesión                                                                                                                                 | 21 prompts, en 12 días                                                                  | **Falta en `_AI`**               |
| `@`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | Menciona una ruta de archivo con autocompletado                                                                                                                                              | 1 prompt, suelto (no como mención)                                                      | Función real, poco uso: se queda |
| `\` + Enter | Salto de línea en cualquier terminal. Shift+Enter solo es nativo en algunas (Konsole no figura) y `Ctrl+J` funciona en todas | 0 prompts | Ya existe como hold de `/` |                                                                                                                                                                                              |                                                                                         | |                                                                                                                                                                                              |                                                                                         | |                                                                                                                                                                                              |                                                                                         |                                  |
| `:`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | `:nombre:` inserta un emoji; con 2 o más letras abre sugerencias (se apaga con `emojiCompletionEnabled`)                                                                                     | 61 prompts                                                                              | Sigue como símbolo de texto      |
| `?`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | Con el prompt vacío abre la ayuda de atajos                                                                                                                                                  | 275 prompts (los pone el dictado)                                                       | Texto, en `_SYMB`                |
| `#`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | **Sin función** en las tablas de atajos ni en la guía de memoria. Una incidencia de GitHub reporta que dejó de funcionar en 2.0.74; no encontré en el changelog la versión exacta de la baja | 6 prompts, ninguno al inicio; 5 de sus 12 apariciones van seguidas de un número (`#24`) | **No es símbolo de IA**          |

Prueba de 5 segundos para `#`: escribir `#` al inicio de un prompt en Claude Code. Si no cambia el modo ni aparece nada, no tiene función en esta versión.

## Evidencia de uso

Fuente: `~/.claude/history.jsonl` (3.060 prompts, 2.315 únicos, 2026-09-22 → 2026-11-10, 48 días con actividad) y `~/.bash_history`
(1.000 líneas, 289 únicas; sin los marcadores de pegado `ESC[200~`). Solo se contaron símbolos, no se copió contenido.
Los `[Pasted text #N]` se descartaron. Cifras = prompts que contienen el símbolo y líneas únicas de bash que lo contienen.

| Símbolo                                         |                 Prompts |                       % |    Bash únicas | Nota                                                                                 |
| ----------------------------------------------- | ----------------------: | ----------------------: | -------------: | ------------------------------------------------------------------------------------ |
| `/`                                             |                     242 |                     7,9 |            129 | 190 prompts empiezan con `/` (`/model` 60, `/context` 42, `/clear` 28, `/effort` 17) |
| `-`                                             |                      63 |                     2,1 |            168 | flags `--x`                                                                          |
| `_`                                             |                      48 |                     1,6 |             59 | nombres de archivo                                                                   |
| `~`                                             |                      15 |                     0,5 |             49 | `~/`                                                                                 |
| `&&`                                            |                       8 |                     0,3 |             40 | 39 de las 40 líneas son `cd … && …`                                                  |
| `!`                                             |                      21 |                     0,7 |             13 | los 21 prompts empiezan con `!`                                                      |
| `"` / `'`                                       |                 11 / 14 |               0,4 / 0,5 |         16 / 2 |                                                                                      |
| `:`                                             |                      61 |                     2,0 |              8 |                                                                                      |
| `( )`                                           |                 29 / 29 |                     0,9 |          5 / 6 | siempre en pareja                                                                    |
| `=` `;` `$` `>`                                 |           6 · 4 · 1 · 4 |                    ≤0,2 |  6 · 6 · 4 · 1 |                                                                                      |
| `\|\|` / `\|`                                   |                   3 / 1 |               0,1 / 0,0 |          7 / 2 | `\|\|` repite 41 veces la misma docena de líneas                                     |
| `#` / `@`                                       |                   6 / 1 |               0,2 / 0,0 |          1 / 0 |                                                                                      |
| `.` `,` `?` `¿`                                 | 1617 · 1528 · 275 · 259 | 52,8 · 49,9 · 9,0 · 8,5 | 39 · 1 · 0 · 0 | **los pone el dictado**, no son tecleo                                               |
| `` ` `` `{ }` `[ ]` `*` `^` `%` `+` `<` `&` `\` |                     0–3 |                    ≤0,1 |            0–2 | casi nunca                                                                           |
| `-> <- => \|> != == :: <= >=`                   |                       0 |                       0 |            0–1 | nunca                                                                                |

Frecuencia: `/` al inicio ≈ 4 por día activo (41 de 48 días, máximo 13 en un día); `!` ≈ 0,4. Con tan poco uso, la comodidad de la posición pesa
menos que no tener que reaprender.

Límites del dato: no distingue tecleado, dictado y pegado corto; el bash son solo 1.000 líneas; no incluye lo que Claude teclea por ti.

Por teclas actuales de `_SYMB`: 10 son de uso real, 10 de uso mínimo (`* +`, `%`, `[ ]`, `||`, `=`, `< <=`, `> >=`, `$`, `;`, `^`) y 5 sin ningún uso
(`` ` ``, `{ }`, `|>`, `-> <-`, `!= ¡`).

## Clasificación

- **Nivel 1, agente y terminal:** `/`, `!`, `@`, `-`, `_`, `~`, `.`, `&&`, `"` (hold `'`). `#` se queda por costumbre aunque no sea de IA.
- **Nivel 2, texto y segundo plano:** `, . ? ¿ ! ¡ ( )` como respaldo del dictado, y `: | = ; $ >` (uso bajo, pero la terminal los pide al escribir a mano).
- **Nivel 3, extraños (a la tecla de extraños):** `` ` ^ % * + & < [ ] { } `` y los compuestos `-> <- |> != == <= >= ::`.

Todo lo que hay hoy sigue alcanzable: o tiene tecla, o sale en la cadena de la tecla de extraños. No se pierde ninguna de las 38 salidas.

## Opción 1 — dos capas (recomendada)

`_AI` queda como la capa de agente y terminal; `_SYMB` como la de texto y segundo plano. `/` y `@` no se mueven.
`●` = tecla nueva o movida.

`_AI`, mitad derecha (la izquierda no cambia):

```
          c6     c7     c8     c9       c10    c11
arriba    ·      #●     !●     / / \    @      ·
centro    .●     -●     _●     &&●      ~●     " / '●
abajo     ·      ·      Sup+C+← Sup+C+→  ·      C+Sup+D
```

`_SYMB`:

```
          c0       c1    c2    c3           c4    c5       |  c6    c7     c8    c9    c10   c11
arriba    ·        ·     ·     ·            ·     ·        |  ·     ·      ·     ·     ·     ·
centro    ? / ¿    ,     .     (            )     ! / ¡    |  :     | / ||  =     ;     $     >
abajo     ·        ·     ·     ·            ·     BOOT     |  BOOT  ·      ·     ·     ·     EXTRAÑOS
```

Resultado: `_AI` 10 teclas de símbolos (6 nuevas, `#` corrido una tecla), `_SYMB` 13 teclas con la de extraños. Total 23 en lugar de 28.

## Opción 2 — una sola capa

Es la propuesta de la primera versión: todo en `_SYMB` con la regla izquierda = texto / derecha = terminal (sostener `E` para la derecha, `S` para la izquierda).
`_AI` no se toca; `/` queda duplicado en `_SYMB` o se acepta que vive en dos sitios.

```
          c0       c1       c2    c3           c4    c5       |  c6    c7     c8      c9    c10   c11
arriba    ·        " / '    ·     ·            ·     ·        |  ·     &&     | / ||  $     ;     ·
centro    ? / ¿    ,        .     (            )     ! / ¡    |  .     / / \  -       _     ~     :
abajo     ·        ·        ·     ·            ·     BOOT     |  BOOT  =      >       ·     ·     EXTRAÑOS
```

Resultado: 20 teclas en `_SYMB` más las 3 que ya tiene `_AI`. Total 23.

## Comparación

| Criterio                                               | Opción 1 · dos capas                                                                                                                                           | Opción 2 · una capa                                   |
| ------------------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------- |
| Memoria muscular                                       | `/` y `@` no se mueven; solo `#`                                                                                                                               | `/` pasa a una tecla nueva (o queda duplicada)        |
| Teclas libres de `_AI`                                 | Usa 6 de las 38 (tras la limpieza)                                                                                                                             | No las usa                                            |
| Secuencia de terminal (`cd ~/dev_x && ls -la`)         | Todo en `_AI`, una sola entrada                                                                                                                                | Todo en la mitad derecha de `_SYMB`, una sola entrada |
| Texto y terminal mezclados (`git commit -m "fix (x)"`) | Cambias de capa entre `"` y `(`                                                                                                                                | Cambias de mitad entre `"` y `(`                      |
| Qué aprender                                           | `A` para agente y terminal, `E`/`S` para texto                                                                                                                 | `E` y `S` con la regla de mitades                     |
| Riesgo                                                 | Sin los atajos de la izquierda, ya no hay roces. Cambiar de pestaña o de escritorio escribiendo en `_ALFA` exige pasar a `_BASE` (ocurre con las dos opciones) | Ninguno nuevo                                         |
| Esfuerzo                                               | Editar dos capas                                                                                                                                               | Editar una capa                                       |

**Recomendación: opción 1.** No obliga a reaprender `/` ni `@`, aprovecha teclas vacías, cada capa queda con un nombre que dice lo que contiene
y las secuencias típicas de terminal caben en una sola entrada. El coste es tener dos entradas que recordar y cambiar de capa al mezclar texto con terminal,
algo que la opción 2 también obliga a hacer con sus dos mitades.

## Paréntesis: dos teclas

Hoy `(` es tap y `)` es hold de la misma tecla, con `TAPPING_TERM` de 175 ms (`get_tapping_term` no la excluye): una pulsación un poco lenta escribe `)` en vez de `(`.
En los datos `(` y `)` salen siempre en pareja (29 y 29). **Recomendación:** dos teclas normales (`KC_LPRN`, `KC_RPRN`) juntas en la fila central izquierda de `_SYMB`
(`c3` y `c4`). No necesitan manejador. `{ }` y `[ ]` pasan a la tecla de extraños (0 y 2 prompts).

## Tecla de extraños

Una tecla nueva (`SYM_RAROS`) escribe con `SEND_STRING` una sola línea de tokens separados por espacio, para seleccionarlos con el ratón y copiarlos:

```
` ^ % * + & < [ ] { } -> <- |> != == <= >= ::
```

- 19 tokens, solo ASCII, por eso funciona en cualquier sistema con layout `us`.
- No incluye los que ya tienen tecla, en especial `/`, `!` y `@`, que en el prompt de Claude Code abren un menú o cambian de modo.
- Posición propuesta: abajo a la derecha, columna exterior de `_SYMB` (`c11`, hoy `^`).
- Coste: una línea de `SEND_STRING` y un caso en `process_record_user`.

## Riesgos y notas de implementación

- **No renumerar el enum `custom_keycodes`.** `LT(CUT,COPY)` y `LT(SEL_ALL,KC_SPACE)` usan keycodes personalizados como número de capa (ver Fix 1 en `fixes.md`,
  `layer & 0xF`) y `SEL_ALL` tiene valor fijo. Al quitar teclas, dejar el hueco en el enum.
- Manejadores a quitar de `process_record_user`: `DOUBLE_PIPE`, `PIPE_M`, `DOUBLE_COLON`, `LBRC2`, `LLAMBDA`, `NOT_EQUAL` (su rama de `¡` pasa a la tecla `!`/`¡`),
  `LT(0,KC_LABK)`, `LT(0,KC_RABK)`, `LT(0,KC_LPRN)`, `LT(0,KC_LCBR)`. `ASTRISK_PLUS`, `EQUAL_DBL`, `LT(0,KC_MINS)` y `LT(0,KC_SLSH)` se quedan porque `_NUMB` y `_AI` los usan.
- Teclas nuevas con manejador: `!`/`¡` (tap/hold), `|`/`||` (tap/hold) y la de extraños. `"`/`'` reutiliza `LT(0,KC_DQT)` y `&&` reutiliza `AMP_DOUBLE`.
  El resto son keycodes normales (`KC_EXLM`, `KC_MINS`, `KC_UNDS`, `KC_DOT`, `S(KC_GRAVE)`, `KC_COLN`, `KC_EQL`, `KC_SCLN`, `KC_DLR`, `KC_GT`, `KC_LPRN`, `KC_RPRN`).
- Opción 1: sostener `A` en `_ALFA` es un `LT` normal. En cuanto se activa `_AI` se manda `LAYER_AI` al status_script y arranca el turbo del trackball.
  Solo importa si se mueve un trackball mientras se teclea.
- `C(S(KC_ENT))` (Ctrl+Shift+Enter) **se quitó de `_SYMB` el 2026-11-10.** En JetBrains es "Complete Current Statement" (cierra la instrucción, por ejemplo con `;`; confirmado en la ayuda de Rider/ReSharper). En el Linux de Hero nada más la usa: no está en Konsole, Kate, Dolphin, KDE ni en su VS Code, y Claude Code solo usa `Ctrl+Enter` (enviar mensajes en cola). JetBrains sigue instalado (IDEA CE, PyCharm CE, CLion), pero CLion se abrió por última vez el 2026-09-25 y Hero ya quitó sus capas (tarea 1) y gestos (tarea 17). Se restaura con git. En una terminal sin teclas extendidas es probable que llegue como Enter normal y envíe el prompt (deducido, no probado).
- `_DEL` conserva 4 atajos de JetBrains del mismo origen: `C(A(KC_L))` (reformatear), `C(S(KC_J))` (unir líneas), `C(A(KC_I))` (indentar) y `C(A(KC_O))` (optimizar imports). Fuera de esta tarea; se pueden revisar igual.
- Los `QK_BOOT` y `TO(_BASE)` no se tocan.
- Compilar y mirar el tamaño después. Hoy el firmware ocupa 26.452 de 28.672 bytes (2.220 libres; la cifra de 24.126 de la guía está desactualizada). Los 10 manejadores que se quitan liberan espacio para la tecla de extraños.

## Decisiones abiertas

1. ¿Opción 1 (dos capas) u opción 2 (una capa)? (recomendado: 1)
2. En `_AI`: ¿se corre `#` una tecla a la izquierda para dejar `! / @` juntos, o se queda y `!` va a su izquierda? (recomendado: correrlo)
3. ¿Comillas `"` `'` en la mano derecha de `_AI` (meñique exterior, fila central)? (recomendado: sí)
4. ¿Paréntesis en dos teclas en `c3` y `c4` de `_SYMB`? (recomendado)
5. ¿Se quedan `$ ; > =` como teclas de nivel 2 o también pasan a la tecla de extraños? (recomendado: se quedan, no cuestan nada)
6. ¿La tecla de extraños abajo a la derecha, con esos 19 tokens?
7. ¿`¿ ¡ ñ` en Linux como tarea aparte? (probablemente hoy no escriben; requiere Unicode o AltGr en el sistema)
