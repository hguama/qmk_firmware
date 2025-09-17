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
    _AI            // 15
};


//Macro enum
enum custom_keycodes {
    GUI_E = SAFE_RANGE,
    ARROW_CTRL_LEFT,
    ARROW_CTRL_RIGHT,
    CTRLW_L4,
    SPACE_BASE,
    ALT_TAB,
    AMP_DOUBLE,
    OPEN_EXCL,
    OPEN_QUEST,
    NOT_EQUAL,
    MS_ACL0_TOGGLE,
    DEV_LY_SPACE,
    Z_UNDO,
    TRIPLE_WHLD,
    HASH_CIRC,
    EQUAL_DBL,
    DOUBLE_PIPE,
    VOICE_A,
    VOICE,
    CHATGPT,
    SEL_WORD_PARAGRAPH,
    M_ALT_TAB,
    F_W,
    B_V,
    N_ENIE,
    DEL_WORD,
    DEL_LINE,
    WIN_D,
    LLAMBDA,
    RLAMBDA,
    DOUBLE_COLON,
    LBRC2,
    HOME_END,
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
    MARKER_3,
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
    BASE_LAYER_F,    // Tecla personalizada para ir a capa _BASE momentáneamente
    ESC_W,           // Tecla personalizada TAP: ESC | HOLD: W
    TAB_F,           // Tecla personalizada TAP: TAB | HOLD: F
    H_J,             // Tecla personalizada TAP: H | HOLD: J
    D_Q,             // Tecla personalizada TAP: D | HOLD: Q
    L_K,             // Tecla personalizada TAP: L | HOLD: K
    G_Z,             // Tecla personalizada TAP: G | HOLD: Z
    C_X,             // Tecla personalizada TAP: C | HOLD: X
    P_ENIE,          // Tecla personalizada TAP: P | HOLD: Ñ
};

//Combo enum
enum combo_events {
  CB_CTRL_Z,   // identificador del combo
};

//Tap Dance enum
enum {
    TDQ_COPY,
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
static bool tdq_copy_hold_active = false;
static bool tdq_paste_hold_active = false;

// para LT(_DEL, KC_PERC)
static uint8_t del_prev_layer = _BASE;
static bool del_layer_active = false;

// para BASE_LAYER_F
static uint8_t base_prev_layer = _ALFA;
static bool base_layer_active = false;

bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;

static bool shift_active = false;

// para LEFT
static uint16_t timer_key_left;
static bool is_hold_left = false;
static bool is_pressed_left = false;

// para RIGHT
static uint16_t timer_key_right;
static bool is_hold_right = false;
static bool is_pressed_right = false;

static bool space_base_double_tap = false;

bool ms_acl0_active = false;

bool voice_mode = false;

static bool defer_copy = false;
static bool defer_cut = false;
static uint16_t defer_timer_copy = 0;
static uint16_t defer_timer_cut = 0;


// Prototypes quad
td_state_t cur_dance(tap_dance_state_t *state);
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);
void tdq_copy_reset(tap_dance_state_t *state, void *user_data);
void tdq_paste_reset(tap_dance_state_t *state, void *user_data);

void tdq_copy_finished(tap_dance_state_t *state, void *user_data);
void tdq_paste_finished(tap_dance_state_t *state, void *user_data);
void tdq_cut_finished(tap_dance_state_t *state, void *user_data);

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
const uint16_t PROGMEM cb_ctrl_z[] = {KC_J, LT(0,KC_K), COMBO_END};

//combo actions
combo_t key_combos[] = {
 [CB_CTRL_Z]   = COMBO(cb_ctrl_z, LCTL(KC_Z)),
 };

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

