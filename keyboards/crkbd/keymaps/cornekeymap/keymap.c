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


//macro enum
enum custom_keycodes {
    M_SEL_COPY = SAFE_RANGE,
    M_ALT_TAB,
    M_CTRL_TAB,
    SPC_CTRL_TAB,
    ALT_ARROW,
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
    PGUP_DOWN,
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
//    ONE,
//    TWO,
//    THREE,
    MOUSE_PRESSED_CLICK,
    SHOW_QUICK_ENT,
    CODE_COMPLET,
    INFOPARM,
    COMM,
    FOLDING,
    MULTICURSOR,
    EDIT_OCCURR,
    LETTER_LAST_EDIT,
    LAST_EDIT,
    RECENT_LOC,
    USAGES,
    LETTER_ENTER,
    REFACTOR,
    SHIFT_TOGGLE,
    CTRL_TOGGLE,
    ALT_TOGGLE,
    MOD_CLEAR,
    CUT,
    OSL_DEV_LY,
    TG_0,
    TG_6,


};

//combo enum
enum combos{
  CB_CTRL_Z,
  CB_CTRL_Z2,
  CB_SUPR,
  CB_SUPR_MOVE_LY,
  CB_SUPR_MOUSE_LY,
  CB_CTRL_Z_MOUSE,
  CB_TAB,
  CB_LAYER,
  CB_ALL_COPY,
  CB_CLEAR_WIN,
  CB_HIDE_WIN,
  CB_CLOSE_TAB,
  CB_CLOSE_OTHERS,
};

//Tap Dance enum
enum {
    TD_ESC_CAPS,
    TD_HOME_PGUP,
    TD_END_PGDOWN,
    TD_RABK_EQ,
    TD_LABK_EQ,
    TD_LAMBDA,
    TD_BSPC,
    TD_PASTE,
    TDQ_CTL,
    TDQ_SHIFT,
    TDQ_MOVE_LY,
    TDQ_COPY,
    TDQ_PASTE,
    TDQ_CUT,
    TDQ_DEL,
    TDQ_MOUSE_PRESSED,
    TDQ_CLICK,
    TDQ_Z_ENG,
    TDQ_TOGGLE_HOLD,
    TDQ_BOOKMARK,
    TDQ_GOTO,
    TDQ_FIND,
    TDQ_GENERATE,

};

//#####Quad enum
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

bool is_alt_tab_active = false; // ADD this near the beginning of keymap.c
uint16_t alt_tab_timer = 0;
static uint16_t timer_key;
uint16_t RGBLED_NUM = 54;

//rgb def
//const rgblight_segment_t PROGMEM layer6_light[] = RGBLIGHT_LAYER_SEGMENTS(
//    {7, 2, HSV_RED},
//    {0, 1, HSV_RED} // LED número 5, encender 1 LED, color rojo
//);
//
//const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
//    layer6_light
//);

void keyboard_post_init_user(void) {
//    rgblight_layers = my_rgb_layers;
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(0, 0, 0); // Blanco puro

}


// prototypes tap dance
void dance_rabk(tap_dance_state_t *state, void *user_data);
void dance_labk(tap_dance_state_t *state, void *user_data);
void dance_bspc(tap_dance_state_t *state, void *user_data);
void dance_paste(tap_dance_state_t *state, void *user_data);
void dance_lambda(tap_dance_state_t *state, void *user_data);


//### prototypes quad
td_state_t cur_dance(tap_dance_state_t *state);

void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);

void tdq_shift_finished(tap_dance_state_t *state, void *user_data);
void tdq_shift_reset(tap_dance_state_t *state, void *user_data);

void tdq_move_ly_finished(tap_dance_state_t *state, void *user_data);
void tdq_move_ly_reset(tap_dance_state_t *state, void *user_data);

void tdq_copy_finished(tap_dance_state_t *state, void *user_data);
void tdq_paste_finished(tap_dance_state_t *state, void *user_data);
void tdq_cut_finished(tap_dance_state_t *state, void *user_data);
void tdq_del_finished(tap_dance_state_t *state, void *user_data);
void tdq_mouse_pressed_finished(tap_dance_state_t *state, void *user_data);
void tdq_click_finished(tap_dance_state_t *state, void *user_data);
void tdq_z_eng_finished(tap_dance_state_t *state, void *user_data);
void tdq_toggle_hold_finished(tap_dance_state_t *state, void *user_data);
void tdq_toggle_hold_reset(tap_dance_state_t *state, void *user_data);
void tdq_bookmark_finished(tap_dance_state_t *state, void *user_data);
void tdq_goto_finished(tap_dance_state_t *state, void *user_data);
void tdq_find_finished(tap_dance_state_t *state, void *user_data);
void tdq_generate_finished(tap_dance_state_t *state, void *user_data);

