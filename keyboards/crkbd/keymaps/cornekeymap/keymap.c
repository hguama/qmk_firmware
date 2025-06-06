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


//Macro enum
enum custom_keycodes {
    M_SEL_COPY = SAFE_RANGE,
    M_ALT_TAB,
    M_CTRL_TAB,
    SPC_CTRL_TAB,
    ALT_RIGHT,
    M_ENIE,
    DBL_CLICK,
    ALL_COPY,
    CLEAR_WIN,
    HIDE_WIN,
    WIN_D,
    CTRL_SHIFT_ENTER,
    LLAMBDA,
    RLAMBDA,
    DOUBLE_COLON,
    ENV_VAR,
    LBRC2,
    SEL_MOUSE,
    HOME_END,
    PGUP_CTRLPG,
    PGDW_CTRLPG,
    SPACE_ENTER,
    ENT_UP,
    EVERYW_ACT,
    TAB_SPLIT,
    PROJECT_VIEW,
    NEW_FILE,
    SPLIT_WIN,
    FULL_SCREEN,
    NAV_ERROR,
    Q_ESC,
    MOUSE_PRESSED_CLICK,
    SHOW_QUICK_ENT,
    CODE_COMPLET,
    INFOPARM,
    COMM,
    FOLDING,
    MULTICURSOR,
    EDIT_OCCURR,
    R_LAST_EDIT,
    LAST_EDIT,
    RECENT_LOC,
    USAGES,
    W_ENTER,
    REFACTOR,
    SHIFT_TOGGLE,
    CTRL_TOGGLE,
    ALT_TOGGLE,
    MOD_CLEAR,
    CUT,
    OSL_DEV_LY,
    TG_0,
    TG_6,
    TG_7,

};

//Combo enum
enum combos{
    CB_CTRL_Z,
    CB_CTRL_Z_MOVE,
    CB_SUPR,
    CB_SUPR_MOVE_LY,
    CB_SUPR_MOUSE_LY,
    CB_BACKSPACE,
    CB_BACKSPACE_MOVE,
    CB_BACKSPACE_MOUSE,
    CB_CTRL_Z_MOUSE,
    CB_CTRL_Z_NUMBERS,
    CB_TAB,
    CB_LAYER,
    CB_ALL_COPY,
    CB_INSERT,
    CB_HIDE_WIN,
    CB_CLOSE_TAB,
    CB_CLOSE_OTHERS,
};

//Tap Dance enum
enum {
    TD_ESC_CAPS,
    TD_RABK_EQ,
    TD_LABK_EQ,
    TDQ_DEMO,
    TDQ_COPY,
    TDQ_PASTE,
    TDQ_CUT,
    TDQ_DEL,
    TDQ_CLICK,
    TDQ_Z_ENG,
    TDQ_TOGGLE_HOLD,
    TDQ_BOOKMARK,
    TDQ_GOTO,
    TDQ_FIND,
    TDQ_GENERATE,

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

bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;
static uint16_t timer_key;


// Prototypes quad
void dance_rabk(tap_dance_state_t *state, void *user_data); //plantilla
void dance_labk(tap_dance_state_t *state, void *user_data); //plantilla
void tdq_toggle_hold_finished(tap_dance_state_t *state, void *user_data);//plantilla
void tdq_toggle_hold_reset(tap_dance_state_t *state, void *user_data); //plantilla

td_state_t cur_dance(tap_dance_state_t *state);
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);
void tdq_copy_finished(tap_dance_state_t *state, void *user_data);
void tdq_paste_finished(tap_dance_state_t *state, void *user_data);
void tdq_cut_finished(tap_dance_state_t *state, void *user_data);
void tdq_del_finished(tap_dance_state_t *state, void *user_data);
void tdq_click_finished(tap_dance_state_t *state, void *user_data);
void tdq_z_eng_finished(tap_dance_state_t *state, void *user_data);
void tdq_bookmark_finished(tap_dance_state_t *state, void *user_data);
void tdq_goto_finished(tap_dance_state_t *state, void *user_data);
void tdq_find_finished(tap_dance_state_t *state, void *user_data);
void tdq_generate_finished(tap_dance_state_t *state, void *user_data);

//Config revert to repeat key
uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {

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
}

//Combos
const uint16_t PROGMEM cb_ctrl_z[] = {KC_J, LT(2,KC_K), COMBO_END}; //base right ly //indice medio
const uint16_t PROGMEM cb_ctrl_z_move[] = {KC_LEFT, KC_DOWN, COMBO_END}; //move right ly
const uint16_t PROGMEM cb_ctrl_z_mouse[] = {MS_LEFT, MS_DOWN, COMBO_END}; //mouse right ly
const uint16_t PROGMEM cb_ctrl_z_numbers[] = {KC_0, KC_4, COMBO_END}; //mouse right ly

const uint16_t PROGMEM cb_supr[] = {LT(5,KC_F), KC_J, COMBO_END}; //base central   //indices
const uint16_t PROGMEM cb_supr_move_ly[] = {TD(TDQ_PASTE), KC_LEFT, COMBO_END}; //move  ly
const uint16_t PROGMEM cb_supr_mouse_ly[] = {TD(TDQ_PASTE),  MS_LEFT, COMBO_END};//mouse  ly

const uint16_t PROGMEM cb_backspace[] = {LT(2,KC_D), KC_J, COMBO_END}; //base    //medio, indiced dj dd djdj dj
const uint16_t PROGMEM cb_backspace_move[] = {TD(TDQ_COPY), KC_LEFT, COMBO_END}; //base    //medio, indice
const uint16_t PROGMEM cb_backspace_mouse[] = {TD(TDQ_COPY), MS_LEFT, COMBO_END}; //base    //medio, indice

//const uint16_t PROGMEM cb_tab[] = {LT(5,KC_F), KC_J, COMBO_END}; //base   //indices //YA USADA

const uint16_t PROGMEM cb_close_tab[]    = {KC_UP, TAB_SPLIT, COMBO_END}; //move right ly //medio anular arriba //YA USADA
const uint16_t PROGMEM cb_close_others[] = {M_ALT_TAB,  KC_UP, TAB_SPLIT, COMBO_END}; //move right ly //indice medio anular arriba

