# 🎹 Guía de Tap Dances — Corne Keymap

Guía visual para **practicar y memorizar** los Tap Dances (TDQ) implementados.

> 🟦 **TDQ_ESC** · 🟩 **TDQ_PASTE** · 🟧 **TDQ_MOUSE_HOLD**

---

## 📍 Ubicación en el teclado (capa `_BASE`)

### 🖐️ Mano izquierda

| Col | 1 | 2 | 3 | 4 | 5 | 6 |
|-----|---|---|---|---|---|---|
| **Fila superior** | `F4` | 🟦 <span style="color:#3182ce">**TDQ_ESC**</span> | `ALT` | 🟧 <span style="color:#dd6b20">**TDQ_MOUSE_HOLD**</span> | `ENT` | `BOOT` |
| **Fila media** | `AI` | `NEW` | `FAST` | `BTN1` | 🟩 <span style="color:#38a169">**TDQ_PASTE**</span> | `SEL` |
| **Fila inferior** | `BTN2` | `COPY` | `SPC` | `BTN1` | `TAB` | — |
| **Pulgar** | `ENT` | `CTL` | `GUI` |  |  |  |

### 🖐️ Mano derecha

| Col | 1 | 2 | 3 | 4 | 5 | 6 |
|-----|---|---|---|---|---|---|
| **Fila superior** | `BOOT` | — | `LSFT` | `WHLD` | `WHLU` | — |
| **Fila media** | `SLEEP` | 🟩 <span style="color:#38a169">**TDQ_PASTE**</span> | `BTN1` | — | `ALFA` | `MOVE` |
| **Fila inferior** | `HIB` | `G(D)` | `BTN2` | `BTN2` | `WHLR` | `WHLL` |
| **Pulgar** | `BOOK2` | `BOOK` | `ACL0` |  |  |  |

---

## 📋 Resumen compacto

| TDQ | Tap | Hold | Doble tap | Doble tap + hold |
|-----|-----|------|-----------|------------------|
| 🟦 <span style="color:#3182ce">**TDQ_ESC**</span> | `Esc` <sub>(escape)</sub> | `Win+D` <sub>(minimizar todo)</sub> | `Clic derecho` <sub>(menú contextual)</sub> | — |
| 🟩 <span style="color:#38a169">**TDQ_PASTE**</span> | `Ctrl+V` <sub>(pegar)</sub> | `F5` <sub>(recargar página)</sub> | `F11` <sub>(pantalla completa)</sub> | `Win+V` <sub>(portapapeles)</sub> |
| 🟧 <span style="color:#dd6b20">**TDQ_MOUSE_HOLD**</span> | `Sostener clic` <sub>(clic izquierdo fijo)</sub> | `Impr Pant` <sub>(captura de pantalla)</sub> | `Ctrl+F` <sub>(buscar palabra)</sub> | `Alt+Impr Pant` <sub>(ventana activa)</sub> |

---

## 🎯 Funcionalidades

### 🟦 <span style="color:#3182ce">**TDQ_ESC**</span>

| Gesto | Acción | Keycode |
|-------|--------|---------|
| **Tap** | Escape | `KC_ESC` |
| **Hold** | Minimizar todo | `Win+D` |
| **Doble tap** | Clic derecho | `MS_BTN2` |

### 🟩 <span style="color:#38a169">**TDQ_PASTE**</span>

| Gesto | Acción | Keycode |
|-------|--------|---------|
| **Tap** | Pegar | `Ctrl+V` |
| **Hold** | Recargar página | `F5` |
| **Doble tap** | Pantalla completa | `F11` |
| **Doble tap + hold** | Portapapeles | `Win+V` |

### 🟧 <span style="color:#dd6b20">**TDQ_MOUSE_HOLD**</span>

| Gesto | Acción | Keycode |
|-------|--------|---------|
| **Tap** | Sostener clic izquierdo | `toggle_mouse_hold()` |
| **Hold** | Captura de pantalla | `Impr Pant` |
| **Doble tap** | Buscar palabra | `Ctrl+F` |
| **Doble tap + hold** | Captura de ventana activa | `Alt+Impr Pant` |
