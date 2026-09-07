/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>



This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
JKL
*/
#include QMK_KEYBOARD_H
//#include "rgblight.h"
#include "raw_hid.h"
// Conexión directa con las variables reales del motor de mouse de QMK
#ifdef MOUSEKEY_ENABLE
extern uint8_t mk_delay;
extern uint8_t mk_max_speed;
extern uint8_t mk_time_to_max;
extern uint8_t mk_interval;
#endif


//Layer names enum
enum layer_names {
    _BASE    = 0,
    _MOVE    = 1,
    _ALFA    = 2,
    _AI      = 3,
    _DEL     = 4,
    _SYMB    = 5,
    _NUMB    = 6,
    _BOOK    = 7,
    _BOOK_2  = 8,
    _FAST    = 9,
    _MOVE_WIN = 10,
    _RUN     = 11,
    _MOUSE_KEY = 12,
};


//Macro enum
// ============================================================================
// TECLAS PERSONALIZADAS (custom keycodes), organizadas por categoría.
// El orden no afecta a QMK: cada nombre recibe un valor único automáticamente.
// ============================================================================
enum custom_keycodes {
    // ---- LETRAS ESPECIALES --------------------------------------------------
    P_ENIE = SAFE_RANGE,  // TAP: P | HOLD: ñ (Ñ con Shift) — capa _ALFA

    // ---- PUNTUACIÓN Y OPERADORES (símbolos que escribe) ----------------------
    AMP_DOUBLE,           // &&  (doble ampersand) — capa _SYMB
    DOUBLE_PIPE,          // ||  (doble pipe) — capa _SYMB
    PIPE_M,               // |>  (pipe + mayor que) — capa _SYMB
    QUESTION,             // TAP: ? | HOLD: ¿ (interrogación invertida) — capa _SYMB
    NOT_EQUAL,            // TAP: != | HOLD: ¡ (exclamación invertida) — capa _SYMB
    EQUAL_DBL,            // TAP: = | HOLD: == — capas _SYMB/_NUMB
    DOUBLE_COLON,         // TAP: : | HOLD: :: — capa _SYMB
    LBRC2,                // TAP: [ | HOLD: ] — capa _SYMB
    LLAMBDA,              // TAP: -> | HOLD: <- — capa _SYMB
    ASTRISK_PLUS,         // TAP: * | HOLD: + — capas _SYMB/_NUMB

    // ---- EDICIÓN DE TEXTO (copiar, cortar, borrar, seleccionar, deshacer) ----
    COPY,                 // LT(CUT,COPY): TAP = Ctrl+C (copiar) — capas _BASE/_MOVE
    CUT,                  // LT(CUT,COPY): HOLD = Ctrl+X (cortar) — capas _BASE/_MOVE
    SEL_ALL = SAFE_RANGE + 14, // LT(SEL_ALL,KC_SPACE): HOLD = Ctrl+A | TAP = espacio | doble+HOLD = repetir espacio
                                   // VALOR FIJO: se usa como layer en LT(); no renumerar (colisiona con MS_ACL0)
    DEL_WORD,             // Borra la palabra completa (Ctrl+←, Ctrl+Shift+→, Supr) — capa _DEL
    DEL_LINE,             // Borra la línea completa (Inicio, Shift+Fin, Supr) — capa _DEL
    UNDO_WIN,             // TAP: Ctrl+Z (deshacer) | HOLD: tecla Windows — capas _BASE/_MOVE/_ALFA

    // ---- MOUSE Y SCROLL ------------------------------------------------------
    CTL_CLICK,            // Ctrl + clic izquierdo (abrir enlace en pestaña nueva) — capa _BASE
    MS_ACL0_TOGGLE,       // Toggle de aceleración del mouse (MS_ACL0) — pruebas — TODO: Esta tecla la necesito para unas pruebas.
    DOWN_10,              // Rueda hacia abajo (8 ticks) — capa _FAST
    UP_10,                // Rueda hacia arriba (8 ticks) — capa _FAST

    // ---- VENTANAS Y SISTEMA ---------------------------------------------------
    ALT_TAB,              // Alt+Tab (cambiar de ventana) — capas _BASE/_MOVE
    CLOSE_WIN,            // LT(KC_F4, CLOSE_WIN): TAP = Ctrl+W (cerrar pestaña) | HOLD = Alt+F4 (cerrar ventana)
    SLEEP,                // Suspender equipo (Win+X → U → S) — capas _BASE/_MOVE
    HIBERNATE,            // Hibernar equipo (Win+X → U → H) — capas _BASE/_MOVE
    CS_F15_HOLD,          // Mantiene Ctrl+Shift+F15 (remapeo externo, ej. PowerToys) — capa _FAST

    // ---- CAPAS Y MODIFICADORES --------------------------------------------------
    TG_ALFA,              // Alterna entre la capa de ratón (_MOVE) y la alfabética (_ALFA)
    MOVE_WIN_TG,          // TAP: toggle _MOVE_WIN | HOLD: capa _AI (guarda y restaura al soltar) — capa _BASE

    // ---- IDE / IA (JetBrains y asistentes de código) -----------------------------
    COMM,                 // TAP: Ctrl+/ (comentar línea) | HOLD: Ctrl+Shift+/ (comentar bloque) — TODO: sin uso en capas; preservar por posible uso en otras apps Se queda.

    // ---- MARCADORES / BOOKMARKS ---------------------------------------------------
    MARKER_B,             // LT(_BOOK_2, MARKER_B): TAP = Ctrl+Shift+F21 | HOLD = capa _BOOK_2 — capa _BOOK
    MARKER_2,             // LT(_BOOK_2, MARKER_2): TAP = Ctrl+2 | HOLD = capa _BOOK_2 — capa _BOOK
};


//Tap Dance enum
enum {
    TDQ_SEL,
    TDQ_BOOKMARK,
    TDQ_ESC,
    TDQ_PASTE,
    TDQ_MOUSE_HOLD,
};


//Quad enum
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP,
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

typedef struct {//for quad
    bool is_press_action;
    td_state_t state;
} td_tap_t;

//Vars

// static uint16_t timer_key;
static bool space_repeat_active = false;


// Helper: guardar la capa actual, mover a otra y restaurar la guardada al soltar.
static uint8_t saved_layer = _BASE;
static bool has_saved_layer = false;

static void save_current_layer(void) {
    saved_layer = biton32(layer_state);
    has_saved_layer = true;
}

static void restore_saved_layer(void) {
    if (has_saved_layer) {
        layer_move(saved_layer);
        has_saved_layer = false;
    }
}

bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;
static bool alt_tab_pressed = false;
static bool alt_tab_hold_done = false;


bool ms_acl0_active = false;

bool voice_mode = false;
static bool mouse_held = false;
bool cs_f15_held = false; // Nueva bandera para Ctrl + Shift + F15



// Prototypes quad
void send_layer_status(const char* msg);
void send_layer_status_with_at(const char* msg, layer_state_t state);


td_state_t cur_dance(tap_dance_state_t *state);
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);

void tdq_sel_finished(tap_dance_state_t *state, void *user_data);
void tdq_bookmark_finished(tap_dance_state_t *state, void *user_data);
void tdq_esc_finished(tap_dance_state_t *state, void *user_data);
void tdq_paste_finished(tap_dance_state_t *state, void *user_data);
void tdq_mouse_hold_finished(tap_dance_state_t *state, void *user_data);


// Alternate repeat key: Alt + Repeat invierte la acción
uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {

    // Ctrl+Tab → Ctrl+Shift+Tab (navegar pestañas en reversa)
    if ((mods & MOD_MASK_CTRL)) {
        switch (keycode) {
            case KC_TAB: return C(S(KC_TAB));
        }
    }

    // Ctrl+Z ↔ Ctrl+Y (undo/redo reversibles)
    switch (keycode) {
        case C(KC_Z): return C(KC_Y);
        case C(KC_Y): return C(KC_Z);
    }

    return KC_TRNS;
}



//general functions
void clear_all(void) {
    clear_keyboard();         // Libera cualquier tecla registrada
//    clear_mods();             // Libera Ctrl, Shift, Alt, etc.

    if (is_alt_tab_active)   {
       unregister_code(KC_LALT);
       unregister_code(KC_TAB);
       is_alt_tab_active = false;
         send_layer_status_with_at("AT_OFF",layer_state);
       }


    if (voice_mode) {
       tap_code16(G(KC_SPC));
       voice_mode = false;
       }

}

void send_layer_status(const char* msg) {
       uint8_t buffer[32] = {0};
       const char* message = msg;
       strncpy((char*)buffer, message, sizeof(buffer) - 1);
       raw_hid_send(buffer, sizeof(buffer));
  }

void send_layer_status_with_at(const char* at_msg, layer_state_t state) {
    char buffer[40];
    // IMPORTANTE: Ahora usamos 'state' (el valor real actual), no la variable global
    uint8_t layer = get_highest_layer(state);
    const char* layer_name;

    switch (layer) {
        case _ALFA:    layer_name = "LAYER_ALFA"; break;
        case _NUMB:    layer_name = "LAYER_NUMB"; break;
        case _MOVE_WIN:  layer_name = "LAYER_MOVEWIN"; break;
        case _AI: layer_name = "LAYER_AI"; break;
        case _FAST: layer_name = "LAYER_FAST"; break;
        case _MOVE: layer_name = "LAYER_MOVE"; break;
        case _MOUSE_KEY: layer_name = "LAYER_MOUSE_KEY"; break;
        default:       layer_name = "LAYER_BASE"; break;
    }

    snprintf(buffer, sizeof(buffer), "%s %s", at_msg, layer_name);
    send_layer_status(buffer);
}
// ============================================================================
// PROCESS RECORD USER FUNCTION
// ============================================================================

// Helper: alterna el clic izquierdo sostenido (usado por TD(TDQ_MOUSE_HOLD) y MS_BTN1)
static void toggle_mouse_hold(void) {
    mouse_held = !mouse_held;
    if (mouse_held) {
        register_code(MS_BTN1);
    } else {
        unregister_code(MS_BTN1);
        tap_code(MS_BTN1);
    }
}