const uint16_t PROGMEM cb_layer[] = {LT(1, KC_S), LT(1, KC_L), COMBO_END}; //base left right// anulares //toggle ly5 numbers
const uint16_t PROGMEM cb_all_copy[] = {LGUI_T(KC_E), KC_I, COMBO_END}; //base left right //medios arriba
const uint16_t PROGMEM cb_insert[] = {LT(2, KC_D), LT(2,KC_K), COMBO_END}; //base left right //medios//
//const uint16_t PROGMEM cb_hide_win[] = {KC_DOWN, KC_RIGHT, COMBO_END}; //move right// medio anular

//combo actions
combo_t key_combos[] = {
   [CB_CTRL_Z]   = COMBO(cb_ctrl_z, LCTL(KC_Z)), //base right
   [CB_CTRL_Z_MOVE]   = COMBO(cb_ctrl_z_move, LCTL(KC_Z)), //move right
   [CB_CTRL_Z_MOUSE]   = COMBO(cb_ctrl_z_mouse, LCTL(KC_Z)), //mouse right
   [CB_CTRL_Z_NUMBERS]   = COMBO(cb_ctrl_z_numbers, LCTL(KC_Z)), //mouse right
   [CB_SUPR]   =  COMBO(cb_supr, KC_DEL), //base right
   [CB_SUPR_MOVE_LY]    =  COMBO(cb_supr_move_ly, KC_DEL), //move left
   [CB_SUPR_MOUSE_LY]   =  COMBO(cb_supr_mouse_ly, KC_DEL), //mouse left
   [CB_BACKSPACE]   =  COMBO(cb_backspace, KC_BSPC), //base right
   [CB_BACKSPACE_MOVE]    =  COMBO(cb_backspace_move, KC_BSPC), //move left
   [CB_BACKSPACE_MOUSE]   =  COMBO(cb_backspace_mouse, KC_BSPC), //mouse left
//   [CB_TAB]      = COMBO(cb_tab, KC_TAB), //base indices
   [CB_CLOSE_TAB]         = COMBO(cb_close_tab, C(KC_F4)), //move right
   [CB_CLOSE_OTHERS]      = COMBO(cb_close_others, C(KC_F21)), //move right
   [CB_LAYER]    = COMBO(cb_layer, TG(5)), //base anulares
   [CB_ALL_COPY] = COMBO(cb_all_copy, ALL_COPY), //base medios arriba
   [CB_INSERT] = COMBO(cb_insert, KC_INS), //base medios
//   [CB_HIDE_WIN] = COMBO(cb_hide_win, HIDE_WIN), //move right medio anular
};

void clear_all(void) {
    clear_mods();             // Libera Ctrl, Shift, Alt, etc.
    clear_keyboard();         // Libera cualquier tecla registrada
}