// ============================================================================
// PROCESS RECORD USER FUNCTION
// ============================================================================

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ARROW_CTRL_LEFT:
            if (record->event.pressed) {
                timer_key_left = timer_read();
                is_hold_left = false;
                is_pressed_left = true;
            } else {
                is_pressed_left = false;
                if (!is_hold_left) {
                    // TAP: flecha izquierda
                    tap_code(KC_LEFT);
                }
            }
            return false;

        case ARROW_CTRL_RIGHT:
            if (record->event.pressed) {
                timer_key_right = timer_read();
                is_hold_right = false;
                is_pressed_right = true;
            } else {
                is_pressed_right = false;
                if (!is_hold_right) {
                    // TAP: flecha derecha
                    tap_code(KC_RGHT);
                }
            }
            return false;
        case LT(4, CTRLW_L4):
            if (record->event.pressed) {
                if (record->tap.count) {
                    tap_code16(C(KC_W));  // TAP: Ctrl+W
                }
            }
            return true;

        case LT(0,SPACE_BASE):
            if (record->event.pressed) {
                if (record->tap.count == 2) {
                    // DOBLE TAP: activar espacio sostenido
                    space_base_double_tap = true;
                    register_code(KC_SPC);
                    return false;
                }
                if (record->tap.count == 0) {
                    // HOLD sin taps: cambiar capa base
                    layer_invert(_ALFA);
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple: un espacio
                    tap_code(KC_SPC);
                    return false;
                }
            } else {
                // Al soltar, cortar espacio sostenido si estaba en doble tap
                if (space_base_double_tap) {
                    unregister_code(KC_SPC);
                    space_base_double_tap = false;
                }
            }
            return true;

        case LT(_NUMB, ALT_TAB):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    return true; // HOLD: QMK activa capa 4
                } else {
                    tap_code16(A(KC_TAB)); // TAP: Alt+Tab
                    return false; // No enviar el KC original
                }
            }
            return true;

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

        case LT(3, KC_SPACE):
            if (record->event.pressed) {
                if (record->tap.count == 2) {
                    // DOBLE TAP: activar espacio sostenido
                    space_base_double_tap = true;
                    register_code(KC_SPC);
                    return false;
                }
                if (record->tap.count == 0) {
                    // HOLD: activar momentáneamente capa 3
                    return true;
                }
                if (record->tap.count == 1) {
                    // TAP simple: soltar Alt si está activo y enviar Espacio
                    if (get_mods() & MOD_MASK_ALT) {
                        unregister_mods(MOD_MASK_ALT);
                        is_alt_tab_active = false;
                    }
                    tap_code(KC_SPC);
                    return false;
                }
            } else {
                // Al soltar tecla
                if (space_base_double_tap) {
                    // cortar espacio sostenido
                    unregister_code(KC_SPC);
                    space_base_double_tap = false;
                }
            }
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


        /*
        case LT(1, KC_RIGHT):
            if (record->event.pressed) {
                if (record->tap.count == 2) {
                    // DOBLE TAP + (mantener): repetir en ciclos de 5
                    sr_repeat = true;
                    timer_key  = timer_read();
                    return false;
                }
                if (record->tap.count == 0) {
                    // HOLD: 10 flechas inmediatas
                    for (int i = 0; i < 40; i++) {
                        tap_code(KC_RGHT);
                    }
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple: 1 flecha
                    tap_code(KC_RGHT);
                    return false;
                }
            } else {
                // Al soltar, detener el ciclo (si estaba activo)
                if (sr_repeat) {
                    sr_repeat = false;
                }
            }
            return true;
        */

        case LT(0,HASH_CIRC):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(KC_CIRC);  // HOLD: ^
                    return false;
                } else {
                    tap_code16(KC_HASH);  // TAP: #
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

        case LT(_BOOK_2, MARKER_3):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: MO(_BOOK_2)
                    return true; // Permite que QMK maneje el layer tap
                } else {
                    // TAP: C(KC_3)
                    tap_code16(C(KC_3));
                    return false;
                }
            }
            return true;

        case LT(0, CRIGHT_10):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: KC_RIGHT x10
                    for (int i = 0; i < 10; i++) {
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
                    for (int i = 0; i < 10; i++) {
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
                    tap_code(KC_RIGHT);
                }
            }
            return false;

        case CLEFT_5:
            if (record->event.pressed) {
                // KC_LEFT 5 veces
                for (int i = 0; i < 5; i++) {
                    tap_code(KC_LEFT);
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


        case M_ALT_TAB:
            if (record->event.pressed) {
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                    register_code(KC_LALT);
                }
                register_code(KC_TAB);
            } else {
                unregister_code(KC_TAB);
            }
            break;

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

        case LT(0,KC_AT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code16(KC_QUES); // ? hold
                    return false;
                } else {
                    tap_code16(KC_AT);  // @ tap
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

        case LT(3,KC_ENT):
            if (record->event.pressed) {
                clear_all();
            }
            return true;

        case LT(0,HOME_END):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_HOME); //HOLD
                    return false;
                } else {
                    tap_code(KC_END); //TAP
                    return false;
                }
            }
            return false;

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

        case LT(0,N_ENIE):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_KP_1) SS_TAP(X_KP_6) SS_TAP(X_KP_4) SS_UP(X_LALT));
                    return false;
                } else {
                    tap_code(KC_N);
                    return false;
                }
            }
            return false;

        case LT(0,B_V):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_V);
                    return false;
                } else {
                    tap_code(KC_B);
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

        case OPEN_EXCL: // ¡
            if (record->event.pressed) {
                SEND_STRING(
                    SS_DOWN(X_LALT)
                    SS_TAP(X_KP_0)
                    SS_TAP(X_KP_1)
                    SS_TAP(X_KP_6)
                    SS_TAP(X_KP_1)
                    SS_UP(X_LALT)
                );
            }
            break;

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
                    } else {
                        unregister_code(KC_LSFT); // Desactiva Shift
                    }
                    return false;
                }
            }
            return true;

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

        case LT(_DEL,KC_F):
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
                    tap_code(KC_F);
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

        case LT(0,TAB_F):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    tap_code(KC_F); //HOLD
                    return false;
                } else {
                    tap_code(KC_TAB); //TAP
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

        case LT(0,P_ENIE):
            if (record->event.pressed) {
                if (!record->tap.count) {
                    // HOLD: Ñ usando código Alt+164 para teclado inglés
                    register_code(KC_LALT);
                    tap_code(KC_KP_1);
                    tap_code(KC_KP_6);
                    tap_code(KC_KP_4);
                    unregister_code(KC_LALT);
                    return false;
                } else {
                    tap_code(KC_P); //TAP
                    return false;
                }
            }
            return false;

        }//END SWITCH
    return true;
};
//Pr record END

