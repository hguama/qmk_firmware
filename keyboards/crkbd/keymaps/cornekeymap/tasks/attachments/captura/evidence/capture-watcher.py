# capture-watcher.py — Task 12.1: avisa al teclado cuando el overlay de Recortes esta listo.
# Bucle: detecta la huella (fullscreen + SnipOverlayRootWindow + 'Snipping Tool Overlay')
# y manda 'S' por raw_hid (usage page 0xFF60). Sin overlay no hace nada.
# Además escucha los avisos del teclado (CAP_ARM / CAP_EVT / CAP_NET) y los registra
# con reloj en consola y en capture-log.txt, para ver qué ruta disparó cada captura.
# Uso:  python capture-watcher.py [--once] [--log archivo]

import ctypes
import datetime
import sys
import threading
import time

try:
    import hid  # pyhidapi (la misma que usa QMK)
except ImportError:
    sys.exit("falta hid: python -m pip install hid")

user32 = ctypes.windll.user32
T0 = time.perf_counter()
LOG = None


def log(msg):
    line = "[+%6.0fms %s] %s" % ((time.perf_counter() - T0) * 1000.0,
                                 datetime.datetime.now().strftime("%H:%M:%S"), msg)
    print(line, flush=True)
    if LOG is not None:
        LOG.write(line + "\n")
        LOG.flush()


def fullscreen_wins():
    sw, sh = user32.GetSystemMetrics(0), user32.GetSystemMetrics(1)
    out = []
    CMPCF = ctypes.WINFUNCTYPE(ctypes.c_bool, ctypes.c_void_p, ctypes.c_void_p)

    def cb(hwnd, _):
        if not user32.IsWindowVisible(hwnd):
            return True
        buf = (ctypes.c_long * 4)()
        user32.GetWindowRect(hwnd, buf)
        if (buf[2] - buf[0], buf[3] - buf[1]) == (sw, sh):
            ln = user32.GetWindowTextLengthW(hwnd)
            t = ctypes.create_unicode_buffer(ln + 1)
            user32.GetWindowTextW(hwnd, t, ln + 1)
            c = ctypes.create_unicode_buffer(256)
            user32.GetClassNameW(hwnd, c, 256)
            out.append((t.value, c.value))
        return True

    user32.EnumWindows(CMPCF(cb), None)
    return out


def is_overlay(wins):
    return any(t == "Snipping Tool Overlay" and c == "SnipOverlayRootWindow" for t, c in wins)


def open_keyboard():
    for d in hid.enumerate():
        if d.get("usage_page") == 0xFF60:
            h = hid.Device(path=d["path"])
            log("teclado: %s %s" % (d.get("manufacturer_string"), d.get("product_string")))
            return h
    return None


def send_s(h):
    h.write(bytes([0x00] + [ord("S")] + [0] * 31))


def kb_listener(stop):
    # Hilo aparte: lee los avisos del teclado sin bloquear la detección.
    # Se abre su propio handle porque hidapi no permite leer y escribir a la vez.
    while not stop.is_set():
        h = None
        try:
            for d in hid.enumerate():
                if d.get("usage_page") == 0xFF60:
                    h = hid.Device(path=d["path"])
                    break
            if h is None:
                time.sleep(2.0)
                continue
            h.nonblocking = 1
            while not stop.is_set():
                data = h.read(64, 200)
                if data:
                    try:
                        txt = bytes(data[:32]).split(b"\x00")[0].decode("utf-8", "replace")
                    except Exception:
                        txt = repr(data[:8])
                    if txt:
                        log("KB -> %s" % txt)
                time.sleep(0.01)
        except Exception as e:
            log("lector KB: %s (reintentando)" % e)
            time.sleep(2.0)
        finally:
            try:
                if h is not None:
                    h.close()
            except Exception:
                pass


def main():
    global LOG
    once = "--once" in sys.argv
    if "--log" in sys.argv:
        LOG = open(sys.argv[sys.argv.index("--log") + 1], "a", encoding="utf-8")
        log("log iniciado")
    kb = open_keyboard()
    if kb is None:
        sys.exit("no se encontro dispositivo raw_hid (usage 0xFF60). Revisa conexion/flasheo.")
    stop = threading.Event()
    t = threading.Thread(target=kb_listener, args=(stop,), daemon=True)
    t.start()
    log("watcher listo (Ctrl+C para salir)")
    was = False
    try:
        while True:
            now = is_overlay(fullscreen_wins())
            if now and not was:
                t0 = time.perf_counter()
                try:
                    send_s(kb)
                except (OSError, hid.HIDException):
                    log("teclado desconectado, rebuscando...")
                    kb.close()
                    kb = open_keyboard()
                    if kb is None:
                        time.sleep(2.0)
                        continue
                    send_s(kb)
                log("OVERLAY visto -> S enviado (+%.0f ms deteccion)" % ((time.perf_counter() - t0) * 1000.0))
                if once:
                    return
            was = now
            time.sleep(0.02)
    except KeyboardInterrupt:
        log("fin")
    finally:
        stop.set()
        kb.close()
        if LOG is not None:
            LOG.close()


if __name__ == "__main__":
    main()
