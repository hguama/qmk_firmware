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
    _AI      = 3,  // movido aquí desde 17 para que LT() funcione (LT solo soporta capas 0-15)
    _DEL     = 5,
    _SYMB    = 6,
    _NUMB    = 7,
    _BOOK    = 8,
    _BOOK_2  = 9,
    _MOVE_H  = 10,
    _FAST    = 11,
    _MOVE_WIN = 12,
    _RUN     = 14,
    _MODE    = 15,
    _COMMIT  = 16,
    _MOUSE_KEY = 17,  // movido aquí desde 3
};


//Macro enum
enum custom_keycodes {
    CS_F15_HOLD = SAFE_RANGE,
    CTL_CLICK,
    TG_F22,
    CUT,
    TG_ALFA,
    SHIFT_2,
    CLOSE_WIN,
    SEL_ALL,
	TO_NUMB,
    UNDO_WIN,
    AMP_DOUBLE,
    NOT_EQUAL,
    MS_ACL0_TOGGLE,
    EQUAL_DBL,
    DOUBLE_PIPE,
    ALT_TAB,
    DEL_WORD,
    DEL_LINE,
    LLAMBDA,
    DOUBLE_COLON,
    LBRC2,
    SPLIT_WIN,
    MOUSE_HOLD,          // Mantiene el clic derecho del mouse sostenido
    SHOW_QUICK_ENT,
    CODE_COMPLET,
    COMM,
    SHIFT_TOGGLE,
    TG_0,
    TG_6,
    SLEEP,
    HIBERNATE,
    ASTRISK_PLUS,
    MARKER_B,
    MARKER_2,
    C_END_HOME,
    QUESTION,
    DOWN_10,
    UP_10,
    PIPE_M,
    P_ENIE,          // Tecla personalizada TAP: P | HOLD: Ñ
    COPY,            // Tecla personalizada para copy
    CTRL_Z,          // Keycode para usar en LT(_BOOK, CTRL_Z)
};

// --- COMBOS DESACTIVADOS (no usados, LT(KC_F22, _ALFA) no existe en ninguna capa) ---
//enum combo_events {
// CB_CENTER,
// CB_SUP_DER,
// CB_SUP_IZQ,
// CB_INF_DER,
// CB_INF_IZQ
//
//};

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
static uint16_t shift_toggle_timer = 0;
static uint16_t f22_toggle_timer = 0;
static bool space_repeat_active = false;


// para LT(_DEL, KC_PERC)
static uint8_t del_prev_layer = _BASE;
static bool del_layer_active = false;

static uint8_t base_prev_layer = _ALFA;
static bool base_layer_active = false;

bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;
static bool alt_tab_pressed = false;
static bool alt_tab_hold_done = false;

static bool shift_active = false;
static bool f22_active = false;

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


