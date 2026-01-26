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
#include "rgblight.h"
#include "raw_hid.h"


//Layer names enum
enum layer_names {
    _BASE    = 0,  // 0
    _ALFA,         // 1
    _DEV,          // 2
    _DEL,          // 3
    _SYMB,         // 4
    _NUMB,         // 5
    _BOOK,         // 6
    _BOOK_2,       // 7
    _MOVE_H,       // 8
    _MOVE_V,       // 9
    _MOVE_WIN,     // 10
    _MOVE_L,       // 11
    _RUN,          // 12
    _MODE,         // 13
    _COMMIT,       // 14
    _AI,           // 15
    _MOUSE_1,      // 16
    _MOUSE_2       // 17
};


//Macro enum
enum custom_keycodes {
    GUI_E = SAFE_RANGE,
    TG_F22,
    CUT,
    TG_ALFA,
    ENT2,
    SHIFT_2,
    CLOSE_WIN,
    SELALL_SPACE,
    SEL_ALL,
	TO_NUMB,
    CTL_GUI,
    ALT_SHIFT,
    AMP_DOUBLE,
    OPEN_QUEST,
    NOT_EQUAL,
    MS_ACL0_TOGGLE,
    DEV_LY_SPACE,
    Z_UNDO,
    TRIPLE_WHLD,
    EQUAL_DBL,
    DOUBLE_PIPE,
    VOICE_A,
    VOICE,
    CHATGPT,
    SEL_WORD_PARAGRAPH,
    ALT_TAB,
    F_W,
    V_B,
    M_Y,
    DEL_WORD,
    DEL_LINE,
    WIN_D,
    SEL_W_ALL,
    LLAMBDA,
    DOUBLE_COLON,
    LBRC2,
    PGUP_CTRLPG,
    PGDW_CTRLPG,
    PAGE_PARAGRAPH_UP,
    PAGE_PARAGRAPH_DOWN,
    EVERYW_ACT,
    PROJECT_VIEW,
    NEW_FILE,
    SPLIT_WIN,
    FULL_SCREEN,
    MAX_MIN_WIN,
    NAV_ERROR,
    MOUSE_PRESSED_CLICK,
    SHOW_QUICK_ENT,
    CODE_COMPLET,
    INFOPARM,
    COMM,
    FOLDING,
    MULTICURSOR,
    EDIT_OCCURR,
    LAST_EDIT,
    RECENT_LOC,
    USAGES,
    REFACTOR,
    SHIFT_TOGGLE,
    TG_0,
    TG_6,
    SLEEP,
    HIBERNATE,
    ASTRISK_PLUS,
    MARKER_B,
    MARKER_2,
    CRIGHT_10,
    CLEFT_10,
    C_END_HOME,
    PGDN_PGUP,
    QUESTION,
    CRIGHT_5,
    CLEFT_5,
    DOWN_10,
    UP_10,
    PIPE_M,
    DEL_LAYER_PERC,  // Tecla personalizada para ir a capa _DEL momentáneamente
    ESC_W,           // Tecla personalizada TAP: ESC | HOLD: W
    T_F,           // Tecla personalizada TAP: TAB | HOLD: F
    H_J,             // Tecla personalizada TAP: H | HOLD: J
    D_Q,             // Tecla personalizada TAP: D | HOLD: Q
    L_K,             // Tecla personalizada TAP: L | HOLD: K
    G_Z,             // Tecla personalizada TAP: G | HOLD: Z
    C_X,             // Tecla personalizada TAP: C | HOLD: X
    P_ENIE,          // Tecla personalizada TAP: P | HOLD: Ñ
    COPY,            // Tecla personalizada para copy
    PASTE,           // Tecla personalizada para paste
    CTRL_Z,          // Keycode para usar en LT(_BOOK, CTRL_Z)
};

//Combo enum
//enum combo_events {
//  CB_CTRL_Z,   // identificador del combo
//};

//Tap Dance enum
enum {
    TDQ_SEL,
    TDQ_PASTE,
    TDQ_CUT,
    TDQ_BOOKMARK,
    TDQ_GOTO,
    TDQ_FIND,
    TDQ_REPLACE,
    TDQ_OVERRIDE,
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



// Prototypes quad
td_state_t cur_dance(tap_dance_state_t *state);
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);

void tdq_sel_finished(tap_dance_state_t *state, void *user_data);
void tdq_bookmark_finished(tap_dance_state_t *state, void *user_data);
void tdq_goto_finished(tap_dance_state_t *state, void *user_data);
void tdq_find_finished(tap_dance_state_t *state, void *user_data);
void tdq_replace_finished(tap_dance_state_t *state, void *user_data);
void tdq_override_finished(tap_dance_state_t *state, void *user_data);


//Config revert to repeat key
/*uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {

    //form1
    if ((mods & MOD_MASK_CTRL)) {  // Was Ctrl held?
        switch (keycode) {
            case KC_TAB: return C(S(KC_TAB)); //ctrl tab # ctrl shift tab
        }
    }

   //form2
   switch (keycode) { //Was the modifier key pressed?
            case C(KC_Z): return  C(KC_Y); //ctrl z # ctrl y
            case C(KC_Y): return  C(KC_Z); //ctrl y # ctrl z
        }

    return KC_TRNS;  // Defer to default definitions.
}*/


//Combos
//const uint16_t PROGMEM cb_ctrl_z[] = {KC_J, LT(0,KC_K), COMBO_END};

//combo actions
//combo_t key_combos[] = {
// [CB_CTRL_Z]   = COMBO(cb_ctrl_z, LCTL(KC_Z)),
// };