// Helper: envía "¿" (Alt+0191)
static void send_inverted_question_mark(void) {
    SEND_STRING(
        SS_DOWN(X_LALT)
        SS_TAP(X_KP_0)
        SS_TAP(X_KP_1)
        SS_TAP(X_KP_9)
        SS_TAP(X_KP_1)
        SS_UP(X_LALT)
    );
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {

        case CS_F15_HOLD:
            if (record->event.pressed) {
                cs_f15_held = !cs_f15_held; // Cambiamos el estado de la bandera

                if (cs_f15_held) {
                    // Activa los modificadores y la tecla
                    register_code(KC_LCTL);
                    register_code(KC_LSFT);
                    register_code(KC_F15);
                    register_code(KC_F15);

                } else {
                    // Desactiva la tecla y los modificadores
                    unregister_code(KC_F15);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LCTL);
                }
            }
            return false; // Evita que QMK procese la tecla de forma predeterminada

        case MS_BTN1:
            if (record->event.pressed) {
                if (mouse_held) {
                    // Si el clic estaba enganchado (clic sostenido), lo soltamos inmediatamente
                    unregister_code(MS_BTN1);
                    tap_code(MS_BTN1);
                    mouse_held = false;
                    return false; // Evitamos que envíe un nuevo 'pressed' adicional
                }
            }
            return true; // Si no estaba engan

        case CTL_CLICK:
            if (record->event.pressed) {
                register_code(KC_LCTL);
                wait_ms(10);
                tap_code(MS_BTN1);
                wait_ms(10);
                unregister_code(KC_LCTL);
            }
            return false;

            // Para pruebas con teclado
        case MS_ACL0_TOGGLE:
            if (record->event.pressed) {
                ms_acl0_active = !ms_acl0_active; // Cambia el estado
                if (ms_acl0_active) {
                    register_code(MS_ACL0);   // Activa y mantiene
                } else {
                    unregister_code(MS_ACL0); // Desactiva
                }
            }
            return false; // Ya manejamos la tecla

        case LT(_SYMB, KC_ESC):
            if (record->event.pressed) {
                if (record->tap.count > 0) {
                    // TAP → ESC personalizado
                    clear_all();
                    tap_code(KC_ESC);
                    return false;  // detenemos aquí, no dejamos pasar al LT original
                }
            }
            // HOLD → dejamos que QMK maneje el LT como momentáneo
            return true;



        case LT(0,EQUAL_DBL):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(KC_EQUAL);  // HOLD: ==
                    tap_code16(KC_EQUAL);
                    return false;
                } else {
                    tap_code16(KC_EQUAL);  // TAP: =
                    return false;
                }
            }
            return false;

        case DOUBLE_PIPE:
            if (record->event.pressed) {
                tap_code16(KC_PIPE);
                tap_code16(KC_PIPE);
            }
            return false; // evita que se procese por defecto


        case LT(0, ASTRISK_PLUS):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: +
                    tap_code16(KC_PLUS);
                    return false;
                } else {
                    // TAP: *
                    tap_code16(KC_ASTR);
                    return false;
                }
            }
            return false;

        case LT(_BOOK_2, MARKER_B):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: MO(_BOOK_2)
                    return true; // Permite que QMK maneje el layer tap
                } else {
                    // TAP: C(KC_F21)
                    tap_code16(C(S(KC_F21)));
                    return false;
                }
            }
            return true;

        case LT(_BOOK_2, MARKER_2):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: MO(_BOOK_2)
                    return true; // Permite que QMK maneje el layer tap
                } else {
                    // TAP: C(KC_2)
                    tap_code16(C(KC_2));
                    return false;
                }
            }
            return true;

        case LT(0, QUESTION):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: ¿
                    send_inverted_question_mark();
                    return false;
                } else {
                    // TAP: ?
                    tap_code16(KC_QUES);
                    return false;
                }
            }
            return false;





        case DOWN_10:
            if (record->event.pressed) {
                // KC_DOWN 10 veces
                for (int i = 0; i < 8; i++) {
                    tap_code16(MS_WHLD);
                }
            }
            return false;

        case UP_10:
            if (record->event.pressed) {
                // KC_UP 10 veces
                for (int i = 0; i < 8; i++) {
                    tap_code16(MS_WHLU);
                }
            }
            return false;

        case PIPE_M:
            if (record->event.pressed) {
                // |>
                tap_code16(KC_PIPE);
                tap_code16(KC_GT);
            }
            return false;

        case LT(0,DOUBLE_COLON):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: "::" inmediatamente
                    tap_code16(KC_COLN);
                    tap_code16(KC_COLN);
                    return false;
                } else {
                    // TAP: un solo ":"
                    tap_code16(KC_COLN);
                    return false;
                }
            }
            return false;


    	case ALT_TAB:
            if (record->event.pressed) {
                alt_tab_pressed = true;
                alt_tab_hold_done = false;
                alt_tab_timer = timer_read();
                 send_layer_status_with_at("AT_ON",layer_state);
            } else {
                alt_tab_pressed = false;

                if (!alt_tab_hold_done) {
                    // TAP: comportamiento actual (Alt sostenido + Tab)
                    if (!is_alt_tab_active) {
                        is_alt_tab_active = true;
                        register_code(KC_LALT);
                         send_layer_status_with_at("AT_ON",layer_state);
                    }
                    register_code(KC_TAB);
                    unregister_code(KC_TAB);
                    // Alt queda activo como hasta ahora
                }
            }
            return false;


        case SLEEP:
            if (record->event.pressed) {
                tap_code16_delay(LGUI(KC_X),120);
                tap_code16_delay(KC_U,120);
                wait_ms(300);
                tap_code(KC_S);
            }
            break;

        case HIBERNATE:
            if (record->event.pressed) {
                tap_code16_delay(LGUI(KC_X), 420);
                tap_code16_delay(KC_U,320);
                tap_code(KC_H);
            }
            break;

        case DEL_WORD:
            if (record->event.pressed) {
                tap_code16_delay(C(KC_LEFT), 10);
                tap_code16_delay(C(S(KC_RIGHT)), 10);
                tap_code(KC_BSPC);
            }
            break;


        case DEL_LINE:
            if (record->event.pressed) {
                tap_code_delay(KC_HOME, 10);       // Ir al inicio
                tap_code16_delay(S(KC_END), 10);   // Shift + End (seleccionar hasta el final)
                tap_code_delay(KC_BSPC, 10);       // Borrar selección
            }
            break;

        case LT(0,KC_SLSH):
            if (!record->tap.count && record->event.pressed) {
                tap_code16(KC_BSLS); // hold
                return false;
            }
            return true;

        case LT(0,KC_DQT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: '
                    tap_code16(KC_QUOT);
                    return false;
                } else {
                    // TAP: "
                    tap_code16(S(KC_QUOT));
                    return false;
                }
            }
            return true;

        case LT(0, NOT_EQUAL):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: ¡
                    SEND_STRING(
                        SS_DOWN(X_LALT)
                        SS_TAP(X_KP_0)
                        SS_TAP(X_KP_1)
                        SS_TAP(X_KP_6)
                        SS_TAP(X_KP_1)
                        SS_UP(X_LALT)
                    );
                    return false;
                } else {
                    // TAP: !=
                    tap_code16(S(KC_1));  // Envía "!"
                    tap_code(KC_EQL);     // Envía "="
                    return false;
                }
            }
            return false;


        case LT(0,KC_LABK):
            if (record->event.pressed) {
                if (!record->tap.count) { //hold
                  //  SEND_STRING("<=");
                    tap_code16(KC_LABK);
                    tap_code16(KC_EQL);
                    return false;
                } else {
                    tap_code16(KC_LABK); //tap <
                    return false;
                }
            }
            return true;

        case LT(0,KC_RABK):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    SEND_STRING(">=");
                    return false;
                } else {
                    tap_code16(KC_GT);
                    return false;
                }
            }
            return true;

        case LT(0,KC_MINS):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_MINS);
                    return false;
                } else {
                    tap_code16(S(KC_MINS));
                    return false;
                }
            }
            return true;

        case AMP_DOUBLE:
            if (record->event.pressed) {
                tap_code16(KC_AMPR); // &
                tap_code16(KC_AMPR); // &
            }
            return false; // no procesar más

        case LT(0,LLAMBDA):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: <-
                    SEND_STRING("<-");
                    return false;
                } else {
                    // TAP: ->
                    SEND_STRING("->");
                    return false;
                }
            }
            return false;

        case LT(0,KC_LPRN):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(S(KC_0));   // Envía ")"
                    return false;
                } else {
                    tap_code16(S(KC_9));   // Envía "("
                    return false;
                }
            }
            break;

        case LT(0,KC_LCBR):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(S(KC_RBRC));   // Envía "}"
                    return false;
                } else {
                    tap_code16(S(KC_LBRC));   // Envía "{"
                    return false;
                }
            }
            break;

        case LT(0,LBRC2):
            if (record->event.pressed) {
                if (!record->tap.count) { //hold
                    tap_code(KC_RBRC);   // Envía "]"
                    return false;
                } else { //tap
                    tap_code(KC_LBRC);   // Envía "["
                    return false;
                }
            }
            return false;

        case LT(_AI,KC_ENT):
            if (record->event.pressed) {
                clear_all();
            }
            return true;



        // Evaluar si se necesita (JetBrains)
        case LT(0,COMM):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(LCTL(LSFT(KC_SLSH)));
                    return false;
                } else {
                    tap_code16(LCTL(KC_SLSH));
                    return false;
                }
            }
            return false;

        case LT(0, UNDO_WIN):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: tecla Windows (abre menú de Windows)
                    tap_code(KC_LGUI);

                    return false;
                } else {
                    // TAP: Deshacer (Ctrl+Z)
                    clear_all();
                    tap_code16(C(KC_Z));
                    return false;
                }
            }
            return false;



            // layer_invert(_MOVE); // tap - toggle _MOVE layer
         case LT(_AI, MOVE_WIN_TG):
             if (record->event.pressed) {
                if (record->tap.count == 0) {
                    // HOLD: guardar la capa actual y mover a _AI
                    save_current_layer();
                    layer_move(_AI);
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP: alternar la capa _MOVE_WIN
                    layer_invert(_MOVE_WIN);
                    return false;
                }
            } else {
                // RELEASE: restaurar la capa guardada
                restore_saved_layer();
            }
            return true;

        case TG_ALFA:
            if (record->event.pressed) {
                if (IS_LAYER_ON(_MOVE)) {
                    // Si estoy en Mouse, la apago y enciendo Alfa
                    layer_off(_MOVE);
                    layer_on(_ALFA);
                }
                else if (IS_LAYER_ON(_ALFA)) {
                    // Si ya estoy en Alfa, la apago y regreso a Mouse
                    layer_off(_ALFA);
                    layer_on(_MOVE);
                }
                else {
                    // Si por alguna razón estoy en Base, voy a Mouse (o lo que prefieras)
                    layer_on(_MOVE);
                }
            }
            return false;


            // Para pruebas con teclado
        case LT(MS_ACL2, KC_ENT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Activar MS_ACL2
                    register_code(MS_ACL2);
                    return false;
                } else {
                    // TAP
                   tap_code(KC_ENT);
                    return false;
                }
            } else {
                // Al soltar la tecla, desactivar MS_ACL2
                unregister_code(MS_ACL2);
            }
            return false;


        case KC_ESC:
            if (record->event.pressed) {
                clear_all();
                tap_code(KC_ESC);
            }
            break;

        case TO(_BASE):
            clear_all();
            break;

        case LT(_DEL, KC_PERC):
            if (record->event.pressed) {
                if (record->tap.count == 0) {
                    // HOLD: guardar la capa actual y mover a _DEL
                    save_current_layer();
                    layer_move(_DEL);
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple: enviar %
                    tap_code16(KC_PERC);
                    return false;
                }
            } else {
                // RELEASE: restaurar la capa guardada
                restore_saved_layer();
            }
            return true;

        case LT(0, P_ENIE):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    uint8_t mods = get_mods();   // guardar mods actuales
                    clear_mods();

                    if (mods & MOD_MASK_SHIFT) {
                        // Ñ (Alt+0209)
                        SEND_STRING(
                            SS_DOWN(X_LALT)
                            SS_TAP(X_KP_0)
                            SS_TAP(X_KP_2)
                            SS_TAP(X_KP_0)
                            SS_TAP(X_KP_9)
                            SS_UP(X_LALT)
                        );
                    } else {
                        // ñ (Alt+0241)
                        SEND_STRING(
                            SS_DOWN(X_LALT)
                            SS_TAP(X_KP_0)
                            SS_TAP(X_KP_2)
                            SS_TAP(X_KP_4)
                            SS_TAP(X_KP_1)
                            SS_UP(X_LALT)
                        );
                    }

                    set_mods(mods); // restaurar mods originales
                    return false;
                } else {
                    // TAP: manda la P normal
                    tap_code(KC_P);
                    return false;
                }
            }
            return false;



        case LT(SEL_ALL, KC_SPACE):
            if (record->event.pressed) {
                // 1. LÓGICA DE HOLD (Seleccionar Todo)
                if (record->tap.count == 0) {
                    // Limpiamos cualquier estado previo para evitar conflictos
                   // clear_keyboard();
                   // _delay_ms(15); // Aumentamos ligeramente a 15ms para estabilidad

                    // Enviamos Ctrl+A de forma atómica y explícita
                   // register_code(KC_LCTL);
                   // _delay_ms(5); // Micro-pausa entre Ctrl y A
                    //tap_code(KC_A);
                    //_delay_ms(5);
                   // unregister_code(KC_LCTL);
                    tap_code16(C(KC_A));

                    return false;
                }

                // 2. DOBLE TAP + HOLD (Repetir espacios)
                if (record->tap.count >= 2) {
                    register_code(KC_SPACE);
                    space_repeat_active = true;
                    return false;
                }
            } else {
                // 3. AL SOLTAR LA TECLA
                if (space_repeat_active) {
                    unregister_code(KC_SPACE);
                    space_repeat_active = false;
                } else if (record->tap.count == 1) {
                    // Si fue un toque rápido (TAP), enviamos el espacio al soltar
                    tap_code(KC_SPACE);
                }
            }
            return false;

        case LT(CUT,COPY):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Pegar (Ctrl+V)
                    tap_code16(C(KC_X));
                    return false;
                } else {
                    // TAP: Copiar (Ctrl+C)
                    tap_code16(C(KC_C));
                    return false;
                }
            }
            return false;

        case LT(KC_F4, CLOSE_WIN):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD:  cerrar pestaña

                    tap_code16(A(KC_F4));
                    return false;
                } else {
                    // TAP: cerrar ventana
                    tap_code16(C(KC_W));
                    return false;
                }
            }
            return false;


        case LT(MS_ACL0, KC_SPACE):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Activar MS_ACL0, f22
                    register_code(MS_ACL0);
                    return false;
                } else {
                    // TAP: Espacio
                    tap_code(KC_SPACE);
                    return false;
                }
            } else {
                // Al soltar la tecla, desactivar MS_ACL0
                unregister_code(MS_ACL0);
            }
            return false;

        }//END SWITCH
    return true;
};
//Pr record END