void tdq_mouse_ly_reset(tap_dance_state_t *state, void *user_data);

//config revert to repeat key
uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {
    if ((mods & MOD_MASK_CTRL)) {  // Was Ctrl held?
        switch (keycode) {
            case KC_TAB: return C(S(KC_TAB));
        }
    }

   switch (keycode) { //Was the modifier key pressed?
            case C(KC_Z): return  C(KC_Y);
            case C(KC_Y): return  C(KC_Z);
        }

    return KC_TRNS;  // Defer to default definitions.
}

//combos
const uint16_t PROGMEM cb_ctrl_z[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM cb_ctrl_z2[] = {KC_LEFT, KC_DOWN, COMBO_END};
const uint16_t PROGMEM cb_ctrl_z_mouse[] = {MS_LEFT, MS_DOWN, COMBO_END};
const uint16_t PROGMEM cb_supr[] = {LT(2,KC_D), KC_F, COMBO_END};
const uint16_t PROGMEM cb_supr_move_ly[] = {CTRL_TOGGLE, ALT_TOGGLE, COMBO_END};
const uint16_t PROGMEM cb_supr_mouse_ly[] = {CTRL_TOGGLE, ALT_TOGGLE, COMBO_END};//pendiente. falta ponerel boton en mouse ly
const uint16_t PROGMEM cb_tab[] = {KC_F, LT(4, KC_J), COMBO_END};

const uint16_t PROGMEM cb_close_tab[]    = {KC_UP, TAB_SPLIT, COMBO_END}; //dev ly
const uint16_t PROGMEM cb_close_others[] = {M_ALT_TAB,  KC_UP, TAB_SPLIT, COMBO_END}; //dev ly

const uint16_t PROGMEM cb_layer[] = {LT(1, KC_S), LT(1, KC_L), COMBO_END}; //anulares
const uint16_t PROGMEM cb_all_copy[] = {KC_E, KC_I, COMBO_END}; //medios up
const uint16_t PROGMEM cb_clear_win[] = {LT(2, KC_D), LT(2, KC_K), COMBO_END}; //medios
const uint16_t PROGMEM cb_hide_win[] = {KC_DOWN, KC_RIGHT, COMBO_END}; //medios

//combo actions
//const uint16_t PROGMEM test_combo12[] = {TD(TDQ_SHIFT), KC_O, COMBO_END};
combo_t key_combos[] = {
   [CB_CTRL_Z]   = COMBO(cb_ctrl_z, LCTL(KC_Z)),
   [CB_CTRL_Z2]   = COMBO(cb_ctrl_z2, LCTL(KC_Z)),
   [CB_CTRL_Z_MOUSE]   = COMBO(cb_ctrl_z_mouse, LCTL(KC_Z)),
   [CB_SUPR]   =  COMBO(cb_supr, KC_DEL),
   [CB_SUPR_MOVE_LY]    =  COMBO(cb_supr_move_ly, KC_DEL),
   [CB_SUPR_MOUSE_LY]   =  COMBO(cb_supr_mouse_ly, KC_DEL),//pendiente
   [CB_TAB]      = COMBO(cb_tab, KC_TAB),
   [CB_CLOSE_TAB]         = COMBO(cb_close_tab, C(KC_F4)), //dev ly
   [CB_CLOSE_OTHERS]      = COMBO(cb_close_others, C(KC_F21)), //dev ly
   [CB_LAYER]    = COMBO(cb_layer, TG(6)),
   [CB_ALL_COPY] = COMBO(cb_all_copy, ALL_COPY),
   [CB_CLEAR_WIN] = COMBO(cb_clear_win, CLEAR_WIN),
   [CB_HIDE_WIN] = COMBO(cb_hide_win, HIDE_WIN),
};

void clear_all(void) {
    clear_mods();             // Libera Ctrl, Shift, Alt, etc.
//    clear_oneshot_mods();     // Libera modificadores One Shot
    clear_keyboard();         // Libera cualquier tecla registrada
}



//Pr record
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static bool shift_active = false;
    static bool ctrl_active  = false;
    static bool alt_active   = false;



        switch (keycode) {
            case M_SEL_COPY: SEND_STRING(SS_LCTL("ac")); break;
            case M_CTRL_TAB: if (record->event.pressed) { SEND_STRING(SS_LCTL(SS_TAP(X_TAB))); } break;
            case ALT_ARROW: if (record->event.pressed) { tap_code16(A(KC_RIGHT)); } break;
            case M_ENIE: if (record->event.pressed) { SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_KP_1) SS_TAP(X_KP_6) SS_TAP(X_KP_4) SS_UP(X_LALT)); } break;
            case WIN_D: if (record->event.pressed) { SEND_STRING(SS_LGUI("d"));  } break;

            case DOUBLE_COLON:
                if (record->event.pressed){
                   SEND_STRING(":"); //SEND_STRING(SS_TAP(X_COLON) SS_DELAY(10) SS_TAP(X_COLON));
                   }else{
                   SEND_STRING(":");
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
                  tap_code16(KC_RIGHT); // hold to send Ctrl right
                  return false;
              }
              return true; //normal tap

          case LT(1,KC_SCLN):
                if (!record->tap.count && record->event.pressed) {
                   SEND_STRING(SS_LSFT(SS_TAP(X_SCLN))); // Intercept hold function to send Ctrl-X
                    return false;
                }
                return true;

          case LT(1,KC_SLSH):
                if (!record->tap.count && record->event.pressed) {
                   SEND_STRING("\\"); // Intercept hold function to send Ctrl-X
                    return false;
                }
                return true;

          case  LT(1,KC_DQT):
               if (record->event.pressed) {
                  if (!record->tap.count) {
                     SEND_STRING(SS_TAP(X_QUOT)); // Intercept hold function to send Ctrl-X
                      return false;
                  }else{
                     SEND_STRING("\""); // Intercept hold function to send Ctrl-X
                     return false;
                      }
                  }
                  return true;

          case  LT(1,KC_EXLM):
            if (record->event.pressed) {
                 if (!record->tap.count) {
                   SEND_STRING("!="); // Intercept hold function to send Ctrl-X
                    return false;
                 }else {
                 SEND_STRING("!");
                   return false;
                   }
                 }
                 return true;

          case  LT(1,KC_LABK):
            if (record->event.pressed) {
                 if (!record->tap.count) {
                   SEND_STRING("<="); // Intercept hold function to send Ctrl-X
                    return false;
                 }else {
                   SEND_STRING("<");
                    return false;
                       }
                    }
                 return true;

          case  LT(1,KC_RABK):
           if (record->event.pressed) {
                if (!record->tap.count) {
                  SEND_STRING(">="); // Intercept hold function to send Ctrl-X
                   return false;
                }else {
                    SEND_STRING(">");
                     return false;
                        }
                    }
                return true;

          case  LT(1,KC_MINS):
                if (!record->tap.count && record->event.pressed) {
                  SEND_STRING("_"); // Intercept hold function to send Ctrl-X
                   return false;
                }
                return true;

          case  LT(1,KC_EQL):
                if (!record->tap.count && record->event.pressed) {
                   SEND_STRING("#"); // Intercept hold function to send Ctrl-X
                   return false;
                }
                return true;

          case  LT(1,KC_AT):
            if (record->event.pressed) {
                if (!record->tap.count) {
                  SEND_STRING("?"); // Intercept hold function to send Ctrl-X
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
                  SEND_STRING("`"); // Intercept hold function to send Ctrl-X
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
                  SEND_STRING("%");// Intercept hold function to send Ctrl-X
                   return false;

                } else{
                  SEND_STRING("+");
                      return false;
                  }
                } else{}
                return true;