//general functions
void clear_all(void) {
    clear_keyboard();         // Libera cualquier tecla registrada
//    clear_mods();             // Libera Ctrl, Shift, Alt, etc.

    if (is_alt_tab_active)   {
       unregister_code(KC_LALT);
       unregister_code(KC_TAB);
       is_alt_tab_active = false;
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


// ============================================================================
// PROCESS RECORD USER FUNCTION
// ============================================================================

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {

        case TO_NUMB:
            if (record->event.pressed) {
                clear_all();        // Limpia mods y estados activos
                layer_move(_NUMB);  // Mueve exclusivamente a la capa NUMB
            }
            return false; // Ya manejamos la acción, no continuar

        case ALT_SHIFT:
            if (record->event.pressed) {
                register_code(KC_LALT);
                register_code(KC_LSFT);
            } else {
                unregister_code(KC_LSFT);
                unregister_code(KC_LALT);
            }
            return false;

        case WIN_D:
            if (record->event.pressed) {
                tap_code16(G(KC_D)); // Win + D
            }
            break;

        case MS_ACL0_TOGGLE:
            if (record->event.pressed) {
                ms_acl0_active = !ms_acl0_active; // Cambia el estado
                if (ms_acl0_active) {
                    register_code(KC_MS_ACCEL0);   // Activa y mantiene
                } else {
                    unregister_code(KC_MS_ACCEL0); // Desactiva
                }
            }
            return false; // Ya manejamos la tecla

/*        case LT(_DEV,KC_SPACE):
               if (record->event.pressed) {
                clear_all();
                }
            return true;*/

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



        case LT(0,GUI_E):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_LGUI);
                    return false;
                } else {
                    tap_code(KC_E);
                    return false;
                }
            }
            return false;

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

        case LT(0,SEL_W_ALL):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Ctrl+A
                    tap_code16(C(KC_A));
                    return false;
                } else {
                    // TAP: Ctrl+W
                    tap_code16(C(KC_W));
                    return false;
                }
            }
            return false;

        case LT(12,Z_UNDO):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(C(KC_Y));
                    return false;
                } else {
                    tap_code16(C(KC_Z));
                    return false;
                }
            }
            return false;

        case TRIPLE_WHLD:
            if (record->event.pressed) {
                for (int i = 0; i < 8; i++) {
                    tap_code16_delay(MS_WHLD,10);
                }
                tap_code16(KC_ENT);
            }
            return false;

        case DOUBLE_PIPE:
            if (record->event.pressed) {
                tap_code16(KC_PIPE);
                tap_code16(KC_PIPE);
            }
            return false; // evita que se procese por defecto


        case LT(0, VOICE_A):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Ctrl + Win + S
                    tap_code16(C(G(KC_S)));
                    return false;
                } else {
                    // TAP: Alt + Shift + B
                    tap_code16(A(S(KC_B)));
                    return false;
                }
            }
            return false;

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

        case LT(0, CRIGHT_10):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: KC_RIGHT x10
                    for (int i = 0; i < 20; i++) {
                        tap_code(KC_RIGHT);
                    }
                    return false;
                } else {
                    // TAP: KC_RIGHT
                    tap_code16(C(KC_RIGHT));
                    return false;
                }
            }
            return false;

        case LT(0, CLEFT_10):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: KC_LEFT x10
                    for (int i = 0; i < 20; i++) {
                        tap_code(KC_LEFT);
                    }
                    return false;
                } else {
                    // TAP: KC_LEFT
                    tap_code16(C(KC_LEFT));
                    return false;
                }
            }
            return false;

        case LT(0, C_END_HOME):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: C(KC_HOME)
                    tap_code16(C(KC_HOME));
                    return false;
                } else {
                    // TAP: C(KC_END)
                    tap_code16(C(KC_END));
                    return false;
                }
            }
            return false;

        case LT(0, PGDN_PGUP):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: KC_PGUP
                    tap_code(KC_PGUP);
                    return false;
                } else {
                    // TAP: KC_PGDN
                    tap_code(KC_PGDN);
                    return false;
                }
            }
            return false;

        case LT(0, QUESTION):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: ¿
                    SEND_STRING(
                        SS_DOWN(X_LALT)
                        SS_TAP(X_KP_0)
                        SS_TAP(X_KP_1)
                        SS_TAP(X_KP_9)
                        SS_TAP(X_KP_1)
                        SS_UP(X_LALT)
                    );
                    return false;
                } else {
                    // TAP: ?
                    tap_code16(KC_QUES);
                    return false;
                }
            }
            return false;

        case CRIGHT_5:
            if (record->event.pressed) {
                // KC_RIGHT 5 veces
                for (int i = 0; i < 5; i++) {
                    tap_code16(C(KC_RIGHT));
                }
            }
            return false;

        case CLEFT_5:
            if (record->event.pressed) {
                // KC_LEFT 5 veces
                for (int i = 0; i < 5; i++) {
                    tap_code16(C(KC_LEFT));
                }
            }
            return false;

        case DOWN_10:
            if (record->event.pressed) {
                // KC_DOWN 10 veces
                for (int i = 0; i < 10; i++) {
                    tap_code(KC_DOWN);
                }
            }
            return false;

        case UP_10:
            if (record->event.pressed) {
                // KC_UP 10 veces
                for (int i = 0; i < 10; i++) {
                    tap_code(KC_UP);
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

        case LT(0,VOICE):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(G(KC_SPC)); // Solo cambia idioma a español
                    return false;
                } else {
                    // TAP: Activar/Desactivar dictado y cambiar idioma
                    if (!voice_mode) {
                        tap_code16(G(KC_SPC));
                        tap_code16(G(KC_H));   // Activar dictado
                        voice_mode = true;
                    } else {
                        tap_code16(G(KC_SPC));
                        tap_code(KC_ESC);      // Detener dictado
                        voice_mode = false;
                    }
                    return false;
                }
            }
            return false;

        case LT(0,CHATGPT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // TAP: 3 TABs + ENTER
                    tap_code_delay(KC_ENT, 80);
                    tap_code_delay(KC_TAB, 80);
                    tap_code_delay(KC_TAB, 80);
                    tap_code_delay(KC_ENT, 80);
                    return false;
                }
                /*
                else {
                    // HOLD: 2 TABs + ENTER
                    tap_code_delay(KC_TAB, 80);
                    tap_code_delay(KC_TAB, 80);
                    tap_code_delay(KC_ENT, 80);
                    return false;
                }
                */
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
            } else {
                alt_tab_pressed = false;

                if (!alt_tab_hold_done) {
                    // TAP: comportamiento actual (Alt sostenido + Tab)
                    if (!is_alt_tab_active) {
                        is_alt_tab_active = true;
                        register_code(KC_LALT);
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
                tap_code16_delay(LGUI(KC_X), 120);
                tap_code16_delay(KC_U,120);
                wait_ms(300);
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
                    SEND_STRING("<=");
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

        case LT(_DEV,KC_ENT):
            if (record->event.pressed) {
                clear_all();
            }
            return true;

        case LT(0, PGUP_CTRLPG):   // TAP = Ctrl+PgUp | HOLD = PgUp
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: PAGE UP
                    tap_code(KC_PGUP);
                } else {
                    // TAP: CTRL + PAGE UP
                    tap_code16(C(KC_PGUP));
                }
                return false; // Bloquea comportamiento por defecto
            }
            return true;

        case LT(0, PGDW_CTRLPG):   // TAP = Ctrl+PgDn | HOLD = PgDn
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: PAGE DOWN
                    tap_code(KC_PGDN);
                } else {
                    // TAP: CTRL + PAGE DOWN
                    tap_code16(C(KC_PGDN));
                }
                return false;
            }
            return true;

        case LT(0,PAGE_PARAGRAPH_UP):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD para PAGE_PARAGRAPH_UP
                    tap_code16(A(KC_PGUP));
                    tap_code(KC_DOWN);
                    tap_code(KC_END);
                    return false;
                } else {
                    tap_code16(A(KC_PGUP));
                    return false;
                }
            }
            return false;


        case LT(0,PAGE_PARAGRAPH_DOWN):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD para PAGE_PARAGRAPH_DOWN
                    tap_code16(A(KC_PGDN));
                    tap_code(KC_UP);
                    tap_code(KC_END);
                    return false;
                } else {
                    tap_code16(A(KC_PGDN));
                    return false;
                }
            }
            return false;

        case LT(0, SEL_WORD_PARAGRAPH):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: seleccionar párrafo (ejecutar inmediatamente)
                    tap_code16_delay(KC_END, 10);
                    tap_code16_delay(KC_HOME, 10);
                    tap_code16_delay(KC_HOME, 30);
                    tap_code16(S(A(KC_PGDN))); // Shift + Alt + PgDn
                    return false;
                } else {
                    // TAP: seleccionar palabra
                    tap_code16_delay(C(KC_LEFT), 10);
                    tap_code16_delay(C(S(KC_RIGHT)), 10);
                    return false;
                }
            }
            return false;

        case LT(0, EVERYW_ACT):   // HOLD: Ctrl+Shift+A | TAP: F14
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Ctrl+Shift+A
                    tap_code16(C(S(KC_A)));
                } else {
                    // TAP: F14
                    tap_code16(KC_F14);
                }
                return false; // Bloquea comportamiento por defecto
            }
            return true;

        case LT(0, NAV_ERROR):   // HOLD: Shift+F2 | TAP: F2
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Shift+F2
                    tap_code16(S(KC_F2));
                } else {
                    // TAP: F2
                    tap_code(KC_F2);
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;


        case LT(0,V_B):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_B);
                    return false;
                } else {
                    tap_code(KC_V);
                    return false;
                }
            }
            return false;

        case LT(0,M_Y):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_Y);
                    return false;
                } else {
                    tap_code(KC_M);
                    return false;
                }
            }
            return false;

        case LT(0,F_W):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_W);
                    return false;
                } else {
                    tap_code(KC_F);
                    return false;
                }
            }
            return false;

        case OPEN_QUEST: // ¿
            if (record->event.pressed) {
                SEND_STRING(
                    SS_DOWN(X_LALT)
                    SS_TAP(X_KP_0)
                    SS_TAP(X_KP_1)
                    SS_TAP(X_KP_9)
                    SS_TAP(X_KP_1)
                    SS_UP(X_LALT)
                );
            }
            break;

        case LT(0, PROJECT_VIEW):   // HOLD: Alt+F1 | TAP: Alt+1
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Alt+F1 (PROJECT SELECT IN)
                    tap_code16(A(KC_F1));
                } else {
                    // TAP: Alt+1 (PROJECT)
                    tap_code16(A(KC_1));
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

        case LT(0, NEW_FILE):   // HOLD: F15 (NEW CLASS) | TAP: Alt+Insert (NEW FILE)
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: NEW CLASS
                    tap_code16(KC_F15);
                } else {
                    // TAP: NEW FILE
                    tap_code16(A(KC_INS));
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

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


        case LT(0, FULL_SCREEN):   // HOLD: F19 (ZEN MODE) | TAP: F18 (FULL SCREEN)
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: ZEN MODE
                    tap_code16(KC_F19);
                } else {
                    // TAP: FULL SCREEN
                    tap_code16(KC_F18);
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

        case LT(0, MAX_MIN_WIN):   // HOLD: RGUI+Down (MIN WIN) | TAP: RGUI+Up (MAX WIN)
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: MIN WIN
                    tap_code16(RGUI(KC_DOWN));
                } else {
                    // TAP: MAX WIN
                    tap_code16(RGUI(KC_UP));
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

        case LT(0, LAST_EDIT):   // HOLD: F13 | TAP: Ctrl+Shift+Backspace
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: F13
                    tap_code(KC_F13);
                } else {
                    // TAP: Ctrl+Shift+Backspace
                    tap_code16_delay(C(S(KC_BSPC)), 30);
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;



        case LT(0,RECENT_LOC):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(C(S(KC_E)));
                    return false;
                } else {
                    tap_code16(C(KC_E));
                    return false;
                }
            }
            return false;

        case LT(0, USAGES):   // HOLD: Alt+F7 | TAP: Ctrl+Alt+F7
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Alt+F7
                    tap_code16(A(KC_F7));
                } else {
                    // TAP: Ctrl+Alt+F7
                    tap_code16(C(A(KC_F7)));
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

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

        case LT(0,MOUSE_PRESSED_CLICK):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(MS_BTN1);
                    return false;
                } else {
                    tap_code(MS_BTN1);
                    register_code(MS_BTN1);
                    return false;
                }
            }
            return false;

        case LT(_AI,_MOUSE_2):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    return true; // hold - will activate _AI layer
                } else {
                    clear_all();

                    // If CAPS is on, turn it off
                    if (host_keyboard_led_state().caps_lock) {
                        tap_code(KC_CAPS);
                    }

                    layer_invert(_MOUSE_2); // tap - toggle _MOUSE_2 layer
                    return false;
                }
            }
            return true;

        case LT(0, CTL_GUI):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: mantener Alt + Shift mientras se mantiene la tecla
                    register_code(KC_LCTL);

                    return false;
                } else {
                    // TAP: limpiar y alternar capa de mouse
                    clear_all();
                    tap_code(KC_LGUI);
                    return false;
                }
            } else {
                // Cuando se suelta, liberar Alt + Shift
                unregister_code(KC_LCTL);

                return false;
            }



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

        case LT(0, INFOPARM):   // HOLD: Alt+Q | TAP: Ctrl+P
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Alt+Q (view method context info)
                    tap_code16(A(KC_Q));
                } else {
                    // TAP: Ctrl+P (view info parameter)
                    tap_code16(LCTL(KC_P));
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

        case LT(0, FOLDING):   // HOLD: Ctrl+Shift+Keypad- (contrae todos) | TAP: F22 (desplegar 1 región)
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: contrae todos (ejemplo comentado)
                    // tap_code16(LCTL(LSFT(KC_KP_MINUS)));
                } else {
                    // TAP: desplegar 1 región
                    tap_code16(KC_F22);
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

        case LT(0, MULTICURSOR):   // HOLD: Alt+Shift+G | TAP: Alt+Shift+Insert
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: multicursor ends of line
                    tap_code16(LALT(LSFT(KC_G)));
                } else {
                    // TAP: multicursor
                    tap_code16(LALT(LSFT(KC_INS)));
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

        case LT(0, EDIT_OCCURR):   // HOLD: Ctrl+F3 | TAP: F3
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Selecciona la ocurrencia a buscar
                    tap_code16(C(KC_F3));
                } else {
                    // TAP: Busca ocurrencias, se desplaza de 1 en 1
                    tap_code16(KC_F3);
                }
                return false; // Bloquea el comportamiento por defecto
            }
            return true;

        case LT(0,REFACTOR):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(LSFT(KC_F6));  // Rename
                    return false;
                } else {
                    tap_code16(LCTL(LALT(LSFT(KC_T))));  // Refactor This
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
 

        case PASTE:
            if (record->event.pressed) {
                tap_code16(C(KC_V));
            }
            break;

        case LT(KC_MS_ACCEL0, COPY):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Activar KC_MS_ACCEL0
                    register_code(KC_MS_ACCEL0);
                    return false;
                } else {
                    // TAP: Ejecutar COPY (Ctrl+C)
                    tap_code16(C(KC_C));
                    return false;
                }
            } else {
                // Al soltar la tecla, desactivar KC_MS_ACCEL0
                unregister_code(KC_MS_ACCEL0);
            }
            return false;

        case LT(KC_MS_ACCEL2, PASTE):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Activar KC_MS_ACCEL1
                    register_code(KC_MS_ACCEL2);
                    return false;
                } else {
                    // TAP: Ejecutar PASTE (Ctrl+V)
                    tap_code16(C(KC_V));
                    return false;
                }
            } else {
                // Al soltar la tecla, desactivar KC_MS_ACCEL1
                unregister_code(KC_MS_ACCEL2);
            }
            return false;


        case LT(KC_F22, TG_0):
            if (record->event.pressed) {
                // REGISTRO INMEDIATO: No esperamos a record->tap.count
                // Esto le dice a Python "F22 está abajo" AHORA MISMO.
                register_code(KC_F22);

                if (record->tap.count) {
                    // Si al final resulta ser un toque rápido, igual invertimos capa
                    layer_invert(_MOUSE_2);
                }
            } else {
                unregister_code(KC_F22);
            }
            return false;



        case TG_ALFA:
            if (record->event.pressed) {
                if (IS_LAYER_ON(_MOUSE_2)) {
                    // Si estoy en Mouse, la apago y enciendo Alfa
                    layer_off(_MOUSE_2);
                    layer_on(_ALFA);
                }
                else if (IS_LAYER_ON(_ALFA)) {
                    // Si ya estoy en Alfa, la apago y regreso a Mouse
                    layer_off(_ALFA);
                    layer_on(_MOUSE_2);
                }
                else {
                    // Si por alguna razón estoy en Base, voy a Mouse (o lo que prefieras)
                    layer_on(_MOUSE_2);
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

        case LT(KC_MS_ACCEL2, KC_ENT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Activar KC_MS_ACCEL2
                    register_code(KC_MS_ACCEL2);
                    return false;
                } else {
                    // TAP
                   tap_code(KC_ENT);
                    return false;
                }
            } else {
                // Al soltar la tecla, desactivar KC_MS_ACCEL2
                unregister_code(KC_MS_ACCEL2);
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
                    // HOLD (sin taps): mover exclusivamente a _BASE
                    base_prev_layer = biton32(layer_state); // guarda la capa activa más alta
                    layer_move(_BASE);                       // dejamos _BASE sola (prioridad)
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

        case LT(0,ESC_W):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_W); //HOLD
                    return false;
                } else {
                    tap_code(KC_ESC); //TAP
                    return false;
                }
            }
            return false;

        case LT(0,T_F):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_F); //HOLD
                    return false;
                } else {
                    tap_code(KC_T); //TAP
                    return false;
                }
            }
            return false;

        case LT(0,H_J):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_J); //HOLD
                    return false;
                } else {
                    tap_code(KC_H); //TAP
                    return false;
                }
            }
            return false;

        case LT(0,D_Q):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_Q); //HOLD
                    return false;
                } else {
                    tap_code(KC_D); //TAP
                    return false;
                }
            }
            return false;

        case LT(0,L_K):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_K); //HOLD
                    return false;
                } else {
                    tap_code(KC_L); //TAP
                    return false;
                }
            }
            return false;

        case LT(0,G_Z):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_Z); //HOLD
                    return false;
                } else {
                    tap_code(KC_G); //TAP
                    return false;
                }
            }
            return false;

        case LT(0,C_X):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_X); //HOLD
                    return false;
                } else {
                    tap_code(KC_C); //TAP
                    return false;
                }
            }
            return false;

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
                    clear_keyboard();
                    _delay_ms(15); // Aumentamos ligeramente a 15ms para estabilidad

                    // Enviamos Ctrl+A de forma atómica y explícita
                    register_code(KC_LCTL);
                    _delay_ms(5); // Micro-pausa entre Ctrl y A
                    tap_code(KC_A);
                    _delay_ms(5);
                    unregister_code(KC_LCTL);

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
                    tap_code16(C(KC_W));
                    return false;
                } else {
                    // TAP: cerrar ventana
                    tap_code16(A(KC_F4));
                    return false;
                }
            }
            return false;

        case LT(KC_S, SHIFT_2):
            if (record->event.pressed) {
                if (record->tap.count > 0) {
                    // TAP: Presiona Control una vez
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

         case LT(_MOVE_WIN, ENT2):
            if (record->event.pressed) {
                if (record->tap.count == 0) {
                    // HOLD (sin taps): mover exclusivamente a _DEL
                    del_prev_layer = biton32(layer_state); // guarda la capa activa más alta
                    layer_move(_MOVE_WIN);                       // dejamos _DEL sola (prioridad)
                    del_layer_active = true;
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple:
                    tap_code(KC_ENT);
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

        case LT(KC_MS_WH_UP, KC_MS_WH_DOWN):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Scroll up
                    tap_code(KC_MS_WH_UP);
                    return false;
                } else {
                    // TAP: Scroll down
                    tap_code(KC_MS_WH_DOWN);
                    return false;
                }
            }
            return false;

        case  LT(KC_MS_WH_LEFT,KC_MS_WH_RIGHT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD:
                    tap_code(KC_MS_WH_LEFT);
                    return false;
                } else {
                    // TAP:
                    tap_code(KC_MS_WH_RIGHT);
                    return false;
                }
            }
            return false;

        case LT(_MOVE_V, PASTE):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Activar capa _MOVE_V
                    return true; // QMK maneja el HOLD automáticamente
                } else {
                    // TAP: Ctrl+V para pegar
                    tap_code16(C(KC_V));
                    return false;
                }
            }
            return true;

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


	if (is_alt_tab_active && timer_elapsed(alt_tab_timer) > 3000)   {
           unregister_code(KC_LALT);
           unregister_code(KC_TAB);
           is_alt_tab_active = false;

   }

 }