//timer for  macro
void matrix_scan_user(void) {


	//para alt_tab
    if (alt_tab_pressed && !alt_tab_hold_done) {
        if (timer_elapsed(alt_tab_timer) > 150) {
            // HOLD: Alt+Tab corto inmediato
            tap_code16(A(KC_TAB));
            alt_tab_hold_done = true;
        }
    }


	if (is_alt_tab_active && timer_elapsed(alt_tab_timer) > 20000)   {
           unregister_code(KC_LALT);
           unregister_code(KC_TAB);
           is_alt_tab_active = false;
           send_layer_status_with_at("AT_OFF",layer_state);

   }

 }


// ============================================================================
// TAP DANCE ACTIONS
// ============================================================================

tap_dance_action_t tap_dance_actions[] = {
    [TDQ_SEL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_sel_finished, x_reset),
    [TDQ_BOOKMARK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_bookmark_finished, x_reset),
    [TDQ_ESC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_esc_finished, x_reset),
    [TDQ_PASTE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_paste_finished, x_reset),
    [TDQ_MOUSE_HOLD] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_mouse_hold_finished, x_reset),
};

// ============================================================================
// KEYMAPS  LT(_AI,_MOVE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//TDQ
// LT(KC_F4, CLOSE_WIN)
// LT(_SYMB,KC_ESC)
/// LT(_NUMB,KC_TAB)
// LT(_NUMB,KC_TAB) *
// LT(_MIRROR, _ALFA) *
// PASTE
// LT(_DEL,KC_LGUI)
// LT(CUT,COPY)
// KC_F24
// LT(_MOVE_WIN, KC_ENT **
// LT(0,CTL_GUI) **
//TD(TDQ_SEL) sel word and line lo puedo hacer con mouse. Mejorar las fx.
//
//LT(SEL_ALL,KC_SPACE)
// KC_LSFT
// MS_WHLD, MS_WHLU, MS_WHLL MS_WHLR KC_PGDN, KC_PGUP**
// LT(_RUN,MS_BTN2)
//
//
//