//
//          case  LT(1,KC_Z):
//                if (!record->tap.count && record->event.pressed) {
//                  tap_code16(KC_LWIN);
//                   return false;
//                }
//                return true;

          case  LT(1,KC_AMPR):
           if (record->event.pressed) {
                if (!record->tap.count) {
                   SEND_STRING("$");// Intercept hold function to send Ctrl-X
                   return false;
                }else {
                    SEND_STRING("&");
                     return false; // Evita que se ejecute la acción por defecto
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
                 SEND_STRING("<-"); // Intercept hold function to send Ctrl-X
               }
               break;

          case  RLAMBDA:
               if (record->event.pressed) {
                 SEND_STRING("->"); // Intercept hold function to send Ctrl-X
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

            case PGUP_DOWN:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP → PAGE DOWN
                            tap_code(KC_PGDN);
                        }else {
                            // HOLD → PAGE UP
                            tap_code(KC_PGUP);
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

              case LETTER_ENTER:
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

             case LETTER_LAST_EDIT:
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

//            case ONE:
//                    if (record->event.pressed) {
//                        timer_key = timer_read(); // Inicia el temporizador
//                    }else {
//                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
//                            // TAP
//                            SEND_STRING("100");
//                        }else {
//                            // HOLD -
//                            SEND_STRING("1000");
//
//                        }
//                    }
//                    return false;
//
//            case TWO:
//                    if (record->event.pressed) {
//                        timer_key = timer_read(); // Inicia el temporizador
//                    }else {
//                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
//                            // TAP
//                            SEND_STRING("200");
//
//                        }else {
//                            // HOLD
//                            SEND_STRING("2000");
//
//
//                        }
//                    }
//                    return false;
//
//            case THREE:
//                    if (record->event.pressed) {
//                        timer_key = timer_read();
//                    }else {
//                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
//                            // TAP
//                            SEND_STRING("300");
//
//                        }else {
//                            // HOLD
//                            SEND_STRING("3000");
//
//
//                        }
//                    }
//                    return false;

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
//                reset_oneshot_layer();  // Cancela cualquier one-shot layer activa
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

        case LT(3,TG_6):
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
        }
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

//Tap Dance actions
tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC_CAPS] = ACTION_TAP_DANCE_DOUBLE(KC_ESC, KC_CAPS),
    [TD_HOME_PGUP] = ACTION_TAP_DANCE_DOUBLE(KC_HOME, KC_PGUP),
    [TD_END_PGDOWN] = ACTION_TAP_DANCE_DOUBLE(KC_END, KC_PGDN),
    [TD_RABK_EQ] =  ACTION_TAP_DANCE_FN(dance_rabk),
    [TD_LABK_EQ] =  ACTION_TAP_DANCE_FN(dance_labk),
    [TD_LAMBDA] =  ACTION_TAP_DANCE_FN(dance_lambda),
    [TD_BSPC] =  ACTION_TAP_DANCE_FN(dance_bspc),
    [TD_PASTE] =  ACTION_TAP_DANCE_FN(dance_paste),

    [TDQ_CTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished, x_reset),
    [TDQ_SHIFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_shift_finished, tdq_shift_reset),
    [TDQ_MOVE_LY] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_move_ly_finished, tdq_move_ly_reset),
    [TDQ_COPY] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_copy_finished, x_reset),
    [TDQ_PASTE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_paste_finished, x_reset),
    [TDQ_CUT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_cut_finished, x_reset),
    [TDQ_DEL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_del_finished, x_reset),
    [TDQ_MOUSE_PRESSED] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_mouse_pressed_finished, x_reset),
    [TDQ_CLICK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_click_finished, x_reset),
    [TDQ_Z_ENG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_z_eng_finished, x_reset),
    [TDQ_TOGGLE_HOLD] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_toggle_hold_finished, tdq_toggle_hold_reset),
    [TDQ_BOOKMARK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_bookmark_finished, x_reset),
    [TDQ_FIND] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_find_finished, x_reset),
    [TDQ_GENERATE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_generate_finished, x_reset),

};


 //KEY MAP