// ============================================================================
// TAP DANCE ACTIONS
// ============================================================================

tap_dance_action_t tap_dance_actions[] = {
    [TDQ_SEL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_sel_finished, x_reset),
    [TDQ_GOTO] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_goto_finished, x_reset),
    [TDQ_BOOKMARK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_bookmark_finished, x_reset),
    [TDQ_FIND] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_find_finished, x_reset),
    [TDQ_REPLACE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_replace_finished, x_reset),
    [TDQ_OVERRIDE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_override_finished, x_reset),
};

// ============================================================================
// KEYMAPS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    //_BASE Layer LT(ALT_SHIFT,_MOUSE_1) LT(_DEV,SHIFT_TOGGLE)
    [_BASE] = LAYOUT_split_3x6_3(
// ,-------------------------------------------------------------------------------------.                                    ,-----------------------------------------------------.
LT(KC_F4, CLOSE_WIN), LT(_SYMB,KC_ESC), ALT_TAB, LT(_NUMB,KC_TAB), LT(SEL_ALL,KC_SPACE), QK_BOOT,                              QK_BOOT, XXXXXXX, TD(TDQ_SEL), LT(_RUN,KC_HOME), LT(_SYMB,KC_END), XXXXXXX,
// |--------+--------+--------+--------+--------+----------------------------------------|                                    |--------+--------+--------+--------+--------+--------|
LT(_AI,_MOUSE_2), LT(_DEL,TG_0), LT(_MOVE_H,COPY), LT(_MOVE_V,PASTE), PASTE, LT(KC_S,SHIFT_2), LT(KC_S, SHIFT_2),          SLEEP, LT(_BOOK,KC_LEFT), LT(_MOVE_WIN,KC_DOWN), LT(_MOVE_L,KC_RIGHT), LT(_AI,KC_UP),
// |--------+--------+--------+--------+--------+----------------------------------------|                                    |--------+--------+--------+--------+--------+--------|
TG(_ALFA), LT(CUT,COPY), KC_F24, MO(_BOOK), WIN_D, KC_LALT,                                                                    HIBERNATE, XXXXXXX, TG(_MODE), LT(0,SHOW_QUICK_ENT), LT(0,CODE_COMPLET), KC_INS,
// |--------+--------+--------+--------+--------+--------+-------------------------------|                                    |--------+--------+--------+--------+--------+--------+--------|
                                    				   LT(_MOVE_WIN,KC_ENT), C(KC_Z), LT(0,CTL_GUI),                           TO(_BASE), XXXXXXX, LT(_DEV,KC_SPACE)
                                                        // `----------------------------------'                                `---------------------------------'
    ),


    //_ALFA ly 1
    [_ALFA] = LAYOUT_split_3x6_3(
// ,--------------------------------------------------------.                                ,-----------------------------------------------------.
LT(KC_F4, CLOSE_WIN), LT(0,ESC_W), LT(0,T_F), LT(_NUMB,KC_TAB), XXXXXXX, XXXXXXX,             XXXXXXX, XXXXXXX, LT(0,H_J), LT(0,D_Q), LT(0,L_K), XXXXXXX,
// |--------+--------+--------+--------+--------+-----------|                                |--------+--------+--------+--------+--------+--------|
LT(_AI,KC_A), LT(_DEL,TG_0), LT(_SYMB,KC_E), LT(_BASE,KC_I), KC_F24, LT(KC_S, SHIFT_2),      XXXXXXX,  KC_CAPS, KC_O, LT(_SYMB,KC_S), KC_R, KC_N,
// |--------+--------+--------+--------+--------+-----------|                                |--------+--------+--------+--------+--------+--------|
TG_ALFA, LT(0,G_Z), LT(0,C_X), LT(0,V_B), WIN_D, KC_LALT  ,                                   HIBERNATE, XXXXXXX,  KC_U, LT(0,M_Y), LT(0,P_ENIE), KC_TAB,
// |--------+--------+--------+--------+--------+-----------|                                |--------+--------+--------+--------+--------+--------+--------|
                         LSFT_T(KC_ENT), C(KC_Z),  LT(0,CTL_GUI),                     		  TO(_BASE),  KC_CAPS,  RSFT_T(KC_SPACE)
                         // `--------------------------------'                                `--------------------------'
    ),

    //_DEV Ly 2
    [_DEV] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, LT(0,MULTICURSOR), A(KC_J), S(A(KC_J)), LT(0,FOLDING), XXXXXXX,             XXXXXXX, C(S(KC_U)), A(KC_F12), LT(0,PROJECT_VIEW), LT(0,NEW_FILE), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           C(KC_D), LT(0,COMM), TD(TDQ_FIND), LT(0,REFACTOR), LT(0,INFOPARM), XXXXXXX,          XXXXXXX, LT(0,NAV_ERROR), TD(TDQ_GOTO), C(A(KC_LEFT)), C(A(KC_RIGHT)), C(S(KC_F12)),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           A(KC_Q), C(A(KC_T)), TD(TDQ_REPLACE), TD(TDQ_OVERRIDE), XXXXXXX, QK_BOOT,            QK_BOOT, XXXXXXX, LT(0,USAGES), LT(0,LAST_EDIT), C(KC_F12), LT(0,RECENT_LOC),
        // |--------+--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------+--------|
                                      LT(0,FULL_SCREEN), XXXXXXX, XXXXXXX,                      TO(_BASE), XXXXXXX, LT(0,EVERYW_ACT)
                                      // `------------------------------'                        `--------------------------'
    ),

    //_DEL Ly 3
    [_DEL] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, C(A(KC_L)), C(S(KC_J)), C(A(KC_I)), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, TO(_BASE), XXXXXXX, XXXXXXX, XXXXXXX,                              XXXXXXX, XXXXXXX, KC_DEL, KC_BSPC , DEL_WORD,  DEL_LINE,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, C(A(KC_O)), XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                    _______, _______,  _______,                                 TO(_BASE),   XXXXXXX, C(KC_Z)
                                    // `----------------------'                                  `--------------------------'
    ),

    // _SYMB Ly 4
    [_SYMB] = LAYOUT_split_3x6_3(
        // ,---------------------------------------------------------------------.                ,-----------------------------------------------------.
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
                                      KC_ENT, XXXXXXX, XXXXXXX,                                 TO(_BASE), TG(_NUMB), KC_SPACE
                                     // `---------------------------------'                      `--------------------------'
    ),

    // _BOOK Ly 6
    [_BOOK] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------------.                     ,-----------------------------------------------------.
           XXXXXXX, C(KC_F17), C(KC_F18), C(KC_F19), XXXXXXX, XXXXXXX,                        XXXXXXX, XXXXXXX, C(KC_5), C(KC_6), C(KC_7), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------------|                     |--------+--------+--------+--------+--------+--------|
           C(KC_F13), C(KC_F14), C(KC_F15), C(KC_F16), XXXXXXX , XXXXXXX,                     XXXXXXX, XXXXXXX, C(KC_1), LT(_BOOK_2,MARKER_2), C(KC_3), C(KC_4),
        // |--------+--------+--------+--------+--------+--------------|                     |--------+--------+--------+--------+--------+--------|
           XXXXXXX, C(KC_F20), LT(_BOOK_2,MARKER_B), C(KC_F22), XXXXXXX, XXXXXXX,             XXXXXXX, XXXXXXX, C(KC_8), C(KC_9), TD(TDQ_BOOKMARK), XXXXXXX,
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
           XXXXXXX, A(KC_H), A(KC_I), A(KC_K), XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, C(S(KC_8)), C(S(KC_9)), XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     XXXXXXX, XXXXXXX,  _______,                               TO(_BASE),   XXXXXXX, XXXXXXX
                                     // `------------------------'                              `--------------------------'
    ),

    // _MOVE_H Ly 8
    [_MOVE_H] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX , XXXXXXX, XXXXXXX,                              XXXXXXX, XXXXXXX, XXXXXXX, C(S(KC_M)), KC_F20, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, C(S(KC_ENT)), XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, LT(0, CLEFT_10), CLEFT_5, LT(0, CRIGHT_10), CRIGHT_5,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, MS_WHLL, MS_WHLR, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     XXXXXXX, XXXXXXX, _______,                                TO(_BASE), XXXXXXX, LT(0,C_END_HOME)
                                     // `------------------------'                              `--------------------------'
    ),

    // _MOVE_V Ly 9
    [_MOVE_V] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, UP_10, DOWN_10, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, G(KC_V), C(S(KC_V)), XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, LT(0,PAGE_PARAGRAPH_UP), A(KC_UP), LT(0,PAGE_PARAGRAPH_DOWN), A(KC_DOWN),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, MS_WHLU, MS_WHLD, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                       XXXXXXX, XXXXXXX, XXXXXXX,                               TO(_BASE), XXXXXXX, LT(0, PGDN_PGUP)
                                       // `----------------------'                              `--------------------------'
    ),

    // _MOVE_WIN Ly 10
    [_MOVE_WIN] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, C(KC_T), C(KC_L), LT(0,SPLIT_WIN), XXXXXXX, XXXXXXX,                               XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           C(S(KC_TAB)), A(KC_LEFT), C(KC_TAB), A(KC_RIGHT), KC_F5, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, MO(_MOVE_L), XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
        C(S(KC_F12)), A(KC_LEFT), A(KC_RIGHT), C(KC_F), XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     C(KC_F4), _______,  _______,                               TO(_BASE), XXXXXXX,XXXXXXX
                                     // `------------------------'                               `--------------------------'
    ),


    // _MOVE_L Ly 11
    [_MOVE_L] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           A(S(KC_UP)), C(S(KC_DOWN)), A(S(KC_DOWN)), C(S(KC_UP)), XXXXXXX , XXXXXXX,           XXXXXXX, XXXXXXX , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, RM_SATD, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     XXXXXXX, XXXXXXX,  _______,                                TO(_BASE),   XXXXXXX, XXXXXXX
                                     // `------------------------'                               `--------------------------'
    ),

    // _RUN  Ly 12
    [_RUN] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           A(S(KC_F10)), C(KC_F5), C(S(KC_F10)), S(KC_F10), XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                        C(KC_F2), _______,  _______,                             TO(_BASE), XXXXXXX, XXXXXXX
                                        // `----------------------'                              `--------------------------'
    ),

    // _MODE Ly 13
    [_MODE] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, TG(_COMMIT), TO_NUMB, XXXXXXX, XXXXXXX,                          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, TG(_MODE), XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                         XXXXXXX, XXXXXXX, XXXXXXX,                             TO(_BASE), XXXXXXX, TG(_MODE)
                                         // `---------------------'                             `--------------------------'
    ),

    // _COMMIT Ly 14     MS_ACL0_TOGGLE
    [_COMMIT] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, KC_ESC, C(KC_K), A(KC_0), XXXXXXX, XXXXXXX,                                 XXXXXXX, XXXXXXX, XXXXXXX, C(KC_Z), XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           MS_ACL2, MS_ACL1, S(KC_F7), KC_F7, XXXXXXX, C(KC_S),                                 SLEEP, XXXXXXX, MS_WHLL, MS_WHLD, MS_WHLR, MS_WHLU,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, MS_ACL0, XXXXXXX, WIN_D,                                  HIBERNATE, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     C(A(KC_R)),  XXXXXXX, XXXXXXX,                             TO(_BASE), XXXXXXX, TO(_BASE)
                                     // `-------------------------'                             `--------------------------'
    ),

    // _AI Ly 15
    [_AI] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, KC_HASH, LT(0,KC_SLSH), KC_AT, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, S(KC_TAB), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, S(KC_TAB), XXXXXXX,XXXXXXX, C(S(KC_L)),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, C(KC_L), C(KC_I), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                       XXXXXXX, XXXXXXX, XXXXXXX,                               XXXXXXX, XXXXXXX, KC_TAB
                                       // `---------------------'                               `--------------------------'
    ),

    // _MOUSE_1 Ly 16 not used
    [_MOUSE_1] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                                     ,-----------------------------------------------------.
        LT(KC_F4, CLOSE_WIN), KC_ESC, ALT_TAB, XXXXXXX, XXXXXXX, XXXXXXX,                                            XXXXXXX, XXXXXXX, XXXXXXX, KC_MS_WH_UP, KC_MS_WH_DOWN, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                                     |--------+--------+--------+--------+--------+--------|
        TO(_BASE), KC_BTN1, LT(KC_MS_ACCEL0, COPY), LT(KC_MS_ACCEL2,PASTE), XXXXXXX, XXXXXXX,           XXXXXXX, KC_BTN2, KC_MS_L, KC_MS_D , KC_MS_R, KC_MS_U,
        // |--------+--------+--------+--------+--------+--------|                                     |--------+--------+--------+--------+--------+--------|
        LT(KC_MS_WH_UP,KC_MS_WH_DOWN), LT(0,PGDN_PGUP), G(KC_T), C(KC_Z), XXXXXXX, KC_LALT  ,             XXXXXXX, XXXXXXX, KC_MS_WH_LEFT, KC_MS_WH_RIGHT, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                                     |--------+--------+--------+--------+--------+--------+--------|
                                        LT(KC_SPC, KC_ENT), C(KC_Z), LT(0,CTL_GUI),                          XXXXXXX, KC_BTN2, KC_SPC
                                       // `---------------------'                                       `--------------------------'
    ),

    // _MOUSE_2 Ly 17 LCTL_T(KC_F3) LT(KC_MS_WH_LEFT,KC_MS_WH_RIGHT) LT(KC_F24, F23_F24) KC_BTN1 C(KC_X)
    [_MOUSE_2] = LAYOUT_split_3x6_3(
    // ,-----------------------------------------------------.                                             ,-----------------------------------------------------.
    LT(KC_F4, CLOSE_WIN),  LT(_SYMB,KC_ESC), ALT_TAB, LT(_NUMB,KC_TAB), LT(SEL_ALL,KC_SPACE), KC_F15,                        XXXXXXX, XXXXXXX, TD(TDQ_SEL), KC_MS_WH_LEFT, KC_MS_WH_RIGHT, XXXXXXX,
    // |--------+--------+--------+--------+--------+--------|                                             |--------+--------+--------+--------+--------+--------|
    LT(KC_F22, TG_0), LT(KC_F22, TG_F22), KC_MS_D, KC_MS_U, PASTE, LT(KC_S, SHIFT_2),                            SLEEP, LT(KC_MS_WH_LEFT,KC_MS_WH_RIGHT), KC_MS_L, KC_MS_R, KC_MS_WH_DOWN, KC_MS_WH_UP,
    // |--------+--------+--------+--------+--------+--------|                                             |--------+--------+--------+--------+--------+--------|
    TG_ALFA, LT(CUT,COPY), KC_F24, KC_BTN1, WIN_D, KC_LALT  ,                                             HIBERNATE, XXXXXXX, XXXXXXX, XXXXXXX, LT(0,PGDN_PGUP) , XXXXXXX,
    // |--------+--------+--------+--------+--------+--------|                                             |--------+--------+--------+--------+--------+--------+--------|
                                        LT(_MOVE_WIN, ENT2), C(KC_Z), LT(0,CTL_GUI),                             TO(_BASE), KC_BTN2, KC_SPC
                                       // `---------------------'                                           `--------------------------'
    ),






};



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
                    //seleccionar palabra
                    tap_code16_delay(C(KC_LEFT), 10);
                    tap_code16_delay(C(S(KC_RIGHT)), 10);
        break;

        case TD_SINGLE_HOLD: //
                 //sel 1 line
                tap_code_delay(KC_HOME, 10);
                tap_code_delay(KC_HOME, 10);
                tap_code16_delay(S(KC_END), 10);
                //tap_code16_delay(C(KC_C), 10);

         break;

        case TD_DOUBLE_TAP:
                    // HOLD: seleccionar párrafo
                    tap_code16_delay(KC_END, 10);
                    tap_code16_delay(KC_HOME, 10);
                    tap_code16_delay(KC_HOME, 30);
                    tap_code16(S(A(KC_PGDN))); // Shift + Alt + PgDn

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

