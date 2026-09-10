# Estudio: tap dances activos y duplicaciones

## 0. Manifiesto

| id | archivo | rol | estado |
|---|---|---|---|
| C1 | `spec-duplicados-td.md` | estudio | auditoría · 2026-09-10 · pendiente decisión Hero |

## 1. Inventario (6 TDQs definidos, 4 en uso)

| TD | Gestos | Usado en layouts | Duplicado en otro lado | Veredicto |
|---|---|---|---|---|
| `TDQ_SEL` | tap `C-A`, hold selecciona línea, doble-tap `Espacio` | ❌ en ninguno (sin asignar) | ✅ cubierto hoy vía `LT(SEL_ALL,KC_SPACE)` (en uso) | ✅ Se queda — reserva Hero para selección futura (§5) |
| `TDQ_BOOKMARK` | `S-F11` / `A-2` / `F11` / `C-S-F11` | ✅ capa BOOK ×2 | ❓ `F11` también sale del doble-tap de `TDQ_PASTE` | ✅ Se queda; decidir el solape `F11` (§2) |
| `TDQ_ESC` | `Esc` (+suelta hold) / `Win+D` (+suelta) / `C-F` | ✅ `_BASE`×2, `_MOVE`, `_ALFA` | ⚠️ `Esc` también vía `LSFT_T(KC_ESC)` (otra posición, sin lógica de soltar) | ✅ Se queda (posición + soltar hold) |
| `TDQ_PASTE` | `C-V` / `F5` / `F11` / `Win+V` | ✅ `_BASE`×2, `_MOVE`, `_ALFA` | ❓ `F11` también sale del doble-tap de `TDQ_BOOKMARK` | ✅ Se queda; decidir el solape `F11` (§2) |
| `TDQ_MOUSE_HOLD` | toggle / captura / `MS_BTN2` / `A-PrtScn` | ✅ `_BASE`×2, `_MOVE` | ✅ `MS_BTN1/2` directos en layouts (complemento por posición, no reemplazo) | ✅ Se queda |
| `TDQ_FAST` | toggle `_FAST` / `MS_BTN2` | ❌ en ninguno (revertido a `TG`) | ✅ toggle vía `TG(_FAST)` (en uso, tu decisión); clic derecho vía `MS_BTN2` directos | ❌ Eliminar — Hero confirma (§5) |

## 2. Solape `F11` (único duplicado real entre vivos)

Doble-tap de `TDQ_PASTE` y doble-tap de `TDQ_BOOKMARK` dan `F11`. Distinto contexto (pegar vs marcadores), misma salida. Opciones: (a) dejarlo (cada flujo tiene su gesto a mano); (b) cambiar uno (p.ej. el de paste a `C-F`, libre). Decisión Hero.

## 3. Eliminación propuesta (2 muertos)

Quitar enum + action + función (+ excepción de tapping-term de `TDQ_SEL`, keymap.c:1506). Sin tocar layouts (ya no los usan). Ahorra flash y ruido. Compilar para verificar.

## 4. Teclas sueltas verificadas (se quedan)

`LT(SEL_ALL,KC_SPACE)` (cubre lo de `TDQ_SEL`), `MS_BTN1/2` directos, `LSFT_T(KC_ESC)`, `TG(_FAST)`, `TG(_MOUSE_KEY)` — todas en uso y sin redundancia tras §3.

## 5. Comentarios de Hero (2026-09-10; re-validar al implementar)

- `TDQ_SEL`: se queda. "Me parece útil, la puedo utilizar después para seleccionar diferentes elementos, asignarle una tecla."
- `TDQ_FAST`: se elimina. "Estoy de acuerdo con eliminarla, la probé en un momento y pasar de capas de esa manera es muy lento."
- Pendiente: veredicto del solape `F11` (§2) y re-validación de ambas al llevar a cabo la tarea.