//PR record
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static bool shift_active = false;
    static bool ctrl_active  = false;
    static bool alt_active   = false;


        switch (keycode) {
            case M_SEL_COPY: SEND_STRING(SS_LCTL("ac")); break;
            case M_CTRL_TAB: if (record->event.pressed) { SEND_STRING(SS_LCTL(SS_TAP(X_TAB))); } break;
            case ALT_RIGHT: if (record->event.pressed) { tap_code16(A(KC_RIGHT)); } break;
            case M_ENIE: if (record->event.pressed) { SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_KP_1) SS_TAP(X_KP_6) SS_TAP(X_KP_4) SS_UP(X_LALT)); } break;
            case WIN_D: if (record->event.pressed) { SEND_STRING(SS_LGUI("d"));  } break;

            case DOUBLE_COLON:
                if (record->event.pressed){
                   SEND_STRING(":"); //when pressed
                   }else{
                   SEND_STRING(":"); //when release
                   }
                break;

            case M_ALT_TAB:
                  if (record->event.pressed) {
                    if (!is_alt_tab_active) {
                      is_alt_tab_active = true;
                      register_code(KC_LALT);
                    }
                    alt_tab_timer = timer_read();
                    register_code(KC_TAB);
                  } else {
                    unregister_code(KC_TAB);
                  }
              break;

            case DBL_CLICK:
                    if (record->event.pressed) {
                        tap_code(KC_BTN1);
                        wait_ms(50);
                        tap_code(KC_BTN1);
                    }
               break;

            case ALL_COPY:
                  if (record->event.pressed) {
                    SEND_STRING(SS_LCTL("ac"));
                  }
              break;

            case CLEAR_WIN:
                    if (record->event.pressed) {
                      SEND_STRING(SS_LGUI("d"));
                     }
                 break;

            case HIDE_WIN:
                if (record->event.pressed) {
                 tap_code16(C(S(KC_F12)));
                 }
                 break;


            case CTRL_SHIFT_ENTER :
                   if (record->event.pressed) {
                       SEND_STRING(SS_LCTL(SS_LSFT(SS_TAP(X_ENTER))));
                       }
                   break;

            case LT(0,KC_DOWN):
              if (!record->tap.count && record->event.pressed) {
                  tap_code16(KC_RIGHT); // hold
                  return false;
              }
              return true; //normal tap

            case LT(1,KC_SCLN):
                if (!record->tap.count && record->event.pressed) {
                   SEND_STRING(SS_LSFT(SS_TAP(X_SCLN))); // hold
                    return false;
                }
                return true;

            case LT(1,KC_SLSH):
                if (!record->tap.count && record->event.pressed) {
                   SEND_STRING("\\"); // hold
                    return false;
                }
                return true;

            case  LT(1,KC_DQT):
                   if (record->event.pressed) {
                      if (!record->tap.count) {
                         SEND_STRING(SS_TAP(X_QUOT)); // hold
                          return false;
                      }else{
                         SEND_STRING("\""); // tap
                         return false;
                          }
                      }
                      return true;

            case  LT(1,KC_EXLM):
                  if (record->event.pressed) {
                     if (!record->tap.count) {
                       SEND_STRING("!="); // hold
                        return false;
                     }else {
                     SEND_STRING("!"); //tap
                       return false;
                       }
                     }
                     return true;

            case  LT(1,KC_LABK):
                  if (record->event.pressed) {
                     if (!record->tap.count) {
                       SEND_STRING("<="); // hold
                        return false;
                     }else {
                       SEND_STRING("<"); //tap
                        return false;
                           }
                        }
                     return true;

            case  LT(1,KC_RABK):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                      SEND_STRING(">="); // hold
                       return false;
                    }else {
                        SEND_STRING(">"); //tap
                         return false;
                            }
                        }
                    return true;

            case  LT(1,KC_MINS):
                    if (!record->tap.count && record->event.pressed) {
                      SEND_STRING("_"); //hold
                       return false;
                    }
                    return true;

            case  LT(1,KC_EQL):
                    if (!record->tap.count && record->event.pressed) {
                       SEND_STRING("#"); // hold
                       return false;
                    }
                    return true;

            case  LT(1,KC_AT):
                  if (record->event.pressed) {
                    if (!record->tap.count) {
                      SEND_STRING("?"); // hold
                       return false;
                    }else {
                       SEND_STRING("@");
                         return false;
                        }
                    }
                    return true;

            case  LT(1,KC_TILD):
                 if (record->event.pressed) {
                    if (!record->tap.count  ) {
                      SEND_STRING("`"); // hold
                       return false;
                    }else {
                      SEND_STRING("~");
                        return false;
                      }
                     }else {}
                    return true;

            case  LT(1, KC_PERC):
                 if (record->event.pressed) {
                    if (!record->tap.count && record->event.pressed) {
                      SEND_STRING("%");// hold
                       return false;

                    } else{
                      SEND_STRING("+");
                          return false;
                      }
                    } else{}
                    return true;

            case  LT(1,KC_AMPR):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                       SEND_STRING("$");// hold
                       return false;
                    }else {
                        SEND_STRING("&");
                         return false; // tap
                             }
                     }else {}
                    return true;

            case  LT(1,ENV_VAR):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                       SEND_STRING("#{}#");
                       wait_ms(100);
                       tap_code(KC_LEFT);  // Coloca cursor entre { y }
                       tap_code(KC_LEFT);
                       return false;
                    }else {
                        SEND_STRING("${}");
                        wait_ms(100);
                        tap_code(KC_LEFT);

                         return false; // Evita que se ejecute la acción por defecto
                             }
                     }else {}
                    return true;

            case  LLAMBDA:
                   if (record->event.pressed) {
                     SEND_STRING("<-"); // hold
                   }
                   break;

            case  RLAMBDA:
                   if (record->event.pressed) {
                     SEND_STRING("->"); // hold
                   }
                  break;

            case  LT(1,KC_LPRN):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                      SEND_STRING(")");
                       return false;
                      }else {
                         SEND_STRING("(");
                         return false;
                      }
                    }
                   break;

            case  LT(1,KC_LCBR):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                     SEND_STRING("}");
                       return false;
                    }else {
                          SEND_STRING("{");
                          return false;
                          }
                       }
                    break;

            case  LT(1,LBRC2):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                      SEND_STRING("]");
                       return false;
                    }else {
                       SEND_STRING("[");
                       return false;
                         }
                     }
                    break;

            case  HOME_END:
                   if (record->event.pressed) {
                      timer_key = timer_read();
                   }else {
                       if (timer_elapsed(timer_key) < TAPPING_TERM) {
                        tap_code(KC_END);//TAP
                        }else{
                         tap_code(KC_HOME);//HOLD
                        }
                     }
                    return false;

            case PGUP_CTRLPG:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → CTRL PAGE UP
                            tap_code16(C(KC_PGUP));
                        }else {
                            // HOLD → PAGE UP
                            tap_code(KC_PGUP);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case PGDW_CTRLPG:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → CTRL PAGE DOWN
                            tap_code16(C(KC_PGDN));
                        }else {
                            // HOLD → PAGE DOWN
                            tap_code(KC_PGDN);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case EVERYW_ACT:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → search everywhere
                            tap_code16(KC_F14);
                        }else {
                            // HOLD → actions
                            SEND_STRING(SS_LCTL(SS_LSFT("A")));
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case Q_ESC:
                   if (record->event.pressed) {
                       timer_key = timer_read(); // Inicia el temporizador
                   }else {
                       if (timer_elapsed(timer_key) < TAPPING_TERM) {
                           // TAP → PAGE DOWN
                           tap_code(KC_Q);
                       }else {
                           // HOLD → PAGE UP
                           tap_code(KC_ESC);
                       }
                   }
                   return false; // Bloquea el comportamiento por defecto

            case SPC_CTRL_TAB:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → PAGE DOWN
                            tap_code(KC_SPACE);
                        }else {
                            // HOLD → PAGE UP
                            tap_code16(A(KC_TAB));
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case ENT_UP:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → ENTER
                            tap_code(KC_ENT);
                        }else {
                            // HOLD → ARROW UP
                            tap_code(KC_UP);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case NAV_ERROR:
                   if (record->event.pressed) {
                       timer_key = timer_read(); // Inicia el temporizador
                   }else {
                       if (timer_elapsed(timer_key) < TAPPING_TERM) {
                           // TAP → Navigate to error
                           tap_code(KC_F2);
                       }else {
                           // HOLD → Navigate to error back
                           tap_code16(S(KC_F2));
                       }
                   }
                   return false; // Bloquea el comportamiento por defecto

            case W_ENTER:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → KC W
                            tap_code(KC_W);
                        }else {
                            // HOLD → ENTER
                            tap_code(KC_ENT);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case PROJECT_VIEW:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → PROJECT
                            tap_code16(A(KC_1));
                        }else {
                            // HOLD → PROJECT SELECT IN
                            tap_code16(A(KC_F1));
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case NEW_FILE:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → NEW FILE, CLASS...
                            tap_code16(A(KC_INS));
                        }else {
                            // HOLD → NEW CLASS
                            tap_code16(KC_F15);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case SPLIT_WIN:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → SPLIT RIGHT
                            tap_code16(KC_F16);
                        }else {
                            // HOLD → SPLIT DOWN
                            tap_code16(KC_F17);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case FULL_SCREEN:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → FULL SCREEN
                            tap_code16(KC_F18);
                        }else {
                            // HOLD → ZEN MODE
                            tap_code16(KC_F19);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case R_LAST_EDIT:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → KC R
                            tap_code(KC_R);
                        }else {
                            // HOLD → ctrl shift backspace / go to last edit location
                            register_code(KC_LCTL);
                            register_code(KC_LSFT);
                            tap_code_delay(KC_BSPC, 30);
                            unregister_code(KC_LCTL);
                            unregister_code(KC_LSFT);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case LAST_EDIT:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                        //tap
                            register_code(KC_LCTL);
                            register_code(KC_LSFT);
                            tap_code_delay(KC_BSPC, 30);
                            unregister_code(KC_LCTL);
                            unregister_code(KC_LSFT);
                        }else {
                        //hold
                           tap_code(KC_F13);

                        }
                    }
                    return false;

            case RECENT_LOC:
                    if (record->event.pressed) {
                        timer_key = timer_read();
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                        //tap
                            SEND_STRING(SS_LCTL("e"));
                        }else {
                        //hold
                          SEND_STRING(SS_LCTL(SS_LSFT("e")));
                        }
                    }
                    return false;

            case USAGES:
                    if (record->event.pressed) {
                        timer_key = timer_read();
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                        //tap
                            SEND_STRING(SS_LCTL(SS_LALT("f7")));
                        }else {
                        //hold
                          SEND_STRING(SS_LALT("f7"));
                        }
                    }
                    return false;

            case CUT:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → CTRL X
                            tap_code16(C(KC_X));
                        }else {
                            // HOLD
                            tap_code_delay(KC_HOME, 30);
                            register_code(KC_LSFT);
                            tap_code_delay(KC_END, 30);
                            tap_code16_delay(C(KC_X), 30);
                            unregister_code(KC_LSFT);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case TAB_SPLIT:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → TAB
                            tap_code16(KC_TAB);
                        }else {
                            // HOLD GO TO NEXT WIN SPLIT
                             tap_code16(KC_F20);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case COMM:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP - comment line
                            tap_code16(LCTL(KC_SLSH));
                        }else {
                            // HOLD - comment block
                            tap_code16(LCTL(LSFT(KC_SLSH)));

                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case MOUSE_PRESSED_CLICK:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP - mouse pressed
                            tap_code(MS_BTN1);
                            register_code(MS_BTN1);
                        }else {
                            // HOLD - mouse click
                            tap_code(MS_BTN1);
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case SHOW_QUICK_ENT:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP - enter
                            tap_code(KC_ENT);
                        }else {
                            // HOLD - show quick fixes
                            tap_code16(LALT(KC_ENT));
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case CODE_COMPLET:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP - code completion basic
                            tap_code16(LCTL(KC_SPACE));
                        }else {
                            // HOLD - code completion adv
                            tap_code16(LCTL(LSFT(KC_SPACE)));

                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case INFOPARM:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP - view info parameter
                            tap_code16(LCTL(KC_P));
                        }else {
                            // HOLD - view method context info
                            tap_code16(A(KC_Q));
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case FOLDING:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP - desplegar 1 region
                            tap_code16(LCTL(KC_KP_PLUS));
                        }else {
                            // HOLD - contrae o cierra todos
                            tap_code16(LCTL(LSFT(KC_KP_MINUS)));

                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case MULTICURSOR:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP - multicursor
                          tap_code16(LALT(LSFT(KC_INS)));
                        }else {
                            // HOLD - multicursor ends of line
                            tap_code16(LALT(LSFT(KC_G)));
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case EDIT_OCCURR:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP -  Busca ocurrencias, se desplaza de 1 en 1
                           tap_code16(KC_F3);
                        }else {
                            // HOLD -  Selecciona la ocurrencia a buscar
                             tap_code16(C(KC_F3));
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto

            case REFACTOR:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP - REFACTOR THIS
                            tap_code16(LCTL(LALT(LSFT(KC_T))));
                        }else {
                            // HOLD - rename
                             tap_code16(LSFT(KC_F6));
                        }
                    }
                    return false; // Bloquea el comportamiento por defecto


            case SHIFT_TOGGLE:
                  if (record->event.pressed) {
                    shift_active = !shift_active;
                    if (shift_active) {
                        register_code(KC_LSFT);
                    } else {
                        unregister_code(KC_LSFT);
                    }
                }
                break;

            case CTRL_TOGGLE:
                  if (record->event.pressed) {
                    ctrl_active = !ctrl_active;
                    if (ctrl_active) {
                        register_code(KC_LCTL);
                    } else {
                        unregister_code(KC_LCTL);
                    }
                }
                  break;

            case ALT_TOGGLE:
                  if (record->event.pressed) {
                    alt_active = !alt_active;
                    if (alt_active) {
                        register_code(KC_LALT);
                    } else {
                        unregister_code(KC_LALT);
                    }
                }
                  break;

            case MOD_CLEAR:
                   if (record->event.pressed) {
                    if (shift_active) { unregister_code(KC_LSFT); shift_active = false; }
                    if (ctrl_active)  { unregister_code(KC_LCTL); ctrl_active  = false; }
                    if (alt_active)   { unregister_code(KC_LALT); alt_active   = false; }
                   }
                   break;

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
                    if (get_mods() & MOD_MASK_ALT) { unregister_mods(MOD_MASK_ALT);}  // Detiene Alt antes de que llegue a Windows
                    if (shift_active) { unregister_code(KC_LSFT); shift_active = false; }
                    if (ctrl_active)  { unregister_code(KC_LCTL); ctrl_active  = false; }
                    if (alt_active)   { unregister_code(KC_LALT); alt_active   = false; }

                    clear_keyboard();
                    tap_code(KC_ESC);
                }
                break;

            case TO(0): clear_all(); break;

            case LT(3,TG_0):
                  if (record->event.pressed) {
                       if (!record->tap.count) {
                          return true; //hold
                       }else {
                           clear_all();
                           layer_invert(2); //tap

                            return false;
                               }
                           }
                       return true;

            case LT(3,TG_7):
                  if (record->event.pressed) {
                       if (!record->tap.count) {
                          return true; //hold
                       }else {
                           clear_all();
                           layer_invert(7); //tap
                            return false;
                               }
                           }
                       return true;

            case LT(4,TG_6):
                  if (record->event.pressed) {
                       if (!record->tap.count) {
                          return true; //hold
                       }else {
                           clear_all();
                           layer_invert(6); //tap
                            return false;
                               }
                           }
                       return true;


        }//END SWITCH
    return true;
};
//Pr record END