void tdq_goto_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:  tap_code16(C(KC_B)); break; //go to definition
        case TD_SINGLE_HOLD: tap_code16(C(A(KC_B))); break;//go to implemetation
        case TD_DOUBLE_TAP:  tap_code16(C(S(KC_B))); break;//Go to Type Declaration
        case TD_DOUBLE_HOLD: tap_code16(C(S(KC_T))); break;//go to test
        case TD_TRIPLE_HOLD: tap_code16(C(KC_U));  break;//Go to Super Method / Class

        default: break;
    }
}


void tdq_find_finished(tap_dance_state_t *state, void *user_data) {
        xtap_state.state = cur_dance(state);

    switch (xtap_state.state) {
        case TD_SINGLE_TAP: //ctrl f //find normal.
            tap_code16(C(KC_F));
            break;

        case TD_SINGLE_HOLD: //ctrl f + ctrl alt e //search on selection
                tap_code16_delay(LCTL(KC_F), 10);  // Ctrl + F
                tap_code16(LCTL(LALT(KC_E)));       // Ctrl + Alt + E
            break;

        case TD_DOUBLE_TAP: //find on 1 word
                tap_code16_delay(C(KC_LEFT), 10);
                tap_code16_delay(C(S(KC_RIGHT)), 10);
                tap_code16_delay(C(KC_F), 10);
            break;

        case TD_DOUBLE_HOLD: //ctrl shift f //find in files
                tap_code16_delay(C(KC_LEFT), 10);
                tap_code16_delay(C(S(KC_RIGHT)), 10);
                tap_code16(LCTL(LSFT(KC_F)));
            break;
        default:
            break;
    }
}