// --- COMBOS DESACTIVADOS (no usados, LT(KC_F22, _ALFA) no existe en ninguna capa) ---
//const uint16_t PROGMEM cb_ctrl_z[] = {LT(KC_F22, _ALFA), MS_BTN1, COMBO_END};
//const uint16_t PROGMEM cb_ctrl_z2[] = {MS_UP, MS_LEFT, COMBO_END};
//
//combo_t key_combos[] = {
// [CB_CENTER]   = COMBO(cb_ctrl_z, LCTL(KC_Z)),
//};

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

    if (shift_active) {
       unregister_code(KC_LSFT);
       shift_active = false;
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
        case _MODE:    layer_name = "LAYER_MODE"; break;
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

// Helper: alterna el clic izquierdo sostenido (lógica original de MOUSE_HOLD)
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

        case MOUSE_HOLD:
            if (record->event.pressed) {
                toggle_mouse_hold();
            }
            return false;


        case MS_BTN1:
            if (record->event.pressed) {
                if (mouse_held) {
                    // Si el clic estaba enganchado por MOUSE_HOLD, lo soltamos inmediatamente
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

        case TO_NUMB:
            if (record->event.pressed) {
                clear_all();        // Limpia mods y estados activos
                layer_move(_NUMB);  // Mueve exclusivamente a la capa NUMB
            }
            return false; // Ya manejamos la acción, no continuar

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

        case LT(_BOOK, CTRL_Z):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: MO(_BOOK)
                    return true; // Permite que QMK maneje el layer tap
                } else {
                    // TAP: Ctrl+Z
                    tap_code16(C(KC_Z));
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
        case LT(0,SPLIT_WIN):   // HOLD: F17 (SPLIT DOWN) | TAP: F16 (SPLIT RIGHT)
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: SPLIT DOWN
                    tap_code16(KC_F17);
                } else {
                    // TAP: SPLIT RIGHT
                    tap_code16(KC_F16);
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;


        // Evaluar si se necesita (JetBrains)
        case LT(0,COMM):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    if (shift_active) {
                        unregister_code(KC_LSFT);
                        shift_active = false;
                    }
                    tap_code16(LCTL(LSFT(KC_SLSH)));
                    return false;
                } else {
                    if (shift_active) {
                        unregister_code(KC_LSFT);
                        shift_active = false;
                    }
                    tap_code16(LCTL(KC_SLSH));
                    return false;
                }
            }
            return false;

        case LT(_MOVE_H,_MOVE):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    return true; // hold - will activate _AI layer
                } else {
                    clear_all();

                    // If CAPS is on, turn it off
                    if (host_keyboard_led_state().caps_lock) {
                        tap_code(KC_CAPS);
                    }

                    layer_invert(_MOVE); // tap - toggle _MOVE layer
                    return false;
                }
            }
            return true;


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



        case LT(0,SHOW_QUICK_ENT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(LCTL(KC_F1));  // HOLD: CTRL + F1
                    return false;
                } else {
                    // TAP: ALT + ENTER (si no fue HOLD)
                    tap_code16(LALT(KC_ENT));
                    return false;
                }
            }
            return false;

        case LT(0,CODE_COMPLET):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD - code completion adv
                    tap_code16(LCTL(LSFT(KC_SPACE)));
                    return false;
                } else {
                    // TAP - code completion basic
                    tap_code16(LCTL(KC_SPACE));
                    return false;
                }
            }
            return false;

        case LT(_AI,SHIFT_TOGGLE): // TAP: Toggle Shift | HOLD: Capa _AI
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: ir a capa _AI
                    return true; // QMK maneja el HOLD automáticamente
                } else {
                    // TAP: Toggle Shift
                    shift_active = !shift_active;
                    if (shift_active) {
                        register_code(KC_LSFT);   // Activa Shift
						shift_toggle_timer = timer_read();
                    } else {
                        unregister_code(KC_LSFT); // Desactiva Shift
                    }
                    return false;
                }
            }
            return true;

        case SHIFT_TOGGLE:
            if (record->event.pressed) {
                    shift_active = !shift_active;
                    if (shift_active) {
                        register_code(KC_LSFT);   // Activa Shift
						shift_toggle_timer = timer_read();
                    } else {
                        unregister_code(KC_LSFT); // Desactiva Shift
                    }

            }
            break;

        case LT(KC_F22, TG_F22):
            if (record->event.pressed) {
                // Guardamos el tiempo en que se presionó
                f22_toggle_timer = timer_read();
                // Registramos la tecla inmediatamente para que se sienta reactivo
                register_code(KC_F22);
            } else {
                // Al soltar, verificamos cuánto tiempo pasó
                if (timer_elapsed(f22_toggle_timer) < TAPPING_TERM) {
                    // FUE UN TOQUE CORTO: Comportamiento Toggle
                    // Si ya estaba activo por el register_code de arriba, lo dejamos.
                    // Si queremos apagarlo después de un segundo toque, invertimos el estado.
                    if (f22_active) {
                        unregister_code(KC_F22);
                        f22_active = false;
                    } else {
                        f22_active = true;
                        // No hacemos unregister porque queremos que se quede prendido
                    }
                } else {
                    // FUE UNA PULSACIÓN LARGA: Comportamiento Momentary
                    // Simplemente soltamos la tecla y nos aseguramos que el estado toggle sea falso
                    unregister_code(KC_F22);
                    f22_active = false;
                }
            }
            return false; // Importante para que no procese la tecla original
            break;
 

            // layer_invert(_MOVE); // tap - toggle _MOVE layer
         case LT(_AI, TG_0):
             if (record->event.pressed) {
                if (record->tap.count == 0) {
                    // HOLD (sin taps): mover exclusivamente a _AI
                    del_prev_layer = biton32(layer_state); // guarda la capa activa más alta
                    layer_move(_AI);                       // dejamos _AI sola (prioridad)
                    del_layer_active = true;
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple: enviar
                    layer_invert(_MOVE_WIN); // tap - toggle _MOVE layer
                    return false;
                }
            } else {
                // RELEASE: si activamos la capa, restauramos la previa
                if (del_layer_active) {
                    layer_move(del_prev_layer);
                    del_layer_active = false;
                }
            }
            return true;

        case LT(_RUN, MS_BTN2):
            if (record->event.pressed) {
                if (record->tap.count == 0) {
                    // HOLD (sin taps): mover exclusivamente a _RUN
                    del_prev_layer = biton32(layer_state); // guarda la capa activa más alta
                    layer_move(_RUN);                       // dejamos _RUN sola (prioridad)
                    del_layer_active = true;
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple: enviar
                    tap_code16(MS_BTN2);
                    return false;
                }
            } else {
                // RELEASE: si activamos la capa, restauramos la previa
                if (del_layer_active) {
                    layer_move(del_prev_layer);
                    del_layer_active = false;
                }
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

        case LT(KC_F22, KC_ENT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD → F22 DOWN real
                    register_code(KC_F22);
                } else {
                    // TAP → alternar capa
                    tap_code(KC_ENT);
                }
            } else {
                // RELEASE → F22 UP real
                unregister_code(KC_F22);
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


        case TG(2):
            if (!record->event.pressed) {
                clear_all();
            }
            break;

        case TG(6):
            if (!record->event.pressed) {
                clear_all();
            }
            break;

        case KC_ESC:
            if (record->event.pressed) {
                clear_all();
                tap_code(KC_ESC);
            }
            break;

        case TO(_BASE):
            clear_all();
            break;

        case LT(_DEL,TG_0):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    return true; //hold
                } else {
                    clear_all();

					// Si CAPS está activado, lo apagamos
    				if (host_keyboard_led_state().caps_lock) {
        				tap_code(KC_CAPS);
    				}

                    layer_invert(_ALFA); //tap
                    return false;
                }
            }
            return true;

        case LT(0,TG_6):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    return true; //hold
                } else {
                    clear_all();
                    layer_invert(_COMMIT); //tap
                    return false;
                }
            }
            return true;


		case LT(_DEL, KC_PERC):
   				 if (record->event.pressed) {
        			if (record->tap.count == 0) {
            			// HOLD (sin taps): mover exclusivamente a _DEL
            			del_prev_layer = biton32(layer_state); // guarda la capa activa más alta
            			layer_move(_DEL);                       // dejamos _DEL sola (prioridad)
            			del_layer_active = true;
            			return false;
        			}
				if (record->tap.count == 1) {
					// TAP simple: enviar %
					tap_code16(KC_PERC);
					return false;
					}
				} else {
					// RELEASE: si activamos la capa, restauramos la previa
					if (del_layer_active) {
						layer_move(del_prev_layer);
						del_layer_active = false;
						}
				}
			return true;

        case LT(_BASE,KC_I):
            if (record->event.pressed) {
                if (record->tap.count == 0) {
                    // HOLD (sin taps): mover exclusivamente a _MOVE
                    base_prev_layer = biton32(layer_state); // guarda la capa activa más alta
                    layer_move(_MOVE);                       // dejamos _MOVE sola (prioridad)
                    base_layer_active = true;
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple: enviar F
                    tap_code(KC_I);
                    return false;
                }
            } else {
                // RELEASE: si activamos la capa, restauramos la previa
                if (base_layer_active) {
                    layer_move(base_prev_layer);
                    base_layer_active = false;
                }
            }
            return true;


        case LT(_BASE,KC_O):
            if (record->event.pressed) {
                if (record->tap.count == 0) {
                    // HOLD (sin taps): mover exclusivamente a _MOVE
                    base_prev_layer = biton32(layer_state); // guarda la capa activa más alta
                    layer_move(_MOVE);                       // dejamos _MOVE sola (prioridad)
                    base_layer_active = true;
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple: enviar F
                    tap_code(KC_O);
                    return false;
                }
            } else {
                // RELEASE: si activamos la capa, restauramos la previa
                if (base_layer_active) {
                    layer_move(base_prev_layer);
                    base_layer_active = false;
                }
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



        case LT(_MOVE_H, COPY):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Activar capa _MOVE_H
                    return true; // QMK maneja el HOLD automáticamente
                } else {
                    // TAP: Ctrl+C para copiar
					 clear_all();
                   	 tap_code16(C(KC_C));

                    return false;
                }
            }
            return true;

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

        case LT(KC_S, SHIFT_2):
            if (record->event.pressed) {
                if (record->tap.count > 0) {
                    // TAP: Ctrl+S
                    tap_code16(C(KC_S));
                    return false;
                } else {
                    // HOLD: Mantiene Shift presionado
                    register_code(KC_LSFT);
                    return false;
                }
            } else {
                // Al soltar la tecla, soltamos Shift por si estaba activo
                unregister_code(KC_LSFT);
            }
            return false;

        }//END SWITCH
    return true;
};
//Pr record END