//timer for alt_tab macro
void matrix_scan_user(void) {
  if (is_alt_tab_active) {
    if (timer_elapsed(alt_tab_timer) > 1000) {
      unregister_code(KC_LALT);
      is_alt_tab_active = false;
    }
  }
  }

//Quad actions
tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC_CAPS] = ACTION_TAP_DANCE_DOUBLE(KC_ESC, KC_CAPS), //plantilla
    [TD_RABK_EQ] =  ACTION_TAP_DANCE_FN(dance_rabk), //plantilla
    [TD_LABK_EQ] =  ACTION_TAP_DANCE_FN(dance_labk), //plantilla
    [TDQ_DEMO] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished, x_reset), //plantilla
    [TDQ_TOGGLE_HOLD] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_toggle_hold_finished, tdq_toggle_hold_reset), //plantilla

    [TDQ_COPY] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_copy_finished, x_reset),
    [TDQ_PASTE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_paste_finished, x_reset),
    [TDQ_CUT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_cut_finished, x_reset),
    [TDQ_DEL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_del_finished, x_reset),
    [TDQ_GOTO] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_goto_finished, x_reset),
    [TDQ_CLICK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_click_finished, x_reset),
    [TDQ_Z_ENG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_z_eng_finished, x_reset),
    [TDQ_BOOKMARK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_bookmark_finished, x_reset),
    [TDQ_FIND] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_find_finished, x_reset),
    [TDQ_GENERATE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_generate_finished, x_reset),

};


 //KEY MAP asdj jplkj j
 //LY 0                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   //LY 0
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                       ,-----------------------------------------------------.
   Q_ESC, W_ENTER, LGUI_T(KC_E),  R_LAST_EDIT, KC_T, XXXXXXX,                      XXXXXXX, KC_Y, KC_U,   KC_I,  KC_O,  KC_ESC,
  //|--------+--------+--------+--------+--------+--------|                        |--------+--------+--------+--------+--------+--------|
   LSFT_T(KC_A), LT(1, KC_S), LT(2,KC_D), LT(5,KC_F), KC_G, KC_CAPS,               XXXXXXX,  KC_H, KC_J, LT(2,KC_K), LT(1, KC_L), RSFT_T(KC_P),
  //|--------+--------+--------+--------+--------+--------|                         |--------+--------+--------+--------+--------+--------|
   KC_Z, KC_X, KC_C, LCTL_T(KC_V),  KC_B, WIN_D,                                   XXXXXXX, KC_N,  KC_M, CODE_COMPLET, TD(TDQ_DEL), M_ENIE,
  //|--------+--------+--------+--------+--------+--------+--------|                |--------+--------+--------+--------+--------+--------+--------|
                                      TG(2), SPC_CTRL_TAB,  XXXXXXX,     XXXXXXX,   SHOW_QUICK_ENT, TG(5)
                                      //`--------------------------'  `--------------------------'


  ),
