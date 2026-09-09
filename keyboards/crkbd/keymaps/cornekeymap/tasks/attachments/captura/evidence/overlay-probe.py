# overlay_probe.py — Fase A del Block captura (Task 12)
# Huella del overlay de Recortes + latencia de aparicion, con ctypes (sin dependencias).
#
#   python3 overlay_probe.py --baseline   lista ventanas visibles (no toca nada)
#   python3 overlay_probe.py --watch       espera hasta 20 s a que HERO pulse Win+Shift+S
#                                         y reporta la ventana nueva (huella + latencia aprox)
#   python3 overlay_probe.py --live        el script pulsa Win+Shift+S, mide t0->deteccion
#                                         y cierra con Esc (toma la pantalla unos segundos)

import ctypes
import sys
import time

user32 = ctypes.windll.user32

EnumWindowsProc = ctypes.WINFUNCTYPE(ctypes.c_bool, ctypes.c_void_p, ctypes.c_void_p)


def list_windows():
    wins = []

    def cb(hwnd, _):
        if not user32.IsWindowVisible(hwnd):
            return True
        length = user32.GetWindowTextLengthW(hwnd)
        title = ctypes.create_unicode_buffer(length + 1)
        user32.GetWindowTextW(hwnd, title, length + 1)
        cls = ctypes.create_unicode_buffer(256)
        user32.GetClassNameW(hwnd, cls, 256)
        rect = ctypes.wintypes.RECT() if hasattr(ctypes, "wintypes") else None
        wins.append((hwnd, title.value, cls.value))
        return True

    user32.EnumWindows(EnumWindowsProc(cb), None)
    return wins


def show_fullscreen_candidates(wins):
    sw, sh = user32.GetSystemMetrics(0), user32.GetSystemMetrics(1)
    print("pantalla: %dx%d" % (sw, sh))
    print("ventanas visibles: %d" % len(wins))
    for hwnd, title, cls in wins:
        r = ctypes.create_string_buffer(16)
        # GetWindowRect via struct of 4 LONGs
        buf = (ctypes.c_long * 4)()
        user32.GetWindowRect(hwnd, buf)
        l, t, rr, b = buf[0], buf[1], buf[2], buf[3]
        if (rr - l, b - t) == (sw, sh):
            print("  FULLSCREEN hwnd=%s clase=%r titulo=%r" % (hwnd, cls, title))


def cmd_baseline():
    wins = list_windows()
    show_fullscreen_candidates(wins)


def snapshot():
    return set(h for h, _, _ in list_windows())


def detail(hwnd):
    length = user32.GetWindowTextLengthW(hwnd)
    title = ctypes.create_unicode_buffer(length + 1)
    user32.GetWindowTextW(hwnd, title, length + 1)
    cls = ctypes.create_unicode_buffer(256)
    user32.GetClassNameW(hwnd, cls, 256)
    buf = (ctypes.c_long * 4)()
    user32.GetWindowRect(hwnd, buf)
    return title.value, cls.value, tuple(buf)


def wait_new_window(before, timeout_s, poll_s=0.02):
    t0 = time.perf_counter()
    while time.perf_counter() - t0 < timeout_s:
        now = snapshot()
        new = now - before
        if new:
            hwnd = next(iter(new))
            return hwnd, (time.perf_counter() - t0) * 1000.0
        time.sleep(poll_s)
    return None, None


def proc_name(hwnd):
    try:
        pid = ctypes.c_ulong()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        ps = ctypes.windll.kernel32
        h = ps.OpenProcess(0x1000, False, pid.value)
        if not h:
            return "?"
        buf = ctypes.create_unicode_buffer(512)
        size = ctypes.c_ulong(512)
        ok = ps.QueryFullProcessImageNameW(h, 0, buf, ctypes.byref(size))
        ps.CloseHandle(h)
        if not ok:
            return "?"
        return buf.value.split("\\")[-1]
    except Exception:
        return "?"


def is_fullscreen(hwnd, sw, sh, cls):
    buf = (ctypes.c_long * 4)()
    user32.GetWindowRect(hwnd, buf)
    l, t, rr, b = buf[0], buf[1], buf[2], buf[3]
    return (rr - l, b - t) == (sw, sh)


def press_esc():
    user32.keybd_event(0x1B, 0, 0, 0)
    time.sleep(0.05)
    user32.keybd_event(0x1B, 0, 2, 0)


def send_win_shift_s():
    VK_LWIN, VK_SHIFT, VK_S = 0x5B, 0x10, 0x53
    for vk in (VK_LWIN, VK_SHIFT, VK_S):
        user32.keybd_event(vk, 0, 0, 0)
        time.sleep(0.03)
    for vk in (VK_S, VK_SHIFT, VK_LWIN):
        user32.keybd_event(vk, 0, 2, 0)
        time.sleep(0.03)


def cmd_watch():
    print("Pulsa Win+Shift+S en los proximos 20 s...")
    before = snapshot()
    hwnd, ms = wait_new_window(before, 20.0)
    if hwnd is None:
        print("sin overlay nuevo en 20 s")
        return
    title, cls, rect = detail(hwnd)
    print("overlay detectado en ~%.0f ms (desde inicio de espera)" % ms)
    print("  clase=%r titulo=%r rect=%s" % (cls, title, rect))


def cmd_live():
    sw, sh = user32.GetSystemMetrics(0), user32.GetSystemMetrics(1)
    before = snapshot()
    send_win_shift_s()
    t0 = time.perf_counter()
    seen = {}
    while time.perf_counter() - t0 < 3.0:
        for hwnd in snapshot() - before:
            if hwnd not in seen and user32.IsWindowVisible(hwnd):
                title, cls, rect = detail(hwnd)
                if rect[2] - rect[0] == sw and rect[3] - rect[1] == sh:
                    seen[hwnd] = (time.perf_counter() - t0) * 1000.0
        time.sleep(0.02)
    if not seen:
        print("ninguna ventana nueva de pantalla completa en 3 s")
        return
    for hwnd, ms in sorted(seen.items(), key=lambda kv: kv[1]):
        title, cls, rect = detail(hwnd)
        print("nueva fullscreen a %.0f ms: clase=%r titulo=%r proc=%s" % (ms, cls, title, proc_name(hwnd)))
    time.sleep(0.5)
    press_esc()
    print("Esc enviado")


if __name__ == "__main__":
    mode = sys.argv[1] if len(sys.argv) > 1 else "--baseline"
    if mode == "--baseline":
        cmd_baseline()
    elif mode == "--watch":
        cmd_watch()
    elif mode == "--live":
        cmd_live()
    else:
        print("uso: overlay_probe.py [--baseline|--watch|--live]")