//LY 0
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                       ,-----------------------------------------------------.
   Q_ESC, LETTER_ENTER, TD(TDQ_MOUSE_PRESSED),  LETTER_LAST_EDIT, KC_T, KC_CAPS,   XXXXXXX, KC_Y, KC_U,   KC_I,  KC_O,  KC_ESC,
  //|--------+--------+--------+--------+--------+--------|                        |--------+--------+--------+--------+--------+--------|
   LSFT_T(KC_A), LT(1, KC_S), LT(2,KC_D), KC_F, KC_G, LCTL(KC_S),                  UG_TOGG,  KC_H, KC_J, KC_K, LT(1, KC_L), RSFT_T(KC_P),
  //|--------+--------+--------+--------+--------+--------|                         |--------+--------+--------+--------+--------+--------|
   KC_Z, LGUI_T(KC_X), LCTL_T(KC_C), KC_V,  KC_B, WIN_D,                           XXXXXXX, KC_N,  KC_M, CODE_COMPLET, TD(TDQ_DEL), M_ENIE,
  //|--------+--------+--------+--------+--------+--------+--------|                |--------+--------+--------+--------+--------+--------+--------|
                                      TG(2), SPC_CTRL_TAB,  XXXXXXX ,     XXXXXXX,   SHOW_QUICK_ENT, TG(5)
                                      //`--------------------------'  `--------------------------'


  ),