//symbols ly 1
    [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                           ,-----------------------------------------------------.
  KC_ASTR,  LT(1,KC_PERC), LT(1,KC_MINS),  LT(1,KC_SLSH), LT(1,ENV_VAR), XXXXXXX,     XXXXXXX, XXXXXXX, LT(1,KC_AT), LT(1,KC_EQL),  LT(1,KC_DQT), LT(1,KC_TILD),
  //|--------+--------+--------+--------+--------+--------|                           |--------+--------+--------+--------+--------+--------|
  DOUBLE_COLON, LT(1, KC_AMPR), LT(1,KC_LABK), LT(1,KC_RABK), KC_PIPE, XXXXXXX,       XXXXXXX, LT(1,KC_EXLM) , KC_DOT, CTRL_SHIFT_ENTER,  LT(1,KC_LPRN), LT(1,KC_LCBR),
  //|--------+--------+--------+--------+--------+--------|                           |--------+--------+--------+--------+--------+--------|
  XXXXXXX, KC_CIRC, LLAMBDA, RLAMBDA, XXXXXXX, QK_BOOT,                               QK_BOOT, XXXXXXX, KC_COMM, LT(1,KC_SCLN),  LT(1,LBRC2),  XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_SPC, _______,  XXXXXXX,     TO(0),   KC_TRNS, XXXXXXX
                                      //`--------------------------'  `--------------------------'
 ),

 //move ly 2

    [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
  KC_ESC, SHIFT_TOGGLE, CTRL_TOGGLE, ALT_TOGGLE, QK_AREP, XXXXXXX,             XXXXXXX, M_CTRL_TAB, M_ALT_TAB,  KC_UP, TAB_SPLIT, KC_ESC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
  LT(4,TG_6), MO(9), TD(TDQ_COPY), TD(TDQ_PASTE), TD(TDQ_CUT), XXXXXXX,        XXXXXXX, MO(4), KC_LEFT, KC_DOWN, KC_RIGHT, HOME_END,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
  A(KC_F), QK_REP , LCTL(KC_W), MS_BTN1, M_SEL_COPY, XXXXXXX,                  XXXXXXX, XXXXXXX, ALT_RIGHT, CODE_COMPLET, TD(TDQ_DEL), LCTL(LSFT(KC_M)),
  //| ------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                       LT(3,TG_0), SPC_CTRL_TAB, XXXXXXX,     TO(0), KC_TRNS, LT(3,TG_7)
                                      //`--------------------------'  `--------------------------'
  ),

  //dev ly 3
    [3] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      EDIT_OCCURR, MULTICURSOR, A(KC_J) , S(A(KC_J)), INFOPARM, XXXXXXX,           XXXXXXX, NAV_ERROR, RECENT_LOC, PROJECT_VIEW, A(KC_F12), SPLIT_WIN,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      FOLDING, C(KC_D), COMM, TD(TDQ_FIND), C(KC_R), XXXXXXX,                      XXXXXXX, USAGES, LAST_EDIT, TD(TDQ_BOOKMARK), TD(TDQ_GOTO), C(S(KC_F12)),
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, C(A(KC_T)), REFACTOR, TD(TDQ_GENERATE), C(S(KC_U)), XXXXXXX,        XXXXXXX, FULL_SCREEN, NEW_FILE, C(A(KC_LEFT)), C(A(KC_RIGHT)), C(KC_F12),
      //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                 MO(4),  XXXXXXX, XXXXXXX,     TO(0),   KC_TRNS, EVERYW_ACT
                                          //`--------------------------'  `--------------------------'
   ),
   //bookmark ly 4

    [4] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    XXXXXXX, C(KC_F19), C(KC_F20), C(S(KC_F21)), C(KC_F22), XXXXXXX,             XXXXXXX, XXXXXXX, C(KC_7), C(KC_8), C(KC_9), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    XXXXXXX, MO(8), C(KC_F16), C(KC_F17), C(KC_F18), XXXXXXX,                    XXXXXXX, XXXXXXX, C(KC_0), C(KC_4), C(KC_5), C(KC_6),
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    XXXXXXX, C(KC_F13), C(KC_F14), C(KC_F15), XXXXXXX, XXXXXXX,                  XXXXXXX, XXXXXXX, C(KC_1), C(KC_2), C(KC_3), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                      XXXXXXX, _______,  XXXXXXX,     TO(0),   KC_TRNS, MO(8)
                       //`--------------------------'  `--------------------------'
),
 //numbers ly 5

    [5] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    KC_ESC,  MO(8), TD(TDQ_DEL),  KC_DEL, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_ESC,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_ASTR, LT(1,KC_PERC), LT(1,KC_MINS), LT(1,KC_SLSH), XXXXXXX, XXXXXXX,      XXXXXXX,XXXXXXX, KC_0, KC_4, KC_5, KC_6,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_CIRC, XXXXXXX, KC_DOLLAR, KC_EQL, XXXXXXX, XXXXXXX,                       XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                C(KC_Z), SPC_CTRL_TAB, XXXXXXX,   TO(0), KC_TRNS, TG(5)
                                 //`--------------------------'  `--------------------------'
),
//mouse ly 6

      [6] = LAYOUT_split_3x6_3(
   //,-----------------------------------------------------.                    ,-----------------------------------------------------.
   XXXXXXX, MS_ACL2, TD(TDQ_CLICK), MS_ACL0, CTRL_TOGGLE, XXXXXXX,              XXXXXXX, XXXXXXX, MS_WHLU, MS_UP, MS_WHLD, KC_ESC,
   //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
   TG(6), TD(TDQ_Z_ENG), TD(TDQ_COPY), TD(TDQ_PASTE), TD(TDQ_CUT), XXXXXXX,     XXXXXXX, XXXXXXX, MS_LEFT, MS_DOWN, MS_RGHT, MS_WHLR,
   //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
   XXXXXXX, MS_WHLL, MS_WHLR, MOUSE_PRESSED_CLICK, KC_ESC, XXXXXXX,             XXXXXXX, XXXXXXX, MS_WHLL, LCTL(KC_Z), TD(TDQ_DEL), XXXXXXX,
   //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        KC_ENT,  SPC_CTRL_TAB,     XXXXXXX,     TO(0), KC_TRNS, TG(6)
                                       //`--------------------------'  `--------------------------'
  ), // mouse2 ly7 - single mouse hand left

    [7] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, CTRL_TOGGLE, MS_WHLU, TD(TDQ_CLICK), MS_WHLD, MS_ACL2,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX , XXXXXXX,                   XXXXXXX, TD(TDQ_Z_ENG) , TD(TDQ_PASTE), TD(TDQ_COPY), TD(TDQ_CUT), MS_ACL0,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, RM_SATD, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, KC_ESC,  MOUSE_PRESSED_CLICK, MS_WHLR, MS_WHLL, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_ENT, SPC_CTRL_TAB,  _______,     TO(0),   KC_TRNS, TG(7)
                                      //`--------------------------'  `--------------------------'
 ), // layer 8 super bookmark2

      [8] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        XXXXXXX, A(KC_G), A(KC_H), A(KC_I), A(KC_K), XXXXXXX,                    XXXXXXX, XXXXXXX, C(S(KC_7)), C(S(KC_8)), C(S(KC_9)), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, XXXXXXX, A(KC_D), A(KC_E), A(KC_F), XXXXXXX,                    XXXXXXX,XXXXXXX , C(S(KC_0)), C(S(KC_4)), C(S(KC_5)), C(S(KC_6)),
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, A(KC_A), A(KC_B), A(KC_C), XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, C(S(KC_1)), C(S(KC_2)), C(S(KC_3)), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            XXXXXXX, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                        //`--------------------------'  `--------------------------'
   ), // super move ly9

        [9] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, KC_F6, C(KC_HOME), A(KC_UP), C(KC_END), XXXXXXX,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, KC_F2, C(KC_LEFT), A(KC_DOWN), C(KC_RIGHT), XXXXXXX,
      //|--------+--------+---), // layer 8 super bookmark2-----+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, PGUP_CTRLPG, XXXXXXX, PGDW_CTRLPG, XXXXXXX,
      //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                              XXXXXXX, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                          //`--------------------------'  `--------------------------'
     ), //LY 10 not used

          [10] = LAYOUT_split_3x6_3(
        //,-----------------------------------------------------.                    ,-----------------------------------------------------.
            XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
        //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
            KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,     XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
        //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
            KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
        //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                KC_SPC, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                            //`--------------------------'  `--------------------------'
        ), //LY 11 not used

            [11] = LAYOUT_split_3x6_3(
          //,-----------------------------------------------------.                    ,-----------------------------------------------------.
              XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
          //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
              KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,     XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
          //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
              KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
          //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                  KC_SPC, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                              //`--------------------------'  `--------------------------'
         ), // LY12 not used

             [12] = LAYOUT_split_3x6_3(
           //,-----------------------------------------------------.                    ,-----------------------------------------------------.
               XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
           //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
               RM_TOGG, RM_HUEU, RM_SATU, RM_VALU, UG_TOGG, XXXXXXX,                    XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
           //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
               RM_NEXT, RM_HUED, RM_SATD, RM_VALD, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
           //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                   KC_SPC, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                               //`--------------------------'  `--------------------------'
          )



};