//
//

//tdq
//
//LT(KC_F4, CLOSE_WIN),
// LT(_SYMB,KC_ESC)
//
//
//
//
//
//G(KC_D)
//LT(_DEL,KC_LGUI)
//TD(TDQ_SEL)



//LT(_AI, MOVE_WIN_TG)TDQ_ESC  TDQ_ESC LT(0,CTL_GUI)


// _BASE Ly 0
[_BASE] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                                        ,-----------------------------------------------------.
LT(KC_F4, CLOSE_WIN),  TD(TDQ_ESC), ALT_TAB, TD(TDQ_MOUSE_HOLD), LT(_AI, KC_ENT), QK_BOOT,                 QK_BOOT, XXXXXXX, TD(TDQ_MOUSE_HOLD), ALT_TAB,  TD(TDQ_ESC), LT(KC_F4, CLOSE_WIN),
// |--------+--------+--------+--------+--------+--------|                                                |--------+--------+--------+--------+--------+--------|
LT(_AI, MOVE_WIN_TG), TG(_MOUSE_KEY), TG(_FAST), MS_BTN1, TD(TDQ_PASTE), LT(SEL_ALL,KC_SPACE),             SLEEP, TD(TDQ_PASTE), MS_BTN1, TG(_FAST), TG(_ALFA), TG(_MOVE),
// |--------+--------+--------+--------+--------+--------|                                                |--------+--------+--------+--------+--------+--------|
MS_BTN2, LT(CUT,COPY), C(KC_SPACE), MS_BTN1, KC_TAB, CTL_CLICK,                                            HIBERNATE, KC_ENT, MS_BTN2, C(KC_SPACE) ,LT(CUT,COPY), LT(_AI, MOVE_WIN_TG),
// |--------+--------+--------+--------+--------+--------|                                                |--------+--------+--------+--------+--------+--------+--------|
                                 LT(_AI, KC_ENT), LT(0,UNDO_WIN), C(KC_Y),                                 MO(_BOOK_2), LT(_BOOK, KC_ENT), KC_SPACE
                                // `---------------------'                                                  `--------------------------'
),

    // _MOVE Ly 1
    [_MOVE] = LAYOUT_split_3x6_3(
// ,-------------------------------------------------------------------------------------.                          ,-----------------------------------------------------.
LT(KC_F4, CLOSE_WIN), LSFT_T(KC_ESC), ALT_TAB, LT(_NUMB,KC_TAB), TD(TDQ_MOUSE_HOLD), QK_BOOT,                              QK_BOOT, XXXXXXX, XXXXXXX, KC_HOME, LSFT_T(KC_END), XXXXXXX,
// |--------+--------+--------+--------+--------+----------------------------------------|                          |--------+--------+--------+--------+--------+--------|
_______, MO(_DEL), KC_DOWN, KC_UP, TD(TDQ_PASTE), LT(SEL_ALL,KC_SPACE),                                               SLEEP, XXXXXXX, KC_LEFT, KC_RIGHT, XXXXXXX, TG(_MOVE),
// |--------+--------+--------+--------+--------+----------------------------------------|                          |--------+--------+--------+--------+--------+--------|
TG_ALFA, LT(CUT,COPY), KC_F24, MS_BTN1, KC_TAB, G(KC_D),                                                            HIBERNATE, XXXXXXX, _______, XXXXXXX, XXXXXXX, KC_INS,
 //|--------+--------+--------+--------+--------+--------+-------------------------------|                          |--------+--------+--------+--------+--------+--------+--------|
                                                       LT(_MOVE_WIN, KC_ENT), C(KC_Z), LT(0,UNDO_WIN),                TO(_BASE), CTL_T(KC_ENT), KC_SPACE
                                                         // `----------------------------------'                       `---------------------------------'
    ),


    // _ALFA Ly 2
    [_ALFA] = LAYOUT_split_3x6_3(
// ,--------------------------------------------------------.                                         ,-----------------------------------------------------.
  KC_W, TD(TDQ_ESC), KC_T, KC_F, KC_Z, QK_BOOT,                                                        QK_BOOT, KC_X, KC_H, KC_D, KC_L, KC_K,
// |--------+--------+--------+--------+--------+-----------|                                         |--------+--------+--------+--------+--------+--------|
LT(_AI,KC_A), LT(_DEL,KC_R), LT(_SYMB,KC_E), LT(_NUMB,KC_I), TD(TDQ_PASTE), LT(SEL_ALL,KC_SPACE),      TG(_NUMB), KC_Q , KC_O, LT(_SYMB,KC_S), TO(_BASE), KC_N,
// |--------+--------+--------+--------+--------+-----------|                                         |--------+--------+--------+--------+--------+--------|
  KC_B, KC_G, KC_C, KC_V,KC_TAB, G(KC_D),                                                              HIBERNATE, KC_Y,  KC_U, KC_M, LT(0,P_ENIE), KC_J,
// |--------+--------+--------+--------+--------+-----------|                                         |--------+--------+--------+--------+--------+--------+--------|
                         LSFT_T(KC_ENT), LT(0,UNDO_WIN),  LT(0,UNDO_WIN),                     		           TO(_BASE), KC_CAPS,  RSFT_T(KC_SPACE)
                         // `--------------------------------'                                         `--------------------------'
    ),

    // _AI Ly 3
    [_AI] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
         XXXXXXX, A(KC_0), A(KC_2), XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, KC_HASH, LT(0,KC_SLSH), KC_AT, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
        C(KC_G), C(KC_B), C(S(KC_SLSH)), A(KC_1), XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, S(KC_TAB), XXXXXXX,XXXXXXX, C(S(KC_L)),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                       XXXXXXX, XXXXXXX, XXXXXXX,                               TO(_BASE), XXXXXXX, KC_TAB
                                       // `---------------------'                               `--------------------------'
    ),

    // _DEL Ly 4
    [_DEL] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, C(A(KC_L)), C(S(KC_J)), C(A(KC_I)), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                              XXXXXXX, XXXXXXX, KC_DEL, KC_BSPC , DEL_WORD,  DEL_LINE,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, KC_BSPC, KC_DEL, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, C(A(KC_O)), XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     DEL_LINE, _______,  _______,                                 TO(_BASE),   XXXXXXX, XXXXXXX
                                    // `----------------------'                                  `--------------------------'
),

    // _SYMB Ly 5
    [_SYMB] = LAYOUT_split_3x6_3(
        // ,--------------------------------------------------o-------------------.                ,-----------------------------------------------------.
           XXXXXXX, LT(0,ASTRISK_PLUS), LT(0, KC_MINS), KC_EXLM, S(KC_GRAVE), XXXXXXX,             XXXXXXX, KC_GRAVE, KC_PERC , LT(0,DOUBLE_COLON), LT(0,LBRC2), XXXXXXX,
        // |--------+--------+--------+--------+--------+------------------------|                |--------+--------+--------+--------+--------+--------|
           LT(0, QUESTION), LT(0,KC_DQT), KC_COMM, C(S(KC_ENT)), DOUBLE_PIPE, XXXXXXX,             XXXXXXX, AMP_DOUBLE, KC_DOT, LT(0,EQUAL_DBL), LT(0,KC_LPRN), LT(0,KC_LCBR),
        // |--------+--------+--------+--------+--------+------------------------|                |--------+--------+--------+--------+--------+--------|
           PIPE_M, LT(0,LLAMBDA), LT(0,KC_LABK), LT(0,KC_RABK), XXXXXXX, QK_BOOT,                  QK_BOOT, XXXXXXX, KC_DLR, KC_SCLN, LT(0,NOT_EQUAL), KC_CIRC,
        // |--------+--------+--------+--------+--------+--------+---------------|                |--------+--------+--------+--------+--------+--------+--------|
                                         XXXXXXX, XXXXXXX,  XXXXXXX,                               TO(_BASE), XXXXXXX, XXXXXXX
                                         // `----------------------'                                `--------------------------'
    ),

    // _NUMB Ly 6
    [_NUMB] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, LT(0,ASTRISK_PLUS), LT(0,KC_MINS), LT(0,KC_SLSH), XXXXXXX, XXXXXXX,         XXXXXXX, KC_BSPC, KC_7, KC_8, KC_9, KC_ESC,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, LT(_DEL, KC_PERC), KC_EQL, KC_DOT, KC_ENT, XXXXXXX,                        XXXXXXX, C(KC_G), KC_0, KC_4, KC_5, KC_6,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, KC_COMM, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, KC_1, KC_2, LT(0,KC_3), C(KC_G),
        // |--------+--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
                                      KC_ENT, XXXXXXX, XXXXXXX,                                 TO(_BASE), TG(_NUMB), C(S(KC_F15))
                                     // `---------------------------------'                      `--------------------------'
    ),

    // _BOOK Ly 7
    [_BOOK] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------------.                     ,-----------------------------------------------------.
           XXXXXXX, C(KC_F17), C(KC_F18), C(KC_F19), XXXXXXX, XXXXXXX,                        XXXXXXX, XXXXXXX, C(KC_5), C(KC_6), C(KC_7), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------------|                     |--------+--------+--------+--------+--------+--------|
           C(KC_F13), C(KC_F14), C(KC_F15), C(KC_F16), XXXXXXX , XXXXXXX,                     XXXXXXX, XXXXXXX, C(KC_1), LT(_BOOK_2,MARKER_2), C(KC_3), C(KC_4),
        // |--------+--------+--------+--------+--------+--------------|                     |--------+--------+--------+--------+--------+--------|
          C(KC_F20), TD(TDQ_BOOKMARK), LT(_BOOK_2,MARKER_B), C(KC_F22), XXXXXXX, XXXXXXX,             XXXXXXX, XXXXXXX, C(KC_8), C(KC_9), TD(TDQ_BOOKMARK), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------+-----|                     |--------+--------+--------+--------+--------+--------+--------|
                                           MO(_BOOK_2), XXXXXXX,  XXXXXXX,                    TO(_BASE),   XXXXXXX, MO(_BOOK_2)
                                           // `--------------------------'                     `--------------------------'
    ),

    // _BOOK_2 Ly 8
    [_BOOK_2] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, A(KC_E), A(KC_F), A(KC_G), XXXXXXX , XXXXXXX,                               XXXXXXX, XXXXXXX, C(S(KC_5)), C(S(KC_6)), C(S(KC_7)), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           A(KC_A), A(KC_B), A(KC_C), A(KC_D), XXXXXXX , XXXXXXX,                               XXXXXXX, XXXXXXX, C(S(KC_1)), C(S(KC_2)), C(S(KC_3)), C(S(KC_4)),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
          A(KC_H), XXXXXXX, A(KC_I), A(KC_K), XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, C(S(KC_8)), C(S(KC_9)), XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     XXXXXXX, XXXXXXX,  _______,                               TO(_BASE),   XXXXXXX, XXXXXXX
                                     // `------------------------'                              `--------------------------'
    ),

    // _FAST Ly 9
    //para elimirar LT(0,PAGE_PARAGRAPH_UP) LT(0,PAGE_PARAGRAPH_DOWN)  A(KC_UP),  A(KC_DOWN)
    [_FAST] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
        C(KC_S), CS_F15_HOLD ,MS_WHLU, KC_BSPC, XXXXXXX, XXXXXXX,                               XXXXXXX, XXXXXXX, KC_BSPC, MS_WHLU, CS_F15_HOLD, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
          MS_WHLL, MS_WHLD, UP_10, DOWN_10, MS_WHLR, XXXXXXX,                                   XXXXXXX, MS_WHLR, DOWN_10, UP_10, MS_WHLD, MS_WHLL,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
         KC_HOME, KC_END, C(KC_SPACE), LT(SEL_ALL,KC_SPACE), XXXXXXX, XXXXXXX,                  XXXXXXX, XXXXXXX, LT(SEL_ALL,KC_SPACE), KC_END, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                       TO(_BASE), LT(0,UNDO_WIN), C(KC_Y),                     TO(_BASE), XXXXXXX, TO(_BASE)
                                       // `----------------------'                              `--------------------------'
    ),

    // _MOVE_WIN Ly 10 C(KC_L),
    [_MOVE_WIN] = LAYOUT_split_3x6_3(
       // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
        XXXXXXX, A(KC_LEFT), A(KC_RIGHT), LGUI(KC_DOWN), XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, LGUI(KC_DOWN), A(KC_RIGHT), A(KC_LEFT), XXXXXXX,
       // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
        TO(_BASE), C(S(KC_F12)),  C(S(KC_TAB)), C(KC_TAB), XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, C(KC_TAB), C(S(KC_TAB)), C(S(KC_F12)), TO(_BASE),
       // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
        LGUI(KC_UP), XXXXXXX, G(C(KC_LEFT)), G(C(KC_RIGHT)), XXXXXXX, XXXXXXX,                  XXXXXXX,  C(G(KC_D)), G(C(KC_LEFT)), G(C(KC_RIGHT)), XXXXXXX, TO(_BASE),
       // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                      XXXXXXX , _______,  _______,                               TO(_BASE), XXXXXXX,XXXXXXX
                                     // `------------------------'                               `--------------------------'
    ),


    // _RUN Ly 11
    [_RUN] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX,  A(S(KC_F10)), C(S(KC_F10)), S(KC_F10), XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, KC_F6, C(KC_F5), XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                        C(KC_F2), _______,  _______,                             TO(_BASE), XXXXXXX, XXXXXXX
                                        // `----------------------'                              `--------------------------'
    ),

    // _MOUSE_KEY Ly 12
    [_MOUSE_KEY] = LAYOUT_split_3x6_3(
    // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
     XXXXXXX, XXXXXXX, MS_BTN3, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
    MS_BTN2, TO(_BASE),MS_DOWN, MS_UP, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, MS_LEFT, MS_RGHT, XXXXXXX, XXXXXXX,
    // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
     XXXXXXX, XXXXXXX, XXXXXXX, MS_BTN1, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, MS_BTN1, XXXXXXX, XXXXXXX, XXXXXXX,
    // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     XXXXXXX, _______,  _______,                           TO(_BASE),   XXXXXXX, XXXXXXX
                                    // `----------------------'                             `--------------------------'

    ),










};

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (data[0] == 'R' || data[1] == 'R') {
        // 1. Esperamos 50ms para que el clic del mouse "entre" en Windows primero
        //wait_ms(50);

        // 2. Ahora sí, soltamos el Alt físicamente
       clear_all();

        alt_tab_hold_done = false;
        alt_tab_pressed = false;
        alt_tab_timer = timer_read();

        // 4. Confirmación a Python
        send_layer_status_with_at("AT_OFF", layer_state);
    }

        // === NUEVA LÓGICA PARA CALIBRAR EL MOUSE ===
    else if (data[0] == 'M') {
#ifdef MOUSEKEY_ENABLE
        // Recibimos consecutivamente desde la posición 1
    mk_delay       = data[1]; // Lee buf[1] de Python
    mk_max_speed   = data[2]; // Lee buf[2] de Python
    mk_time_to_max = data[3]; // Lee buf[3] de Python
    mk_interval    = data[4]; // Lee buf[4] de Python
#endif
    }
}