//timer for  macro
void matrix_scan_user(void) {

//static uint16_t rpt_timer;

    // LEFT
    if (is_pressed_left) {
        if (!is_hold_left && timer_elapsed(timer_key_left) > 200) {
            is_hold_left = true;
            timer_key_left = timer_read();
        }
        if (is_hold_left && timer_elapsed(timer_key_left) > 80) {
            tap_code16(C(KC_LEFT));
            timer_key_left = timer_read();
        }
    }

    // RIGHT
    if (is_pressed_right) {
        if (!is_hold_right && timer_elapsed(timer_key_right) > 200) {
            is_hold_right = true;
            timer_key_right = timer_read();
        }
        if (is_hold_right && timer_elapsed(timer_key_right) > 80) {
            tap_code16(C(KC_RGHT));
            timer_key_right = timer_read();
        }
    }


    if (defer_copy && timer_elapsed(defer_timer_copy) > 2) {
        defer_copy = false;
        clear_mods();  // elimina cualquier modificador que haya sobrevivido
        tap_code16(C(KC_C));
     }

    if (defer_cut && timer_elapsed(defer_timer_cut) > 50) {
        defer_cut = false;
        clear_mods();  // elimina cualquier modificador que haya sobrevivido
        tap_code16(C(KC_X));
     }


/*
    if (sr_repeat) {
        if (timer_elapsed(timer_key) > 130) {
            for (int i = 0; i < 5; i++) {
                tap_code(KC_RGHT);
            }
            timer_key = timer_read();
        }
    }*/


      /*
    if (shift_active) {
        if (timer_elapsed(shift_toggle_timer) > 30000) {
            unregister_code(KC_LSFT);
            shift_active = false;
        }
    }
    */

 }


// ============================================================================
// TAP DANCE ACTIONS
// ============================================================================

tap_dance_action_t tap_dance_actions[] = {
    [TDQ_COPY] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_copy_finished, tdq_copy_reset),
    [TDQ_PASTE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_paste_finished, tdq_paste_reset),
    [TDQ_CUT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_cut_finished, x_reset),
    [TDQ_GOTO] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_goto_finished, x_reset),
    [TDQ_BOOKMARK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_bookmark_finished, x_reset),
    [TDQ_FIND] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_find_finished, x_reset),
    [TDQ_REPLACE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_replace_finished, x_reset),
    [TDQ_OVERRIDE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_override_finished, x_reset),
};