void tdq_replace_finished(tap_dance_state_t *state, void *user_data) {
        xtap_state.state = cur_dance(state);

    switch (xtap_state.state) {
        case TD_SINGLE_TAP: //ctrl R //replace normal.
            tap_code16(C(KC_R));
            break;

        /*case TD_SINGLE_HOLD: //ctrl R + ctrl alt e //search on selection
                tap_code16_delay(LCTL(KC_R), 10);  // Ctrl + R
                tap_code16(LCTL(LALT(KC_E)));       // Ctrl + Alt + E
            break;*/

        case TD_DOUBLE_TAP: //replace on 1 word
                tap_code16_delay(C(KC_LEFT), 10);
                tap_code16_delay(C(S(KC_RIGHT)), 10);
                tap_code16_delay(C(KC_R), 10);
            break;

        case TD_DOUBLE_HOLD: //ctrl shift R //replace in files
                tap_code16_delay(C(KC_LEFT), 10);
                tap_code16_delay(C(S(KC_RIGHT)), 10);
                tap_code16(LCTL(LSFT(KC_R)));
            break;
        default:
            break;
    }
}

void tdq_override_finished(tap_dance_state_t *state, void *user_data) {
        xtap_state.state = cur_dance(state);

    switch (xtap_state.state) {

        case TD_SINGLE_TAP: //implement interface
                tap_code16(LCTL(KC_I));
            break;

        case TD_SINGLE_HOLD: //override
            tap_code16(LCTL(KC_O));
            break;

/*
        case TD_DOUBLE_TAP://create test
            tap_code16(C(S(KC_T)));
            break;
*/

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
//bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
//
// switch (get_highest_layer(layer_state | default_layer_state)) {
//         case 2:
//             rgb_matrix_set_color(8, RGB_GREEN);  // R, G, B
//             break;
//         case 5:
//            rgb_matrix_set_color(8, RGB_WHITE); //gris suave
//             break;
//         case 6:
//             rgb_matrix_set_color(8, RGB_ORANGE);
//             break;
//
//         default:
//             //apagar todos los LEDs
//             rgb_matrix_set_color_all(0, 0, 0);
//             break;
//     }
//     return false;
//}


//RGB LIGHT


//capslock
//{0, 0, HSV_OFF} {starting, numbers_leds, HSV_OFF}
const rgblight_segment_t PROGMEM my_capslock_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 0, HSV_OFF}
);