//symbols ly 1
    [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                           ,-----------------------------------------------------.
  KC_ASTR,  LT(1,KC_PERC), LT(1,KC_MINS),  LT(1,KC_SLSH), LT(1,ENV_VAR), XXXXXXX,     XXXXXXX, XXXXXXX, LT(1,KC_EQL), LT(1,KC_AT),  LT(1,KC_DQT), LT(1,KC_TILD),
  //|--------+--------+--------+--------+--------+--------|                           |--------+--------+--------+--------+--------+--------|
  LT(1, KC_AMPR), DOUBLE_COLON, LT(1,KC_LABK), LT(1,KC_RABK), KC_PIPE, XXXXXXX,       XXXXXXX, LT(1,KC_EXLM) , KC_DOT, CTRL_SHIFT_ENTER,  LT(1,KC_LPRN), LT(1,KC_LCBR),
  //|--------+--------+--------+--------+--------+--------|                           |--------+--------+--------+--------+--------+--------|
  XXXXXXX, KC_CIRC, LLAMBDA, RLAMBDA, XXXXXXX, QK_BOOT,                               QK_BOOT, XXXXXXX, KC_COMM, LT(1,KC_SCLN),  LT(1,LBRC2),  XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_SPC, _______,  XXXXXXX,     TO(0),   KC_TRNS, XXXXXXX
                                      //`--------------------------'  `--------------------------'
 ),
 //move ly 2

    [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
  KC_ESC, SHIFT_TOGGLE, CTRL_TOGGLE, ALT_TOGGLE, M_SEL_COPY, XXXXXXX,                   XXXXXXX, M_CTRL_TAB, M_ALT_TAB,  KC_UP, TAB_SPLIT, KC_ESC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
  TG(6), TD(TDQ_CUT), TD(TDQ_COPY), TD(TDQ_PASTE), QK_AREP, XXXXXXX,              XXXXXXX, QK_REP, KC_LEFT, KC_DOWN, KC_RIGHT, HOME_END,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
  A(KC_F), LCTL(KC_W), CODE_COMPLET, MS_BTN1, KC_F6, XXXXXXX,              XXXXXXX, ALT_ARROW, LT(6, KC_ENT), PGUP_DOWN, TD(TDQ_DEL), LCTL(LSFT(KC_M)),
  //| ------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                       LT(3,TG_0), SPC_CTRL_TAB, XXXXXXX,     TO(0), KC_TRNS, LT(3,TG_6)
                                      //`--------------------------'  `--------------------------'
  ),
  //dev ly 3
    [3] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      EDIT_OCCURR, MULTICURSOR, A(KC_J) , S(A(KC_J)), INFOPARM, XXXXXXX,            XXXXXXX, NAV_ERROR, RECENT_LOC, PROJECT_VIEW, A(KC_F12), SPLIT_WIN,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      FOLDING, C(KC_D), COMM, TD(TDQ_FIND), C(KC_R), XXXXXXX,                      XXXXXXX, USAGES, LAST_EDIT, TD(TDQ_BOOKMARK), TD(TDQ_GOTO), A(KC_F12),
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, C(A(KC_T)), REFACTOR, TD(TDQ_GENERATE), C(S(KC_U)), XXXXXXX,        XXXXXXX, FULL_SCREEN, NEW_FILE, C(A(KC_LEFT)), C(A(KC_RIGHT)), C(KC_F12),
      //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                 XXXXXXX, _______,  XXXXXXX,     TO(0),   KC_TRNS, EVERYW_ACT
                                          //`--------------------------'  `--------------------------'
   ),
   //dev2 ly 4 //not used

    [4] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                      XXXXXXX, _______,  XXXXXXX,     TO(0),   KC_TRNS, XXXXXXX
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
   KC_ESC, TD(TDQ_DEL), TD(TDQ_CLICK), MS_WHLD, CTRL_TOGGLE, XXXXXXX,        XXXXXXX, XXXXXXX, MS_WHLU, MS_UP, MS_WHLD, KC_ESC,
   //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
   TG(6), TD(TDQ_CUT), TD(TDQ_COPY), TD(TDQ_PASTE), TD(TDQ_Z_ENG), XXXXXXX,    XXXXXXX, XXXXXXX, MS_LEFT, MS_DOWN, MS_RGHT, MS_WHLR,
   //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
   MS_WHLL, MS_WHLR, MS_WHLU, MOUSE_PRESSED_CLICK, A(KC_ENT), XXXXXXX,           XXXXXXX, LCTL(KC_Z), MS_WHLL, XXXXXXX, XXXXXXX, XXXXXXX,
   //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        LT(7,KC_ENT),  SPC_CTRL_TAB,     XXXXXXX,     TO(0), KC_TRNS, TG(6)
                                       //`--------------------------'  `--------------------------'
  ), // mouse2 ly7

    [7] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_SPC, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                      //`--------------------------'  `--------------------------'
 ), //  numbers 2 layer 8

      [8] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            XXXXXXX, _______,  _______,     TO(0),   XXXXXXX, XXXXXXX
                                        //`--------------------------'  `--------------------------'
   ), //  layer

        [9] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
          XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
      //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                              KC_SPC, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                          //`--------------------------'  `--------------------------'
     ), //  layer

          [10] = LAYOUT_split_3x6_3(
        //,-----------------------------------------------------.                    ,-----------------------------------------------------.
            XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
        //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
            KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
        //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
            KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
        //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                KC_SPC, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                            //`--------------------------'  `--------------------------'
       ), //  layer

            [11] = LAYOUT_split_3x6_3(
          //,-----------------------------------------------------.                    ,-----------------------------------------------------.
              XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
          //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
              KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
          //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
              KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
          //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                  KC_SPC, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                              //`--------------------------'  `--------------------------'
         ), //  layer

             [12] = LAYOUT_split_3x6_3(
           //,-----------------------------------------------------.                    ,-----------------------------------------------------.
               XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
           //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
               KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
           //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
               KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
           //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                   KC_SPC, _______,  _______,     TO(0),   KC_TRNS, XXXXXXX
                                               //`--------------------------'  `--------------------------'
          )



};



