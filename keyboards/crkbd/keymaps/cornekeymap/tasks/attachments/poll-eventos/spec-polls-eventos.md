# Especificación: polls del script principal a eventos

## 0. Manifiesto

| id | archivo | rol | estado |
|---|---|---|---|
| C1 | `spec-polls-eventos.md` | contrato | especificación · 2026-09-09 |

## 1. Objetivo

Optimizar el sistema eliminando el desperdicio de recursos del monitoreo constante: hoy 6 hilos giran 24/7 preguntando lo mismo (teclado, botón, cursor, bordes) aunque nada cambie. Pasar a eventos —el hilo duerme y solo trabaja cuando algo pasa— con el modelo ya probado de `utils/captura`. Mismo comportamiento observable, CPU en reposo. Repo: `D:\scripts\status script`, rama `main` (limpio al especificar).

## 2. Funcionalidades (qué hacen hoy → qué debe pasar tras el cambio)

### 2.1 Lector HID (`escuchar_hid`)
Hace hoy: gira sin pausa leyendo `raw_hid` en no-bloqueante; ante reporte arma texto, actualiza bandera alt-tab (`AT_ON/OFF`), resuelve capa por subcadena y refresca la UI por `root.after`; despacha `CAP_*` al módulo captura.
Verificación: cambiar de capa 3 veces → el indicador muestra cada capa (≤600ms); Alt+Tab → bandera activa; un hold de captura → el log muestra `CAP_ARM`→`CAP_EVT`; 10 min en reposo sin `[HID ERROR]` ni CPU del hilo.

### 2.2 Reset de alt-tab (`detectar_clic_reset_alt`)
Hace hoy: cada 10ms pregunta si el botón izquierdo está presionado con alt-tab visible; al soltarlo, suelta `Alt` y manda `R` al teclado (que limpia y reporta `AT_OFF`).
Verificación: Alt+Tab → clic izquierdo en una ventana → se confirma la ventana, `Alt` queda suelto y el indicador vuelve a `BASE`; clics normales (sin alt-tab) no mandan nada; un hold de captura no dispara `R`.

### 2.3 Seguimiento del mouse (`seguimiento_mouse`)
Hace hoy: cada 10ms lee el cursor y coloca el punto indicador debajo de él.
Verificación: mover el cursor por ambas pantallas → el punto lo sigue sin saltos ni rezago visible; al detener, queda quieto bajo el cursor.

### 2.4 Ocultar indicador (`ocultar_indicador_si_mouse_cerca`)
Hace hoy: cada 30-50ms mide distancia cursor→indicadores y los oculta (`withdraw`) si se acerca, los muestra al alejarse (incluye scroll-lock).
Verificación: acercar el cursor al indicador de capa → desaparece; alejarlo → reaparece; igual con el de scroll-lock cuando está activo.

### 2.5 Wrap de bordes (`wrap_around.wrap_loop`)
Hace hoy: cada 10ms revisa si el cursor choca con un borde; tras el dwell (`DELAY_MS`) lo salta al borde opuesto con cooldown anti-rebote y lo registra en log.
Verificación: empujar a cada borde (4) → salta al opuesto tras la espera habitual, una sola vez (sin doble salto); el log marca el salto; mover normal no dispara nada.

### 2.6 Scroll-lock (`scroll_lock`, loop 10ms)
Hace hoy: acumula el movimiento en pasos y emite scroll horizontal; al desactivar resetea acumuladores (log `Desactivado`).
Verificación: con scroll-lock activo, mover el mouse → scroll horizontal por pasos; desactivar → log `Desactivado` y ningún scroll posterior; con scroll-lock inactivo, mover no scrollea.

## 3. Excluidos (no se tocan)

`utils/captura` (ya es eventos), loop del hold en firmware (acotado a 800ms, no es 24/7) y `sleep` de `onenote_nav` (delays de automatización entre teclas, no polls).

## 4. Validación general

Commitear punto de partida → convertir → `py_compile` → reiniciar script (arranque limpio, sin tracebacks) → 10 min en reposo (CPU ~0, hilos vivos) → §2 una por una → uso diario normal 1 día → si algo regresa, revert por git.