//timer for  macro
void matrix_scan_user(void) {

	//para shift_toggle
    if (shift_active) {
        if (timer_elapsed(shift_toggle_timer) > 10000) {  // 10 segundos
            unregister_code(KC_LSFT);
            shift_active = false;
        }
    }

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



//LT(KC_S, SHIFT_2) LT(_MOUSE_KEY, TG_0),   LT(_RUN,MS_BTN2) LT\(_MOVE_H\, TG_0\)
//LT(_AI, TG_0)TDQ_ESC  TDQ_ESC LT(0,CTL_GUI)


[_BASE] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                                        ,-----------------------------------------------------.
LT(KC_F4, CLOSE_WIN),  TD(TDQ_ESC), ALT_TAB, TD(TDQ_MOUSE_HOLD), LT(_AI, KC_ENT), QK_BOOT,                            QK_BOOT, KC_T, KC_T, MS_WHLD, MS_WHLU, XXXXXXX,
// |--------+--------+--------+--------+--------+--------|                                                |--------+--------+--------+--------+--------+--------|
LT(_AI, TG_0), TG(_MOUSE_KEY), TG(_FAST), MS_BTN1, TD(TDQ_PASTE), LT(SEL_ALL,KC_SPACE),                 SLEEP, TD(TDQ_PASTE), MS_BTN1, XXXXXXX, TG(_ALFA), TG(_MOVE),
// |--------+--------+--------+--------+--------+--------|                                                |--------+--------+--------+--------+--------+--------|
MS_BTN2, LT(CUT,COPY), C(KC_SPACE), MS_BTN1, KC_TAB, CTL_CLICK,                                           HIBERNATE, G(KC_D), MS_BTN2 , MS_BTN2 , MS_WHLR, MS_WHLL,
// |--------+--------+--------+--------+--------+--------|                                                |--------+--------+--------+--------+--------+--------+--------|
                                 LT(_AI, KC_ENT), LT(0,UNDO_WIN), C(KC_Y),                                MO(_BOOK_2), MO(_BOOK), LT(MS_ACL0,KC_SPACE)
                                // `---------------------'                                                  `--------------------------'
),

    //_BASE Layer   _FAST _BOOK LT(_SYMB,KC_RIGHT) LT(_SYMB,KC_DOWN)
    [_MOVE] = LAYOUT_split_3x6_3(
// ,-------------------------------------------------------------------------------------.                          ,-----------------------------------------------------.
LT(KC_F4, CLOSE_WIN), LSFT_T(KC_ESC), ALT_TAB, LT(_NUMB,KC_TAB), MOUSE_HOLD, QK_BOOT,                              QK_BOOT, XXXXXXX, TD(TDQ_SEL), LT(_RUN,KC_HOME), LSFT_T(KC_END), XXXXXXX,
// |--------+--------+--------+--------+--------+----------------------------------------|                          |--------+--------+--------+--------+--------+--------|
LT(_MOVE_H,_MOVE), MO(_DEL), KC_DOWN, KC_UP, TD(TDQ_PASTE), LT(SEL_ALL,KC_SPACE),                                               SLEEP, XXXXXXX, KC_LEFT, KC_RIGHT, XXXXXXX, TG(_MOVE),
// |--------+--------+--------+--------+--------+----------------------------------------|                          |--------+--------+--------+--------+--------+--------|
TG_ALFA, LT(CUT,COPY), KC_F24, MS_BTN1, KC_TAB, G(KC_D),                                            HIBERNATE, XXXXXXX, TG(_MODE), LT(0,SHOW_QUICK_ENT), LT(0,CODE_COMPLET), KC_INS,
 //|--------+--------+--------+--------+--------+--------+-------------------------------|                          |--------+--------+--------+--------+--------+--------+--------|
                                                       LT(_MOVE_WIN, KC_ENT), C(KC_Z), LT(0,UNDO_WIN),                TO(_BASE), KC_LCTL, KC_SPACE
                                                         // `----------------------------------'                       `---------------------------------'
    ),


    //_ALFA ly 1  LT(_DEL,TG_0)
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

    // _AI (movido aquí desde posición 17)
    [_AI] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
         XXXXXXX, A(KC_0), A(KC_2), XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, KC_HASH, LT(0,KC_SLSH), KC_AT, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
        C(KC_G), C(KC_B), C(S(KC_SLSH)), A(KC_1), XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, S(KC_TAB), XXXXXXX,XXXXXXX, C(S(KC_L)),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
         TG(_AI), XXXXXXX, XXXXXXX, TG(_AI), XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, C(KC_L), C(KC_I), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                       XXXXXXX, XXXXXXX, XXXXXXX,                               TO(_BASE), XXXXXXX, KC_TAB
                                       // `---------------------'                               `--------------------------'
    ),

    //_DEL Ly 3
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

    // _SYMB Ly 4
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

    // _NUMB Ly 5
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

    // _BOOK Ly 6
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

    // _BOOK_2 Ly 7
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

    // _MOVE_H Ly 8 LT(0,SPLIT_WIN) C(S(KC_M)) KC_F20
    [_MOVE_H] = LAYOUT_split_3x6_3(
       // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      // |--------+--------+--------+--------+--------+--------|                                     |--------+--------+--------+--------+--------+--------|
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      // |--------+--------+--------+--------+--------+--------|                                     |--------+--------+--------+--------+--------+--------|
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX , XXXXXXX, XXXXXXX,                               XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      // |--------+--------+--------+--------+--------+--------|                                     |--------+--------+--------+--------+--------+--------+--------|
                                      XXXXXXX, XXXXXXX, _______,                                TO(_BASE), XXXXXXX, LT(0,C_END_HOME)
                                     // `------------------------'                              `--------------------------'
    ),

    // _FAST Ly 9
    //para elimirar LT(0,PAGE_PARAGRAPH_UP) LT(0,PAGE_PARAGRAPH_DOWN)  A(KC_UP),  A(KC_DOWN) , ,
    [_FAST] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, CS_F15_HOLD ,MS_WHLU, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, UP_10, DOWN_10, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
          MS_WHLL, MS_WHLD, UP_10, DOWN_10, MS_WHLR, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, A(KC_UP), XXXXXXX, A(KC_DOWN),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
         TO(_BASE), XXXXXXX, C(KC_SPACE), MS_BTN1, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, MS_WHLU, MS_WHLD, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                        TO(_BASE), XXXXXXX, XXXXXXX,                               TO(_BASE), XXXXXXX, XXXXXXX
                                       // `----------------------'                              `--------------------------'
    ),

    // _MOVE_WIN Ly 10 C(KC_L),
    [_MOVE_WIN] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
        XXXXXXX, A(KC_LEFT), A(KC_RIGHT), LGUI(KC_DOWN), XXXXXXX, XXXXXXX,                        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
       TO(_BASE), C(S(KC_F12)),  C(S(KC_TAB)), C(KC_TAB), XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
       LGUI(KC_UP), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                    XXXXXXX,  C(G(KC_D)), C(KC_T), G(C(KC_RIGHT)),   G(C(KC_LEFT)), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