//Tap dance imple

// > >=  plantilla
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
}


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

//plantilla
void x_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: SEND_STRING(SS_TAP(X_V)); break;
        case TD_SINGLE_HOLD: layer_invert(2); break; //layer_on(2)
        case TD_DOUBLE_TAP:SEND_STRING(SS_LCTL("v") SS_TAP(X_ENT)); break;
        case TD_DOUBLE_HOLD:SEND_STRING(SS_LCTL(SS_LSFT("v"))); break;
        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE TAP x!!"); break;
        case TD_TRIPLE_HOLD: SEND_STRING("TRIPLE TAP HOLD x!!"); break;
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_V); register_code(KC_V); break;
        default: break;
    }
}

void tdq_copy_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: SEND_STRING(SS_LCTL("c")); break;//copy normal

        case TD_SINGLE_HOLD: //copy 1 line
                tap_code_delay(KC_HOME, 30);
                register_code(KC_LSFT);
                tap_code_delay(KC_END, 30);
                unregister_code(KC_LSFT);
                //wait_ms(50);
                tap_code16_delay(C(KC_C), 30);

         break;

        case TD_DOUBLE_TAP: //copy 1 word

                // Ctrl + Left
                register_code(KC_LCTL);
                tap_code(KC_LEFT);
                unregister_code(KC_LCTL);
                wait_ms(20);

               register_code(KC_LCTL);
               register_code(KC_LSFT);
               tap_code_delay(KC_RIGHT, 30);
               unregister_code(KC_LCTL);
               unregister_code(KC_LSFT);
               tap_code16_delay(C(KC_C), 30);

        break;

        case TD_DOUBLE_HOLD: //copy 1 paragraph
                SEND_STRING(
                  SS_LCTL(SS_LSFT(SS_TAP(X_DOWN))) SS_DELAY(50) SS_LCTL("c")
                );

        break;
        default: break;
    }
}