// ============================================================================
// KEYMAPS
// ============================================================================

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Base Layer ARROW_CTRL_RIGHT ARROW_CTRL_LEFT
    [_BASE] = LAYOUT_split_3x6_3(
        // ,-------------------------------------------------------------------------------------.      ,-----------------------------------------------------.
           KC_ESC, LT(_SYMB,TG_0), M_ALT_TAB, LT(_NUMB,ALT_TAB), TD(TDQ_CUT), QK_BOOT,                   QK_BOOT, XXXXXXX, LT(0,SEL_WORD_PARAGRAPH), LT(_RUN,KC_UP), LT(_SYMB,KC_TAB), XXXXXXX,
        // |--------+--------+--------+--------+--------+----------------------------------------|      |--------+--------+--------+--------+--------+--------|
           LT(_AI,SHIFT_TOGGLE), LT(_DEL,TG_0), TD(TDQ_COPY), TD(TDQ_PASTE), C(KC_Z), C(KC_S),                 SLEEP, C(KC_A), LT(_BOOK, KC_LEFT), LT(_MOVE_WIN, KC_DOWN), LT(_MOVE_L, KC_RIGHT), LT(0,HOME_END),
        // |--------+--------+--------+--------+--------+----------------------------------------|      |--------+--------+--------+--------+--------+--------|
           LT(0,VOICE), LT(0,MOUSE_PRESSED_CLICK), LT(_NUMB,KC_F3), MO(_BOOK), LT(0,TG_6), WIN_D,        HIBERNATE, XXXXXXX, MO(_MODE), LT(0,SHOW_QUICK_ENT), LT(0,CODE_COMPLET), KC_INS,
        // |--------+--------+--------+--------+--------+--------+-------------------------------|      |--------+--------+--------+--------+--------+--------+--------|
                                     LT(_DEV,KC_SPACE), SHIFT_TOGGLE, KC_LALT,                           TO(_BASE), XXXXXXX, LT(_DEV,KC_ENT)
                                     // `------------------------------------'                          `---------------------------------'
    ),


    //Alfa ly1
    [_ALFA] = LAYOUT_split_3x6_3(
        // ,--------------------------------------------------------.                           ,-----------------------------------------------------.
           KC_ESC, LT(_SYMB,KC_Q), LT(0,GUI_E), KC_R, KC_T, KC_W,                            XXXXXXX, KC_Y, KC_U, KC_I, LT(_SYMB,KC_O), KC_TRNS,
        // |--------+--------+--------+--------+--------+-----------|                           |--------+--------+--------+--------+--------+--------|
           LT(_DEL,KC_A), LT(_DEL,KC_S), KC_D, LT(_DEL,KC_F), KC_G , C(KC_S),                                 SLEEP,  KC_H, KC_J, KC_K, KC_L, KC_P,
        // |--------+--------+--------+--------+--------+-----------|                           |--------+--------+--------+--------+--------+--------|
           KC_Z, KC_X, LT(_NUMB,KC_C), LT(0,B_V), XXXXXXX, WIN_D,                                HIBERNATE, XXXXXXX,  KC_M, LT(0,CODE_COMPLET), KC_BSPC, LT(0,N_ENIE),
        // |--------+--------+--------+--------+--------+-----------|                           |--------+--------+--------+--------+--------+--------+--------|
                                 LT(0,SPACE_BASE), LSFT_T(KC_CAPS), XXXXXXX,                     TO(_BASE),  TG(_NUMB),  RSFT_T(KC_ENT)
                                 // `--------------------------------------'                     `--------------------------'
    ),

    // Dev Layer (posición 2 según enum) LT(0, MAX_MIN_WIN)
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

    // Super DEL Layer (posición 3 según enum)
    [_DEL] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, C(A(KC_L)), C(S(KC_J)), C(A(KC_I)), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, TO(_BASE), XXXXXXX, XXXXXXX, XXXXXXX,                              XXXXXXX, XXXXXXX, KC_DEL, KC_BSPC , DEL_WORD,  DEL_LINE,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, C(A(KC_O)), XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                    C(KC_Z), _______,  _______,                                 TO(_BASE),   XXXXXXX, XXXXXXX
                                    // `----------------------'                                  `--------------------------'
    ),

    //Symbols ly2 (posición 4 según enum)

