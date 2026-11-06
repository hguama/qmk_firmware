#!/usr/bin/env python3
"""Prueba de la tarea 23: turbo de los trackballs al sostener la tecla de _AI.

Uso:  python3 turbo-prueba.py      (Ctrl+C para salir)

Mientras el Corne está en LAYER_AI (tecla del meñique izquierdo o del derecho),
el script toma cada trackball conectado (grab) y reenvía sus movimientos
multiplicados por un dispositivo virtual. Fuera de _AI los trackballs van
directo al sistema, sin pasar por aquí: la precisión no cambia.
Si el script se cierra o se cae, el kernel los suelta solos.
"""
import sys, time, signal, threading, subprocess

import hid
from evdev import InputDevice, UInput, list_devices, ecodes as e

VID, PID = 0x4653, 0x0001          # Corne
CAPA_TURBO = "LAYER_AI"
# Factores del turbo por trackball: (horizontal, vertical).
# M575 (pulgar derecho): horizontal 4 x 1,6 (la pantalla 1920x1200 es 1,6 veces
# más ancha que alta); vertical 8, porque girar la bola hacia arriba y abajo
# cuesta más con el pulgar.
# Kensington (mano izquierda): de partida, los mismos que el M575.
TRACKBALLS = {
    "Logitech ERGO M575": (6.4, 8.0),
    "Kensington Expert Wireless TB Mouse": (6.4, 8.0),
}
# Ajustes de KWin que se copian del trackball real a su virtual, para que se sienta igual.
PROPIEDADES = [("pointerAcceleration", "d"), ("pointerAccelerationProfileAdaptive", "b"),
               ("pointerAccelerationProfileFlat", "b"), ("scrollFactor", "d"),
               ("naturalScroll", "b"), ("leftHanded", "b"), ("middleEmulation", "b")]

turbo_pedido = False   # lo que dice el Corne


# ---------------------------------------------------------------- Corne (raw HID)
def abrir_corne():
    path = next((d["path"] for d in hid.enumerate()
                 if d["vendor_id"] == VID and d["product_id"] == PID
                 and d.get("usage_page") == 0xFF60), None)
    if not path:
        sys.exit("No encuentro la interfaz raw HID del Corne.")
    if hasattr(hid, "device"):            # cython-hidapi
        dev = hid.device(); dev.open_path(path)
        return lambda: dev.read(32, 500)
    dev = hid.Device(path=path)           # pyhidapi
    return lambda: dev.read(32, 500)


def escuchar_corne(leer):
    global turbo_pedido
    while True:
        data = leer()
        if not data:
            continue
        msg = "".join(chr(b) for b in data if 31 < b < 127)
        if "LAYER_" in msg:
            turbo_pedido = CAPA_TURBO in msg
            print(f"[capa] {msg.strip()}  -> turbo {'pedido' if turbo_pedido else 'apagado'}")


# ---------------------------------------------------------------- KWin
def kwin_ruta(nombre):
    salida = subprocess.run(["busctl", "--user", "tree", "org.kde.KWin", "--list"],
                            capture_output=True, text=True).stdout
    for ruta in salida.split():
        if "/InputDevice/event" not in ruta:
            continue
        r = subprocess.run(["busctl", "--user", "get-property", "org.kde.KWin", ruta,
                            "org.kde.KWin.InputDevice", "name"], capture_output=True, text=True)
        if r.stdout.strip() == f"s \"{nombre}\"":   # nombre exacto: el virtual contiene el del real
            return ruta
    return None


def kwin_prop(ruta, prop, valor=None, tipo=None):
    base = ["busctl", "--user"]
    if valor is None:
        r = subprocess.run(base + ["get-property", "org.kde.KWin", ruta,
                                   "org.kde.KWin.InputDevice", prop], capture_output=True, text=True)
        return r.stdout.split()[1] if r.returncode == 0 else None
    subprocess.run(base + ["set-property", "org.kde.KWin", ruta,
                           "org.kde.KWin.InputDevice", prop, tipo, valor])