//Tap dance imple

// > >=  plantilla
/*
void dance_rabk(tap_dance_state_t *state, void *user_data) {

   switch (state->count) {
      case 1: SEND_STRING(SS_LSFT(SS_TAP(X_DOT))); break; //1tap
      case 2: SEND_STRING(SS_LSFT(SS_TAP(X_DOT)) SS_TAP(X_EQUAL)); break;//2tap
      case 3: SEND_STRING("{}" SS_TAP(X_LEFT)); break; //3tap

   }
}

// < <=  plantilla
void dance_labk(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) { //1tap
       SEND_STRING(SS_LSFT(SS_TAP(X_COMM)));
    } else {
       SEND_STRING(SS_LSFT(SS_TAP(X_COMM)) SS_TAP(X_EQUAL));
    }
}*/


//Quad impl
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;

    }else if (state->count == 2) {
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;

    }if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

// Create an instance of 'td_tap_t' for the 'x' tap dance.
static td_tap_t xtap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void tdq_sel_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP://
                    //select all
              tap_code16(C(KC_A));

        break;

        case TD_SINGLE_HOLD: //
                 //sel 1 line
                tap_code_delay(KC_HOME, 10);
                tap_code_delay(KC_HOME, 10);
                tap_code16_delay(S(KC_END), 10);
                //tap_code16_delay(C(KC_C), 10);

         break;

        case TD_DOUBLE_TAP:
            //select word
            tap_code(KC_SPACE);
            //tap_code16_delay(C(KC_LEFT), 10);
            //tap_code16_delay(C(S(KC_RIGHT)), 10);

                    // HOLD: seleccionar párrafo
                   // tap_code16_delay(KC_END, 10);
                   // tap_code16_delay(KC_HOME, 10);
                    //tap_code16_delay(KC_HOME, 30);
                    //tap_code16(S(A(KC_PGDN))); // Shift + Alt + PgDn

        break;

        default: break;
    }
}