// _alfa_layer ly1
const rgblight_segment_t PROGMEM _alfa_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {9,1, HSV_GREEN}
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

// _mouse_1_layer ly16
const rgblight_segment_t PROGMEM _mouse_1_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_YELLOW}
);

// _mouse_2_layer ly17
const rgblight_segment_t PROGMEM _mouse_2_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_BLUE}
);

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    NULL,               // 0
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
    _mouse_1_layer,     // 16
    _mouse_2_layer      // 17
);

void keyboard_post_init_user(void) {
    // Enable the LED layers
//    debug_enable=true;
//    debug_keyboard=true;
    rgblight_sethsv_noeeprom(0, 0, 0); // Blanco puro//para apagar la primera capa, que no alumbren todos los leds
    rgblight_layers = my_rgb_layers;
}





layer_state_t layer_state_set_user(layer_state_t state) {
//    rgblight_set_layer_state(INDEX_LIGHT, layer_state_cmp(state, _LAYER));
//    rgblight_set_layer_state(5, layer_state_cmp(state, 5));

    rgblight_set_layer_state(1, false);  // _ALFA LY OFF
    rgblight_set_layer_state(5, false);  // _NUMB LY OFF
    rgblight_set_layer_state(13, false); // _MODE LY OFF
    rgblight_set_layer_state(14, false); // _COMMIT LY OFF
    rgblight_set_layer_state(16, false); // _MOUSE_1 LY OFF
    rgblight_set_layer_state(17, false); // _MOUSE_1 LY OFF


uint8_t layer = get_highest_layer(state);

        switch (layer) {
            case _ALFA:
                 rgblight_set_layer_state(1, true); // ALFA LY
                 send_layer_status("LAYER_ALFA");
                break;

            case _NUMB:
                 rgblight_set_layer_state(5, true); // NUMBERS LY
                 send_layer_status("LAYER_NUMB");
                break;

            case _MODE:
                rgblight_set_layer_state(13, true); // MODE   LY
                send_layer_status("LAYER_MODE");
                break;

            case _COMMIT:
                rgblight_set_layer_state(14, true); // COMMIT LY
                send_layer_status("LAYER_COMMIT");
                break;

      		 case _MOUSE_1:
                rgblight_set_layer_state(16, true); // MOUSE_1 LY
                send_layer_status("LAYER_MOUSE_1");
                break;

      		 case _MOUSE_2:
                rgblight_set_layer_state(17, true); // MOUSE_2 LY
                send_layer_status("LAYER_MOUSE_2");
                break;

            default:
               send_layer_status("LAYER_BASE");
               break;
        }
    return state;
}