//para eliminar
// LT(0,HASH_CIRC)
// LT(0,KC_AT)
//rlambda
// OPEN_EXCL
// tap_code16(KC_CIRC);  // HOLD: ^
// tap_code16(KC_HASH);  // TAP: #
// KC_QUOT '


// [_SYMB]
// LT(0,astrisk_plus) * + kc_plus
// LT(0, QUESTION) ? ¿ OPEN_QUEST
// LT(0,KC_DQT)  " '
// LT(0,LLAMBDA)   ->  <-
// LT(0,EXC_DLR) $ #
// LT(0,NOT_EQUAL)  != ¡
// LT(0,excl_grave) !  ^ KC_EXLM

// pipe_m |>

    // Symbols Layer (posición 4 según enum)
    [_SYMB] = LAYOUT_split_3x6_3(
        // ,---------------------------------------------------------------------.                ,-----------------------------------------------------.
           XXXXXXX, LT(0,ASTRISK_PLUS), LT(0, KC_MINS), KC_EXLM, S(KC_GRAVE), XXXXXXX,       XXXXXXX, KC_GRAVE, KC_PERC , LT(0,DOUBLE_COLON), LT(0,LBRC2), XXXXXXX,
        // |--------+--------+--------+--------+--------+------------------------|                |--------+--------+--------+--------+--------+--------|
           LT(0, QUESTION), LT(0,KC_DQT), KC_COMM, C(S(KC_ENT)), DOUBLE_PIPE, XXXXXXX,             XXXXXXX, AMP_DOUBLE, KC_DOT, LT(0,EQUAL_DBL), LT(0,KC_LPRN), LT(0,KC_LCBR),
        // |--------+--------+--------+--------+--------+------------------------|                |--------+--------+--------+--------+--------+--------|
           PIPE_M, LT(0,LLAMBDA), LT(0,KC_LABK), LT(0,KC_RABK), XXXXXXX, QK_BOOT,                  QK_BOOT, XXXXXXX, KC_DLR, KC_SCLN, LT(0,NOT_EQUAL), KC_CIRC,
        // |--------+--------+--------+--------+--------+--------+---------------|                |--------+--------+--------+--------+--------+--------+--------|
                                         XXXXXXX, XXXXXXX,  XXXXXXX,                               TO(_BASE), XXXXXXX, TRIPLE_WHLD
                                         // `----------------------'                                `--------------------------'
    ),

    // Numbers Layer (posición 5 según enum) XXXXXXX LT(0,KC_MINS),
    // LT(0,astrisk_plus) * +
    [_NUMB] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, LT(0,ASTRISK_PLUS), LT(0,KC_MINS), LT(0,KC_SLSH), XXXXXXX, XXXXXXX,         XXXXXXX, KC_BSPC, KC_7, KC_8, KC_9, KC_ESC,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, LT(_DEL, KC_PERC), KC_EQL, KC_DOT, XXXXXXX, XXXXXXX,                         XXXXXXX, C(KC_G), KC_0, KC_4, KC_5, KC_6,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, KC_COMM, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, KC_1, KC_2, LT(0,KC_3), C(KC_G),
        // |--------+--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
                                      KC_ENT, XXXXXXX, XXXXXXX,                                 TO(_BASE), TG(_NUMB), TRIPLE_WHLD
                                     // `---------------------------------'                      `--------------------------'
    ),

    // Bookmark Layer (posición 6 según enum) D Y H FALTAN
    // LT HOLD TAP
    // LT(_BOOK_2,MARKER_B)    C(KC_F14)  MO(_BOOK_2)
    // LT(_BOOK_2,MARKER_3)    C(KC_3)    MO(_BOOK_2)
    [_BOOK] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------------.                     ,-----------------------------------------------------.
           XXXXXXX, C(KC_F17), C(KC_F18), C(KC_F19), XXXXXXX, XXXXXXX,                        XXXXXXX, XXXXXXX, C(KC_5), C(KC_6), C(KC_7), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------------|                     |--------+--------+--------+--------+--------+--------|
           C(KC_F13), C(KC_F14), C(KC_F15), C(KC_F16), XXXXXXX , XXXXXXX,          XXXXXXX, XXXXXXX, C(KC_1), C(KC_2), LT(_BOOK_2,MARKER_3), C(KC_4),
        // |--------+--------+--------+--------+--------+--------------|                     |--------+--------+--------+--------+--------+--------|
           XXXXXXX, C(KC_F20), LT(_BOOK_2,MARKER_B), C(KC_F22), XXXXXXX, XXXXXXX,                     XXXXXXX, XXXXXXX, C(KC_8), C(KC_9), TD(TDQ_BOOKMARK), XXXXXXX,
        // |--------+--------+--------+--------+--------+--------+-----|                     |--------+--------+--------+--------+--------+--------+--------|
                                           MO(_BOOK_2), XXXXXXX,  XXXXXXX,                    TO(_BASE),   XXXXXXX, MO(_BOOK_2)
                                           // `--------------------------'                     `--------------------------'
    ),

    // Super bookmark Layer (posición 7 según enum)
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

    //Super move ly9 (posición 8 según enum) MS_WHLU,  , MS_WHLD