XXXXXXX , _______,  _______,                               TO(_BASE), XXXXXXX,XXXXXXX
                                     // `------------------------'                               `--------------------------'
    ),


    // _RUN  Ly 12
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

    // _MODE Ly 13 -move for games
    [_MODE] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, TG(_COMMIT), TO_NUMB, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, KC_DOWN, KC_UP, XXXXXXX, XXXXXXX,                          XXXXXXX, XXXXXXX, KC_LEFT, KC_RIGHT, KC_DOWN, KC_UP,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, TG(_MODE), TG(_MODE), XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                          KC_ENT, XXXXXXX, XXXXXXX,                             TO(_BASE), XXXXXXX, KC_SPACE
                                         // `---------------------'                             `--------------------------'
    ),

    // _COMMIT Ly 14     MS_ACL0_TOGGLE
    [_COMMIT] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, KC_ESC, C(KC_K), A(KC_0), XXXXXXX, XXXXXXX,                                 XXXXXXX, XXXXXXX, XXXXXXX, C(KC_Z), XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           MS_ACL2, MS_ACL1, S(KC_F7), KC_F7, XXXXXXX, C(KC_S),                                 SLEEP, XXXXXXX, MS_WHLL, MS_WHLD, MS_WHLR, MS_WHLU,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, MS_ACL0, XXXXXXX, LT(SEL_ALL,KC_SPACE),                                  HIBERNATE, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     C(A(KC_R)),  XXXXXXX, XXXXXXX,                             TO(_BASE), XXXXXXX, TO(_BASE)
                                     // `-------------------------'                             `--------------------------'
    ),

    // _MOUSE_KEY (movido aquí desde posición 3)
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
            // Escape
            tap_code(KC_ESC);
            break;

        case TD_SINGLE_HOLD:
            // Minimizar todo (Win+D)
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
            // Alternar clic izquierdo sostenido (misma lógica que MOUSE_HOLD)
            toggle_mouse_hold();
            break;

        case TD_SINGLE_HOLD:
            // Imprimir pantalla
            tap_code(KC_PSCR);
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

         case _MODE:
             rgb_matrix_set_color(8, RGB_PURPLE);
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