void tdq_bookmark_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: //show list markers floating window
                            tap_code16(S(KC_F11)); // Shift + F11
                            break;

        case TD_SINGLE_HOLD:
                            tap_code16(A(KC_2));
                            break; // show list markers

        case TD_DOUBLE_TAP://create marker
                            tap_code(KC_F11);
                            break;

        case TD_DOUBLE_HOLD: //go to nmotecnic
                            tap_code16(C(S(KC_F11))); // Ctrl + Shift + F11
                            break;

        default: break;
    }
}

void tdq_esc_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:
            // Escape: si cancelabas una captura con el clic sostenido enganchado,
            // hay que soltarlo aquí también, si no se queda pegado hasta el próximo MS_BTN1.
            if (mouse_held) {
                unregister_code(MS_BTN1);
                mouse_held = false;
            }
            tap_code(KC_ESC);
            break;

        case TD_SINGLE_HOLD:
            // Minimizar todo (Win+D): mismo caso, libera el clic sostenido si quedó pegado.
            if (mouse_held) {
                unregister_code(MS_BTN1);
                mouse_held = false;
            }
            tap_code16(G(KC_D));
            break;

        case TD_DOUBLE_TAP:
            // Clic derecho
            tap_code16(MS_BTN2);
            break;

        default:
            break;
    }
}