// LT TAP HOLD
// LT(0, CRIGHT_10) C(KC_RIGHT) KC_RIGHT 10 VECES
// LT(0, CLEFT_10)  C(KC_LEFT)  KC_LEFT 10 VECES
// LT(0,C_END_HOME)  C(KC_END) C(KC_HOME)

// CRIGHT_5   KC_RIGHT 5 VECES
// CLEFT_5    KC_LEFT 5 VECES


    // Move Horizontal Layer (posición 8 según enum)
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

    // Layer 13 - new (posición 9 según enum)

    //LT TAP HOLD
    //[_MOVE_V]
    //LT(0, PGDN_PGUP) KC_PGDN KC_PGUP

    //DOWN_10   KC_DOWN 10 VECES
    //UP_10     KC_UP 10 VECES
    [_MOVE_V] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, DOWN_10, UP_10, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, LT(0,PAGE_PARAGRAPH_UP), A(KC_UP), LT(0,PAGE_PARAGRAPH_DOWN), A(KC_DOWN),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, MS_WHLU, MS_WHLD, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                       XXXXXXX, XXXXXXX, XXXXXXX,                               TO(_BASE), XXXXXXX, LT(0, PGDN_PGUP)
                                       // `----------------------'                              `--------------------------'
    ),

    // Super close window Layer (posición 10 según enum)
    [_MOVE_WIN] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, C(KC_T), C(KC_L), LT(0,SPLIT_WIN), A(KC_F4), XXXXXXX,                               XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           C(S(KC_TAB)), A(KC_LEFT), C(KC_TAB), A(KC_RIGHT), KC_F5, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, MO(_MOVE_L), XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, C(KC_F21), KC_F21, C(S(KC_T)), XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                     C(KC_F4), _______,  _______,                               TO(_BASE), XXXXXXX,XXXXXXX
                                     // `------------------------'                               `--------------------------'
    ),

    // Mouse2 Layer - hand left (posición 11 según enum)
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

    // Run debug Layer (posición 12 según enum)
    [_RUN] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           A(S(KC_F10)), C(KC_F5), C(S(KC_F10)), S(KC_F10), XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                        _______, _______,  _______,                             TO(_BASE), XXXXXXX, XXXXXXX
                                        // `----------------------'                              `--------------------------'
    ),

    // Mode Layer (posición 13 según enum)
    [_MODE] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, TG(_COMMIT), TG(_NUMB), XXXXXXX, XXXXXXX,                          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                         XXXXXXX, XXXXXXX, XXXXXXX,                             TO(_BASE), XXXXXXX, XXXXXXX
                                         // `---------------------'                             `--------------------------'
    ),

    // Commit Layer (posición 14 según enum)        MS_ACL0_TOGGLE MS_ACL21
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

    // AI Layer (posición 15 según enum)
    [_AI] = LAYOUT_split_3x6_3(
        // ,-----------------------------------------------------.                             ,-----------------------------------------------------.
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, KC_HASH, LT(0,KC_SLSH), KC_AT, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, KC_LEFT, XXXXXXX, KC_RIGHT, C(A(KC_I)),
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        // |--------+--------+--------+--------+--------+--------|                             |--------+--------+--------+--------+--------+--------+--------|
                                       XXXXXXX, XXXXXXX, XXXXXXX,                               XXXXXXX, XXXXXXX, XXXXXXX
                                       // `---------------------'                               `--------------------------'
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

void tdq_copy_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP://copy normal

                if (shift_active) {
                    unregister_code(KC_LSFT);     // Suelta físicamente la tecla
                    del_mods(MOD_BIT(KC_LSFT));   // Borra cualquier bit de estado
                    shift_active = false;

                }

                // Aplazar el copiado real
                defer_copy = true;
                defer_timer_copy = timer_read();

                break;

        case TD_SINGLE_HOLD: //
                 tdq_copy_hold_active = true;
                 layer_on(_MOVE_H);

                    //copy 1 line
                //tap_code_delay(KC_HOME, 10);
                //tap_code_delay(KC_HOME, 10);
                // Shift + End para seleccionar
                //tap_code16_delay(S(KC_END), 10);
                //tap_code16_delay(C(KC_C), 10);

         break;

        case TD_DOUBLE_TAP: //copy 1 word
                // Ctrl + Left
                tap_code16_delay(C(KC_LEFT),10);

                // Ctrl + Shift + Right
                tap_code16_delay(C(S(KC_RIGHT)), 10);

                // Ctrl + C para copiar
                tap_code16_delay(C(KC_C), 10);


        break;

        case TD_DOUBLE_HOLD: //copy 1 paragraph

                tap_code(KC_HOME);                   // Home
                tap_code16(S(A(KC_PGDN)));           // Shift + Alt + PgDn
                tap_code16(C(KC_C));                 // Ctrl + C

        break;

        default: break;
    }
}