//###tap dance imple

// > >=
void dance_rabk(tap_dance_state_t *state, void *user_data) {

   switch (state->count) {
      case 1: SEND_STRING(SS_LSFT(SS_TAP(X_DOT))); break;
      case 2: SEND_STRING(SS_LSFT(SS_TAP(X_DOT)) SS_TAP(X_EQUAL)); break;
      case 3: SEND_STRING("{}" SS_TAP(X_LEFT)); break;

   }
}

// < <=
void dance_labk(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
       SEND_STRING(SS_LSFT(SS_TAP(X_COMM)));
    } else {
       SEND_STRING(SS_LSFT(SS_TAP(X_COMM)) SS_TAP(X_EQUAL));
    }
}

// backspace, ctrl backspace, shift supr(borra una linea)
void dance_bspc(tap_dance_state_t *state, void *user_data) {
   switch (state->count) {
      case 1: SEND_STRING(SS_TAP(X_BSPC)); break;
      case 2: SEND_STRING(SS_LCTL(SS_TAP(X_BSPC))); break;
      case 3: SEND_STRING(SS_LCTL(SS_TAP(X_Y))); break;
   }
}

// v, paste, ctrl shift v (portapapeles)
void dance_paste(tap_dance_state_t *state, void *user_data) {
   switch (state->count) {
      case 1: SEND_STRING(SS_TAP(X_V)); break;
      case 2: SEND_STRING(SS_LCTL("v")); break;
      case 3: SEND_STRING(SS_LCTL(SS_LSFT("v"))); break;


   }
}

// -> (lambda)
void dance_lambda(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
       SEND_STRING(SS_TAP(X_MINS) SS_LSFT(SS_TAP(X_DOT)));
    } else {
       SEND_STRING(SS_LSFT(SS_TAP(X_COMM)) SS_TAP(X_MINS));

    }
}

//###Quad impl
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

void x_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: SEND_STRING(SS_TAP(X_V)); break;
        case TD_SINGLE_HOLD: SEND_STRING(SS_LCTL("v")); break;
        case TD_DOUBLE_TAP:SEND_STRING(SS_LCTL("v") SS_TAP(X_ENT)); break;
        case TD_DOUBLE_HOLD:SEND_STRING(SS_LCTL(SS_LSFT("v"))); break;
        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE TAP x!!"); break;
        case TD_TRIPLE_HOLD: SEND_STRING("TRIPLE TAP HOLD x!!"); break;
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_V); register_code(KC_V); break;
        default: break;
    }
}

void tdq_shift_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP:

                      tap_code(MS_BTN1);
                      register_code(MS_BTN1);

        break;

        case TD_SINGLE_HOLD: tap_code(KC_E); break;
        case TD_DOUBLE_TAP:
                              unregister_code(MS_BTN1);