def copiar_ajustes_kwin(nombre_real, nombre_virtual):
    real = kwin_ruta(nombre_real)
    for _ in range(20):                    # KWin tarda un instante en ver el virtual
        virtual = kwin_ruta(nombre_virtual)
        if virtual:
            break
        time.sleep(0.1)
    if not (real and virtual):
        print(f"[kwin] {nombre_real}: no pude copiar los ajustes; el virtual queda con los de fábrica.")
        return
    for prop, tipo in PROPIEDADES:
        valor = kwin_prop(real, prop)
        if valor is not None:
            kwin_prop(virtual, prop, valor, tipo)
    print(f"[kwin] {nombre_real}: ajustes copiados al virtual ({virtual}).")


# ---------------------------------------------------------------- trackballs
def buscar_trackballs():
    encontrados = {}
    for ruta in list_devices():
        d = InputDevice(ruta)
        if d.name in TRACKBALLS and d.name not in encontrados and e.EV_REL in d.capabilities():
            encontrados[d.name] = d
    for nombre in TRACKBALLS:
        if nombre not in encontrados:
            print(f"[aviso] No encuentro '{nombre}' o no tengo permiso para leerlo; sigo sin él.")
    return encontrados


class Turbo:
    def __init__(self, tb, factores):
        self.tb = tb
        self.factores = {e.REL_X: factores[0], e.REL_Y: factores[1]}
        self.activo = False   # lo aplicado (espera a que no haya botones apretados)
        self.resto = {e.REL_X: 0.0, e.REL_Y: 0.0}
        caps = tb.capabilities()
        caps.pop(e.EV_SYN, None)
        self.nombre_virtual = f"Turbo {tb.name} (prueba)"
        self.ui = UInput({k: v for k, v in caps.items() if k in (e.EV_KEY, e.EV_REL, e.EV_MSC)},
                         name=self.nombre_virtual)

    def bucle(self):
        tb, ui = self.tb, self.ui
        try:
            while True:
                # Cambiar de modo solo sin botones apretados, para no dejar un clic pegado.
                if turbo_pedido != self.activo and not tb.active_keys():
                    tb.grab() if turbo_pedido else tb.ungrab()
                    self.activo = turbo_pedido
                    self.resto = {e.REL_X: 0.0, e.REL_Y: 0.0}
                    print(f"[turbo] {tb.name}: {'ENCENDIDO' if self.activo else 'apagado'}")

                ev = tb.read_one()
                if ev is None:
                    time.sleep(0.002)
                    continue
                if not self.activo:
                    continue                   # el sistema ya lo recibe directo
                if ev.type == e.EV_REL and ev.code in self.resto:
                    v = ev.value * self.factores[ev.code] + self.resto[ev.code]
                    n = int(v)
                    self.resto[ev.code] = v - n
                    if n:
                        ui.write(e.EV_REL, ev.code, n)
                elif ev.type == e.EV_SYN:
                    ui.syn()
                else:
                    ui.write(ev.type, ev.code, ev.value)
        except OSError:
            print(f"[aviso] {tb.name} se desconectó; sigo con los demás.")

    def cerrar(self):
        try:
            self.tb.ungrab()
        except OSError:
            pass
        self.ui.close()


def main():
    trackballs = buscar_trackballs()
    if not trackballs:
        sys.exit("No hay ningún trackball que pueda leer (¿pusiste la regla de udev?).")
    turbos = [Turbo(tb, TRACKBALLS[nombre]) for nombre, tb in trackballs.items()]
    time.sleep(0.3)
    for t in turbos:
        copiar_ajustes_kwin(t.tb.name, t.nombre_virtual)

    threading.Thread(target=escuchar_corne, args=(abrir_corne(),), daemon=True).start()
    signal.signal(signal.SIGTERM, lambda *_: sys.exit(0))
    for t in turbos:
        threading.Thread(target=t.bucle, daemon=True).start()
        fx, fy = TRACKBALLS[t.tb.name]
        print(f"Listo: {t.tb.name} (horizontal ×{fx:g}, vertical ×{fy:g}).")
    print("Sostén una tecla de _AI y mueve el trackball. Ctrl+C para salir.")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    finally:
        for t in turbos:
            t.cerrar()
        print("Trackballs liberados.")


if __name__ == "__main__":
    main()