void tdq_paste_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:  //paste normaL
                tap_code16(C(KC_V));      // atajo con el alias C()

        break;

        case TD_SINGLE_HOLD:
                tdq_paste_hold_active = true;
                layer_on(_MOVE_V);

                break;

        case TD_DOUBLE_TAP: //paste 1 word


                tap_code16_delay(C(KC_LEFT),10);
                tap_code16(C(S(KC_RIGHT)));
                tap_code16_delay(C(KC_V), 80);

                        // Ctrl + Left
//                register_code(KC_LCTL);
//                tap_code(KC_LEFT);
//                unregister_code(KC_LCTL);
//                wait_ms(10);
//
//                register_code(KC_LCTL);
//                register_code(KC_LSFT);
//                tap_code(KC_RIGHT); // Selecciona palabra
//                unregister_code(KC_LSFT); // Soltás shift antes de pegar
//                tap_code16_delay(C(KC_V), 10); // Pega normal
//                unregister_code(KC_LCTL);

        break;

        case TD_DOUBLE_HOLD: //portapapeles
                      tap_code16(C(S(KC_V))); //portapapeles INTELLIJ
                      //tap_code16(G(KC_V)); // portapapeles WIN

                      //paste as plain text - disabled
//                    register_code(KC_LCTL);
//                    register_code(KC_LSFT);
//                    register_code(KC_LALT);
//                    tap_code(KC_V);
//                    unregister_code(KC_LALT);
//                    unregister_code(KC_LSFT);
//                    unregister_code(KC_LCTL);
        break;

         case TD_TRIPLE_TAP: // paste 1 line

                 // Ir al inicio (2 veces)
                 tap_code_delay(KC_HOME,10);
                 tap_code_delay(KC_HOME,10);

                 // Shift + End para seleccionar
                 tap_code16_delay(S(KC_END),10);

                 // Ctrl + V para pegar
                 tap_code16(C(KC_V));

//                 // Ir al inicio
//                 tap_code(KC_HOME);
//                 wait_ms(10);
//                 tap_code(KC_HOME);
//                 wait_ms(10);
//                 // Ctrl + Shift + End para seleccionar
//                 register_code(KC_LSFT);
//                 tap_code(KC_END);
//                 unregister_code(KC_LSFT);
//                 wait_ms(10);
//
//                 // Ctrl + V para pegar
//                 register_code(KC_LCTL);
//                 tap_code(KC_V);
//                 unregister_code(KC_LCTL);
         break;
        default: break;
    }
}

void tdq_cut_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:
            if (shift_active) {
                unregister_code(KC_LSFT);
                shift_active = false;
            }


            defer_cut = true;
            defer_timer_cut = timer_read();
            break;

        case TD_SINGLE_HOLD: // Cut 1 line
                tap_code_delay(KC_HOME, 10);     // Ir al inicio
                tap_code16_delay(S(KC_END), 10); // Shift + End para seleccionar
                tap_code16(C(KC_X));             // Ctrl + X para cortar

            break;

        case TD_DOUBLE_TAP: // Cut 1 word

                // Ctrl + Left
                tap_code16_delay(C(KC_LEFT),10);

                // Ctrl + Shift + Right
                tap_code16_delay(C(S(KC_RIGHT)), 10);

                // Ctrl + X para cortar
                tap_code16(C(KC_X));

            break;

        case TD_DOUBLE_HOLD: //cut 1 paragraph

                tap_code(KC_HOME);            // Home
                tap_code16(S(A(KC_PGDN)));    // Shift + Alt + PgDn
                tap_code16(C(KC_X));          // Ctrl + X

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