// _alfa_layer ly1
const rgblight_segment_t PROGMEM _alfa_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {9,1, HSV_BLUE}
//    {8,1, HSV_RED} //PLAN B
);

// _numb_layer ly5
const rgblight_segment_t PROGMEM _numb_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_WHITE}
    //    {4,2, HSV_RED} //PLAN B
);

// _mode_layer ly13
const rgblight_segment_t PROGMEM _mode_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_PURPLE}
    //    {4,2, HSV_RED} //PLAN B
);

// _commit_layer ly14
const rgblight_segment_t PROGMEM _commit_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_RED}
//        {4,2, HSV_RED} //PLAN B
);

// __NEW_layer ly16
const rgblight_segment_t PROGMEM __NEW_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_YELLOW}
);

// _move_layer ly17
const rgblight_segment_t PROGMEM _move_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_BLUE}
);
*/

/*
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    NULL,               // 0
    NULL,
    _alfa_layer,        // 1
    NULL,               // 2
    NULL,               // 3
    NULL,               // 4
    _numb_layer,        // 5
    NULL,               // 6
    NULL,               // 7
    NULL,               // 8
    NULL,               // 9
    NULL,               // 10
    NULL,               // 11
    NULL,               // 12
    _mode_layer,        // 13
    _commit_layer,      // 14
    NULL,               // 15
    __NEW_layer,     // 16
    _move_layer      // 17
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
//    rgblight_set_layer_state(INDEX_LIGHT, layer_state_cmp(state, _LAYER));
//    rgblight_set_layer_state(5, layer_state_cmp(state, 5));

/*
    rgblight_set_layer_state(1, false);  // _ALFA LY OFF
    rgblight_set_layer_state(5, false);  // _NUMB LY OFF
    rgblight_set_layer_state(13, false); // _MODE LY OFF
    rgblight_set_layer_state(14, false); // _COMMIT LY OFF
    rgblight_set_layer_state(16, false); // _NEW LY OFF
    rgblight_set_layer_state(17, false); // _NEW LY OFF
*/


uint8_t layer = get_highest_layer(state);

        switch (layer) {
            case _ALFA:
                 //rgblight_set_layer_state(1, true); // ALFA LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _NUMB:
                // rgblight_set_layer_state(5, true); // NUMBERS LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _MODE:
                //rgblight_set_layer_state(13, true); // MODE   LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _MOVE_WIN:
                //rgblight_set_layer_state(14, true); // COMMIT LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _FAST:
                //rgblight_set_layer_state(16, true); // MOUSE_1 LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

      		 case _MOVE:
                //rgblight_set_layer_state(17, true); // MOUSE_2 LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            case _MOUSE_KEY:
                //rgblight_set_layer_state(17, true); // MOUSE_2 LY
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
                break;

            default:
                send_layer_status_with_at("", state); // <--- Agregamos 'state' aquí
               break;
        }
    return state;
}
