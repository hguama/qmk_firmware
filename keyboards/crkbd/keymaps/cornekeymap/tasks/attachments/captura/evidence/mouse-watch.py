# mouse-watch.py — registra cuándo empieza/termina el movimiento del mouse,
# con el mismo formato de reloj que capture-watcher.py para correlacionar.
# Marca "inicia" (de quieto a moviendo = el usuario empieza a seleccionar),
# posiciones durante el arrastre (cada 300ms) y "quieto" (fin del arrastre).
# Uso: python mouse-watch.py --log archivo

import ctypes
import datetime
import sys
import time

user32 = ctypes.windll.user32


class POINT(ctypes.Structure):
    _fields_ = [("x", ctypes.c_long), ("y", ctypes.c_long)]


def pos():
    p = POINT()
    user32.GetCursorPos(ctypes.byref(p))
    return (p.x, p.y)


def dist(a, b):
    return abs(a[0] - b[0]) + abs(a[1] - b[1])


def main():
    logf = None
    if "--log" in sys.argv:
        logf = open(sys.argv[sys.argv.index("--log") + 1], "a", encoding="utf-8")
    T0 = time.perf_counter()

    def log(msg):
        line = "[+%6.0fms %s] %s" % ((time.perf_counter() - T0) * 1000.0,
                                     datetime.datetime.now().strftime("%H:%M:%S"), msg)
        print(line, flush=True)
        if logf is not None:
            logf.write(line + "\n")
            logf.flush()

    UMBRAL_PX = 8
    QUIETO_MS = 500
    last = pos()
    last_move = time.perf_counter()
    moving = False
    last_report = 0.0
    log("mouse-watch listo (Ctrl+C para salir)")
    try:
        while True:
            now = time.perf_counter()
            p = pos()
            if dist(p, last) > UMBRAL_PX:
                last = p
                last_move = now
                if not moving:
                    moving = True
                    log("MOUSE inicia en %s" % (p,))
                    last_report = now
                elif (now - last_report) * 1000.0 >= 300:
                    log("MOUSE arrastra por %s" % (p,))
                    last_report = now
            else:
                if moving and (now - last_move) * 1000.0 >= QUIETO_MS:
                    moving = False
                    log("MOUSE quieto en %s" % (p,))
            time.sleep(0.01)
    except KeyboardInterrupt:
        log("fin")
    finally:
        if logf is not None:
            logf.close()


if __name__ == "__main__":
    main()