void tdq_copy_reset(tap_dance_state_t *state, void *user_data) {
    // si la acción hold fue la que activó la capa, al liberar apagamos la capa
    if (tdq_copy_hold_active) {
        layer_off(_MOVE_H);
        tdq_copy_hold_active = false;
    }
    // resetea estado común (igual que x_reset)
    xtap_state.state = TD_NONE;
}

void tdq_paste_reset(tap_dance_state_t *state, void *user_data) {
    // si la acción hold fue la que activó la capa, al liberar apagamos la capa
    if (tdq_paste_hold_active) {
        layer_off(_MOVE_V);
        tdq_paste_hold_active = false;
    }
    // resetea estado común (igual que x_reset)
    xtap_state.state = TD_NONE;
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
     //if (keycode == TD(TDQ_SHIFT)) || keycode == TD(TDQ_MOVE_LY ))
//     {return 180;}

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

// move ly2
const rgblight_segment_t PROGMEM my_layer2_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {9,1, HSV_GREEN}
//    {8,1, HSV_RED} //PLAN B
);

// numbers ly5
const rgblight_segment_t PROGMEM my_layer5_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_WHITE}
    //    {4,2, HSV_RED} //PLAN B
);

// mouse ly6
const rgblight_segment_t PROGMEM my_layer6_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_RED}
//        {4,2, HSV_RED} //PLAN B
);

// mouse ly7
const rgblight_segment_t PROGMEM my_layer7_layer[] = RGBLIGHT_LAYER_SEGMENTS(
        {9,1, HSV_ORANGE}
    //    {4,2, HSV_RED} //PLAN B
);

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    NULL,               // 0
    my_layer2_layer,    // 1
    NULL,               // 2
    NULL,               // 3
    NULL,               // 4
    my_layer5_layer,    // 5
    my_layer6_layer,    // 6
    my_layer7_layer     // 7
);

void keyboard_post_init_user(void) {
    // Enable the LED layers
//    debug_enable=true;
//    debug_keyboard=true;
    rgblight_sethsv_noeeprom(0, 0, 0); // Blanco puro//para apagar la primera capa, que no alumbren todos los leds
    rgblight_layers = my_rgb_layers;
}

 void send_layer_status(const char* msg) {

       uint8_t buffer[32] = {0};
       const char* message = msg;
       strncpy((char*)buffer, message, sizeof(buffer) - 1);
       raw_hid_send(buffer, sizeof(buffer));

  }

layer_state_t layer_state_set_user(layer_state_t state) {
//    rgblight_set_layer_state(INDEX_LIGHT, layer_state_cmp(state, _LAYER));
//    rgblight_set_layer_state(5, layer_state_cmp(state, 5));

    rgblight_set_layer_state(1, false); //alfa LY OFF
    rgblight_set_layer_state(2, false); // MOVE LY OFF
    rgblight_set_layer_state(5, false); // NUMBERS LY OFF
    rgblight_set_layer_state(6, false); // MOUSE LY OFF
    rgblight_set_layer_state(7, false); // MOUSE2 LY OFF


uint8_t layer = get_highest_layer(state);

        switch (layer) {
            case 1:
                 rgblight_set_layer_state(1, true); // MOVE LY
                 send_layer_status("LAYER_MOVE");
                break;

            case 5:
                 rgblight_set_layer_state(5, true); // NUMBERS LY
                 send_layer_status("LAYER_NUM");
                break;

            case 6:
                rgblight_set_layer_state(6, true); // MOUSE LY
                send_layer_status("LAYER_MOUSE");
                break;

            case 7:
                rgblight_set_layer_state(7, true); // MOUSE LY
                send_layer_status("LAYER_MOUSE2");
                break;

            default:
               send_layer_status("LAYER_BASE");
               break;
        }
    return state;
}