void tdq_paste_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:  //paste normaL

                SEND_STRING(SS_LCTL("v"));

        break;
        case TD_SINGLE_HOLD: SEND_STRING(SS_LCTL(SS_LSFT("v"))); break; //portapapeles

        case TD_DOUBLE_TAP: //paste 1 word

                        // Ctrl + Left
                        register_code(KC_LCTL);
                        tap_code(KC_LEFT);
                        unregister_code(KC_LCTL);
                        wait_ms(20);

                        register_code(KC_LCTL);
                        register_code(KC_LSFT);
                        tap_code(KC_RIGHT); // Selecciona palabra
                        unregister_code(KC_LSFT); // Soltás shift antes de pegar
                        tap_code16_delay(C(KC_V), 30); // Pega normal
                        unregister_code(KC_LCTL);

        break;

        case TD_DOUBLE_HOLD:
                      //portapapeles win
                      register_code(KC_LGUI);    // Presiona la tecla Win
                      tap_code(KC_V);            // Toca la tecla V
                      unregister_code(KC_LGUI);  // Suelta la tecla Win

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

                     // Ir al inicio
                     tap_code(KC_HOME);
                     wait_ms(20);
                     // Ctrl + Shift + End para seleccionar
                     register_code(KC_LSFT);
                     tap_code(KC_END);
                     unregister_code(KC_LSFT);
                     wait_ms(20);

                     // Ctrl + V para pegar
                     register_code(KC_LCTL);
                     tap_code(KC_V);
                     unregister_code(KC_LCTL);
         break;
        default: break;
    }
}

void tdq_cut_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:
            tap_code16(C(KC_X));  //cut normal
            break;

        case TD_SINGLE_HOLD: // Cut 1 line
            tap_code_delay(KC_HOME, 20);
            register_code(KC_LSFT);
            tap_code_delay(KC_END, 20);
            unregister_code(KC_LSFT);
            wait_ms(30);
            tap_code16(C(KC_X));
            break;

        case TD_DOUBLE_TAP: // Cut 1 word

            // Ctrl + Left
            register_code(KC_LCTL);
            tap_code(KC_LEFT);
            unregister_code(KC_LCTL);
            wait_ms(20);

            register_code(KC_LCTL);
            register_code(KC_LSFT);
            tap_code_delay(KC_RIGHT, 30);
            unregister_code(KC_LSFT);
            unregister_code(KC_LCTL);
            wait_ms(30);
            tap_code16(C(KC_X));
            break;

        case TD_DOUBLE_HOLD: //cut 1 paragrahp
                SEND_STRING(
                  SS_LCTL(SS_LSFT(SS_TAP(X_DOWN))) SS_DELAY(50) SS_LCTL("x")
                );

        break;

        default: break;
    }
}

void tdq_del_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: SEND_STRING(SS_TAP(X_BSPC)); break; //backspace

        case TD_SINGLE_HOLD: //del 1 line

                tap_code_delay(KC_HOME, 30);
                register_code(KC_LSFT);
                tap_code_delay(KC_END, 30);
                tap_code_delay(KC_BSPC, 30);
                unregister_code(KC_LSFT);
          break;

        case TD_DOUBLE_TAP: //del 1 word
               tap_code16_delay(C(KC_LEFT), 10);
               tap_code16_delay(C(S(KC_RIGHT)), 10);
               tap_code(KC_BSPC);

        break;

        case TD_DOUBLE_HOLD: //del todoo hacia la derecha en 1 linea
               tap_code16_delay(S(KC_END), 10);
               tap_code(KC_DEL);
        break;

        case TD_TRIPLE_HOLD: //del todoo hacia la izq en 1 linea
                       tap_code16_delay(S(KC_HOME), 10);
                       tap_code(KC_DEL);
                break;

        default: break;
    }
}

void tdq_toggle_hold_finished(tap_dance_state_t *state, void *user_data) {
     xtap_state.state = cur_dance(state);
        switch (xtap_state.state) {
            case TD_SINGLE_TAP: layer_invert(2); clear_all(); break;
            case TD_SINGLE_HOLD: layer_on(3);  break;
            default: break;
     }
}