void tdq_paste_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:
            // Pegar (Ctrl+V)
            tap_code16(C(KC_V));
            break;

        case TD_SINGLE_HOLD:
            // Recargar página (F5)
            tap_code(KC_F5);
            break;

        case TD_DOUBLE_TAP:
            // Pantalla completa (F11)
            tap_code(KC_F11);
            break;

        case TD_DOUBLE_HOLD:
            // Portapapeles (Win+V)
            tap_code16(G(KC_V));
            break;

        default:
            break;
    }
}

void tdq_mouse_hold_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:
            // Alternar clic izquierdo sostenido
            toggle_mouse_hold();
            break;

        case TD_SINGLE_HOLD:
            // TEMPORAL: clic izquierdo suelto antes que nada, como el "desatasque" manual
            // que ya te funciona (clic con el teclado), por si el botón quedó pegado por
            // fuera de este ciclo.
            tap_code(MS_BTN1);
            // Imprimir pantalla + engancha el clic izquierdo sostenido.
            // El clic queda sostenido aunque sueltes la tecla (no se libera en x_reset):
            // sostén un momento -> suelta -> mueve el mouse para seleccionar la captura ->
            // toca la tecla una vez (TD_SINGLE_TAP -> toggle_mouse_hold) para soltar el clic.
            tap_code(KC_PSCR);
            wait_ms(300); // espera a que Windows abra el overlay de Recorte de pantalla y le dé foco
            // Limpieza incondicional: sin importar lo que diga mouse_held (podría estar
            // desincronizado de lo que de verdad tiene registrado el sistema), garantizamos
            // que el botón quede "arriba" antes de presionarlo de nuevo, para asegurar un
            // flanco de clic realmente nuevo.
            unregister_code(MS_BTN1);
            mouse_held = false;
            wait_ms(50); // deja que el sistema procese el "soltar" antes de mandar un "presionar" nuevo
            register_code(MS_BTN1);
            mouse_held = true;
            break;

        case TD_DOUBLE_TAP:
            // Buscar palabra (Ctrl+F)
            tap_code16(C(KC_F));
            break;

        case TD_DOUBLE_HOLD:
            // Captura de ventana activa (Alt+Print Screen)
            tap_code16(A(KC_PSCR));
            break;

        default:
            break;
    }
}



void x_reset(tap_dance_state_t *state, void *user_data) {
//  if (xtap_state.state == TD_DOUBLE_SINGLE_TAP)
//      {
//             clear_keyboard();
//      }

//   switch (xtap_state.state) {
//            case TD_SINGLE_HOLD: unregister_code(KC_LSFT); break;
//            case TD_DOUBLE_SINGLE_TAP: clear_keyboard(); break;
//            default: break;
//       }

    xtap_state.state = TD_NONE;

}

//Quad END


//tapping term
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {

//#cuando se quiere poner exception a un Tap dance
   if (keycode == TD(TDQ_SEL))
     {return 300;}

   if (QK_TAP_DANCE <= keycode && keycode <= QK_TAP_DANCE_MAX) {
        return 450;
    }
/*
    if (keycode == LT(1, KC_RIGHT)) {
            return 200;  // prueba 110–130 ms
        }
*/

      return TAPPING_TERM;
  }



/////RGB MATRIX
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
// 1. Apagamos absolutamente todos los LEDs primero
    rgb_matrix_set_color_all(0, 0, 0);
uint8_t layer = get_highest_layer(layer_state);

 switch (layer) {
         case _MOUSE_KEY:
             rgb_matrix_set_color(8, RGB_BLUE);  // R, G, B
             break;

     case _AI:
         rgb_matrix_set_color(8, RGB_TURQUOISE);
         break;


         case _MOVE:
             rgb_matrix_set_color(8, RGB_YELLOW);  // R, G, B
             break;

         case _ALFA:
            rgb_matrix_set_color(8, 0, 100, 0); //gris suave
             break;

         case _NUMB:
             rgb_matrix_set_color(8, 128, 128, 128);
             break;

         case _MOVE_WIN:
             rgb_matrix_set_color(8, RGB_PURPLE);
             break;



         case _FAST:
             rgb_matrix_set_color(8, RGB_RED);
             break;

         default:
             //apagar todos los LEDs
             rgb_matrix_set_color_all(0, 0, 0);
             break;
     }
     return false;
}


//RGB LIGHT


//capslock
//{0, 0, HSV_OFF} {starting, numbers_leds, HSV_OFF}
/*
const rgblight_segment_t PROGMEM my_capslock_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 0, HSV_OFF}
);

// _alfa_layer ly2
const rgblight_segment_t PROGMEM _alfa_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {9,1, HSV_BLUE}
//    {8,1, HSV_RED} //PLAN B
);

// _numb_layer ly6
const rgblight_segment_t PROGMEM _numb_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_WHITE}
    //    {4,2, HSV_RED} //PLAN B
);

// _move_layer ly1
const rgblight_segment_t PROGMEM _move_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_BLUE}
);
*/

/*
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    NULL,               // 0 _BASE
    _move_layer,        // 1 _MOVE
    _alfa_layer,        // 2 _ALFA
    NULL,               // 3 _AI
    NULL,               // 4 _DEL
    NULL,               // 5 _SYMB
    _numb_layer,        // 6 _NUMB
    NULL,               // 7 _BOOK
    NULL,               // 8 _BOOK_2
    NULL,               // 9 _FAST
    NULL,               // 10 _MOVE_WIN
    NULL,               // 11 _RUN
    NULL                // 12 _MOUSE_KEY
);
*/

void keyboard_post_init_user(void) {
    //rgb_matrix_set_color(8, 255, 150, 0);
    // Enable the LED layers
//    debug_enable=true;
//    debug_keyboard=true;
   // rgblight_sethsv_noeeprom(0, 0, 0); // Blanco puro//para apagar la primera capa, que no alumbren todos los leds
    //rgblight_layers = my_rgb_layers;
}





layer_state_t layer_state_set_user(layer_state_t state) {
uint8_t layer = get_highest_layer(state);

        switch (layer) {
            case _ALFA:
                 //rgblight_set_layer_state(2, true); // ALFA LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _NUMB:
                // rgblight_set_layer_state(6, true); // NUMBERS LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _MOVE_WIN:
                //rgblight_set_layer_state(10, true); // MOVE_WIN LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _FAST:
                //rgblight_set_layer_state(9, true); // FAST LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _MOVE:
                //rgblight_set_layer_state(1, true); // MOVE LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _MOUSE_KEY:
                //rgblight_set_layer_state(12, true); // MOUSE_KEY LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            default:
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
               break;
        }
    return state;
}