//                             tap_code16_delay(KC_LPRN, 30);  // Escribe '('
//                             tap_code16_delay(KC_RPRN, 30);  // Escribe ')'
//                             tap_code16_delay(KC_SCLN, 30);
//                             tap_code_delay(KC_LEFT, 30);  // Mueve el cursor dentro de los paréntesis break;
//                             tap_code_delay(KC_LEFT, 30);  // Mueve el cursor dentro de los paréntesis
                             break;

        case TD_DOUBLE_HOLD:
        unregister_code(MS_BTN1);


        break;
        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE TAP tdq shift!!"); break;
        case TD_TRIPLE_HOLD: SEND_STRING("TRIPLE TAP HOLD tdq shift!!"); break;
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_W); register_code(KC_W); break;
        default: break;
    }
}

void tdq_shift_reset(tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
          case TD_SINGLE_HOLD: unregister_code(KC_LSFT); break;
          case TD_DOUBLE_SINGLE_TAP: clear_keyboard(); break;
          default: break;
     }
    xtap_state.state = TD_NONE;
}

void tdq_move_ly_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: tap_code(KC_SPACE); break;
        case TD_SINGLE_HOLD: layer_on(2); break;
        case TD_DOUBLE_TAP:SEND_STRING("Double hold move ly!!"); break;

//        case TD_DOUBLE_HOLD: layer_on(4); break;
        case TD_DOUBLE_HOLD:
          // Check to see if the layer is already set
                       if (layer_state_is(2)) {
                           // If already set, then switch it off
                           layer_off(2);
                       } else {
                           // If not already set, then switch the layer on
                           layer_on(2);
                       } break;


        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE move ly!!"); break;
        case TD_TRIPLE_HOLD: SEND_STRING("TRIPLE hold move ly!!"); break;
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_W); register_code(KC_W); break;
        default: break;
    }
}
void tdq_move_ly_reset(tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    //behavior as Momentary layer
//    if (xtap_state.state == TD_SINGLE_HOLD) {
//        layer_off(3);
//    }

//      if (ql_tap_state.state == TD_DOUBLE_HOLD) {
//            layer_off(4);
//        }
//
       xtap_state.state = TD_NONE;
}

void tdq_copy_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: SEND_STRING(SS_LCTL("c")); break;
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
        case TD_SINGLE_TAP:  //normaL

                SEND_STRING(SS_LCTL("v"));

        break;
        case TD_SINGLE_HOLD: SEND_STRING(SS_LCTL(SS_LSFT("v"))); break; //portapapeles

        case TD_DOUBLE_TAP: //pega en una palabra

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

                      //paste as plain text disabled
//                    register_code(KC_LCTL);
//                    register_code(KC_LSFT);
//                    register_code(KC_LALT);
//                    tap_code(KC_V);
//                    unregister_code(KC_LALT);
//                    unregister_code(KC_LSFT);
//                    unregister_code(KC_LCTL);
        break;

         case TD_TRIPLE_TAP: // pega en una linea

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
            tap_code16(C(KC_X));  //normal
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
        case TD_SINGLE_TAP: SEND_STRING(SS_TAP(X_BSPC)); break;
        case TD_SINGLE_HOLD: //del 1 line

                tap_code_delay(KC_HOME, 30);
                register_code(KC_LSFT);
                tap_code_delay(KC_END, 30);
                tap_code_delay(KC_BSPC, 30);
                unregister_code(KC_LSFT);
          break;

        case TD_DOUBLE_TAP: SEND_STRING(SS_LCTL(SS_TAP(X_BSPC))); break; //del 1 word
        case TD_DOUBLE_HOLD: //press supr
            tap_code(KC_DEL);
        break;
        default: break;
    }
}