void tdq_toggle_hold_reset(tap_dance_state_t *state, void *user_data) {
          switch (xtap_state.state) {
              case TD_SINGLE_HOLD:
               layer_off(3);

               break;
              default: break;

       }

     xtap_state.state = TD_NONE;
}

void tdq_bookmark_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: SEND_STRING(SS_LALT("2")); break; //show list markers
        case TD_SINGLE_HOLD: //show list markers floating window
                          register_code(KC_LSFT);   // Mantener Shift
                          tap_code(KC_F11);         // Presionar F11
                          unregister_code(KC_LSFT); // Soltar Shift
                          break;
        case TD_DOUBLE_TAP://create marker
                      tap_code(KC_F11);
                      break;

        case TD_DOUBLE_HOLD: //go to nmotecnic
                  register_code(KC_LCTL);
                  register_code(KC_LSFT);
                  tap_code(KC_F11);
                  unregister_code(KC_LSFT);
                  unregister_code(KC_LCTL);
           break;

        default: break;
    }
}

void tdq_goto_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:  tap_code16(C(KC_B)); break;
        case TD_SINGLE_HOLD: tap_code16(C(A(KC_B))); break;
        case TD_DOUBLE_TAP:  tap_code16(C(S(KC_B))); break;
        case TD_DOUBLE_HOLD: tap_code16(C(S(KC_T))); break;
        case TD_TRIPLE_HOLD: tap_code16(C(KC_U));  break;
        default: break;
    }
}

void tdq_click_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:
            tap_code(KC_BTN1);  // clic
            wait_ms(50);
            tap_code(KC_BTN1);  // clic (doble clic)
            break;

        case TD_SINGLE_HOLD:
            tap_code(KC_BTN2);  // clic derecho
            break;

        case TD_DOUBLE_TAP:
            register_code(KC_LCTL);
            tap_code(KC_BTN1);  // Ctrl + clic izquierdo
            unregister_code(KC_LCTL);
            break;


        case TD_DOUBLE_HOLD:
            tap_code(KC_BTN3);  // clic central
            break;

        default:
            break;
    }
}

void tdq_z_eng_finished(tap_dance_state_t *state, void *user_data) {
        xtap_state.state = cur_dance(state);

    switch (xtap_state.state) {
        case TD_SINGLE_TAP:
            SEND_STRING(SS_LCTL("z"));  // Deshacer
            break;
        case TD_SINGLE_HOLD:
            SEND_STRING(SS_LCTL(SS_LALT(SS_LSFT("o"))));  // reverso
            break;
        case TD_DOUBLE_TAP:
            SEND_STRING(SS_LALT("q"));  // text to speech
            break;
        case TD_DOUBLE_HOLD:
            SEND_STRING(SS_LALT("p"));  // read aloud
            break;
        default:
            break;
    }
}

void tdq_find_finished(tap_dance_state_t *state, void *user_data) {
        xtap_state.state = cur_dance(state);

    switch (xtap_state.state) {
        case TD_SINGLE_TAP: //ctrl f //find normal.
            tap_code16(C(KC_F));
            break;

        case TD_SINGLE_HOLD: //ctrl f + ctrl alt e //search on selection
                tap_code16_delay(LCTL(KC_F), 200);  // Ctrl + F
                tap_code16(LCTL(LALT(KC_E)));       // Ctrl + Alt + E
            break;

        case TD_DOUBLE_TAP: //find on 1 word
                tap_code16_delay(C(KC_RGHT), 20);
                tap_code16_delay(C(S(KC_LEFT)), 20);
                tap_code16_delay(C(KC_F), 20);
            break;

        case TD_DOUBLE_HOLD: //ctrl shift f //find in files
             tap_code16(LCTL(LSFT(KC_F)));
            break;
        default:
            break;
    }
}

void tdq_generate_finished(tap_dance_state_t *state, void *user_data) {
        xtap_state.state = cur_dance(state);

    switch (xtap_state.state) {
        case TD_SINGLE_TAP: //Generate ALT Insert
            tap_code16(A(KC_INS));
            break;

        case TD_SINGLE_HOLD: //implement interface
                tap_code16(LCTL(KC_I));
            break;

        case TD_DOUBLE_TAP: //override
            tap_code16(LCTL(KC_O));
            break;

        case TD_DOUBLE_HOLD://create test
            tap_code16(C(S(KC_T)));
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
     //if (keycode == TD(TDQ_SHIFT)) || keycode == TD(TDQ_MOVE_LY ))
//     {return 180;}

    if (QK_TAP_DANCE <= keycode && keycode <= QK_TAP_DANCE_MAX) {
        return 450;
    }

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
    NULL,               // 1
    my_layer2_layer,    // 2
    NULL,               // 3
    NULL,               // 4
    my_layer5_layer,    // 5
    my_layer6_layer,    // 6
    my_layer7_layer     // 7
);

void keyboard_post_init_user(void) {
    // Enable the LED layers
        rgblight_sethsv_noeeprom(0, 0, 0); // Blanco puro//para apagar la primera capa, que no alumbren todos los leds
        rgblight_layers = my_rgb_layers;
}

layer_state_t layer_state_set_user(layer_state_t state) {
//    rgblight_set_layer_state(INDEX_LIGHT, layer_state_cmp(state, _LAYER));
//    rgblight_set_layer_state(5, layer_state_cmp(state, 5));

    rgblight_set_layer_state(2, false); // MOVE LY OFF
    rgblight_set_layer_state(5, false); // NUMBERS LY OFF
    rgblight_set_layer_state(6, false); // MOUSE LY OFF
    rgblight_set_layer_state(7, false); // MOUSE2 LY OFF


uint8_t layer = get_highest_layer(state);

        switch (layer) {
            case 2:
                rgblight_set_layer_state(2, true); // MOVE LY
                break;
            case 5:
                rgblight_set_layer_state(5, true); // NUMBERS LY
                break;
            case 6:
                rgblight_set_layer_state(6, true); // MOUSE LY
                break;

            case 7:
                rgblight_set_layer_state(7, true); // MOUSE LY
                break;
        }
    return state;
}
