void tdq_mouse_pressed_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: //click pressed
                      tap_code(MS_BTN1);
                      register_code(MS_BTN1);
        break;

        case TD_SINGLE_HOLD: tap_code(KC_E); break;
        case TD_DOUBLE_TAP: //doble click
                        tap_code(KC_BTN1);
                        wait_ms(50);
                        tap_code(KC_BTN1);
         break;

        case TD_DOUBLE_HOLD: //triple click
                        tap_code(KC_BTN1);
                        wait_ms(50);
                        tap_code(KC_BTN1);
                        wait_ms(50);
                        tap_code(KC_BTN1);

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
        case TD_SINGLE_TAP: SEND_STRING(SS_LALT("2")); break;
        case TD_SINGLE_HOLD:
                          register_code(KC_LSFT);   // Mantener Shift
                          tap_code(KC_F11);         // Presionar F11
                          unregister_code(KC_LSFT); // Soltar Shift
                          break;
        case TD_DOUBLE_TAP:
                      register_code(KC_LCTL);
                      tap_code(KC_F11);
                      unregister_code(KC_LCTL);
                      break;

        case TD_DOUBLE_HOLD:
                  register_code(KC_LCTL);
                  register_code(KC_LSFT);
                  tap_code(KC_F11);
                  unregister_code(KC_LSFT);
                  unregister_code(KC_LCTL);
           break;

        case TD_TRIPLE_HOLD: tap_code(KC_F11);  break;
        default: break;
    }
}

void tdq_goto_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: SEND_STRING(SS_LCTL("b")); break;
        case TD_SINGLE_HOLD: SEND_STRING(SS_LCTL(SS_LALT("b"))); break;
        case TD_DOUBLE_TAP: SEND_STRING(SS_LCTL(SS_LSFT("b"))); break;
        case TD_DOUBLE_HOLD: SEND_STRING(SS_LCTL(SS_LSFT("t"))); break;
        case TD_TRIPLE_HOLD: SEND_STRING(SS_LCTL("u"));  break;
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
        case TD_SINGLE_TAP: //ctrl f //find
            tap_code16(C(KC_F));
            break;

        case TD_SINGLE_HOLD: //ctrl f + ctrl alt e //search on selection
                tap_code16_delay(LCTL(KC_F), 200);             // Ctrl + F con 200 ms de delay después
                tap_code16(LCTL(LALT(KC_E)));       // Ctrl + Alt + E con 200 ms de delay después
            break;

        case TD_DOUBLE_TAP: //ctrl shift f //find in files
            tap_code16(LCTL(LSFT(KC_F)));
            break;

//        case TD_DOUBLE_HOLD:
//            SEND_STRING(SS_LALT("p"));
//            break;
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
//  if (xtap_state.state == TD_DOUBLE_SINGLE_TAP) {
             //clear_keyboard();
  //  }
    xtap_state.state = TD_NONE;
}

//Quad END


//tapping term
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {

//#cuando se quiere poner exception a un Tap dance
     //if (keycode == TD(TDQ_SHIFT)) || keycode == TD(TDQ_MOVE_LY )   { TD(TDQ_SHIFT)
     if (keycode == TD(TDQ_MOVE_LY))   {
        return TAPPING_TERM;
    }

    if (keycode == TD(TDQ_MOUSE_PRESSED))   {
        return 180;
    }

    if (QK_TAP_DANCE <= keycode && keycode <= QK_TAP_DANCE_MAX) {
        return 450;
    }

     return TAPPING_TERM;


  }


//rgb IMP

void apagar_todos_los_leds(void) {
    if (!rgblight_is_enabled()) {
        return;
    }

    for (uint8_t i = 0; i < RGBLED_NUM; i++) {   // <<< --- usamos RGBLED_NUM
        rgblight_setrgb_at(0x00, 0x00, 0x00, i);
    }
}



// Función que cambia el color según la capa, usando switch
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case 2: //move ly
              apagar_todos_los_leds();  // Primero apaga todos
              rgblight_setrgb_at(0x00, 0xFF, 0x64, 11);  // Verde claro en LED 24
              break;
        case 3:  // dev ly
              apagar_todos_los_leds();  // Primero apaga todos
//              rgblight_setrgb_at(0x00, 0x80, 0xFF, 40);    // azul claro
              break;
        case 5:  // number ly
               apagar_todos_los_leds();  // Primero apaga todos
               rgblight_setrgb_at(219, 219, 219, 1);;  // plateado suave en LED 24
               break;
        case 6:  // mouse ly
              apagar_todos_los_leds();  // Primero apaga todos
              rgblight_setrgb_at(0xFF, 0x64, 0x00, 11);  // Rojo suave en LED 24
              break;



        default:  // Si no estamos en capa 2 ni 6
//            rgblight_setrgb(0x00, 0x00, 0x00);  // Apagar todos los LEDs
              apagar_todos_los_leds();  // Primero apaga todos

//            rgblight_setrgb_at(0x00, 0x00, 0x00, 24);  // Apaga LED 24
//              rgblight_setrgb_at(0x00, 0x00, 0x00, 11);  // Apaga LED 11
            break;
    }

    return state;
}

//led 11 F

























