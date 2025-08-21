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


//Macro enum
enum custom_keycodes {
    GUI_E = SAFE_RANGE,
    ARROW_CTRL_LEFT,
    ARROW_CTRL_RIGHT,
    CTRLW_L4,
    SPACE_BASE,
    ALT_TAB,
    AMP_DOUBLE,
    EXC_DLR,
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

static uint16_t timer_key;

bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;


static bool shift_active = false;

//for doubles key





uint16_t b_timer = 0;
uint16_t n_timer = 0;



bool n_sent = false;

//vars

// Variables para LEFT
static uint16_t timer_key_left;
static bool is_hold_left = false;
static bool is_pressed_left = false;

// Variables para RIGHT
static uint16_t timer_key_right;
static bool is_hold_right = false;
static bool is_pressed_right = false;



//static uint16_t space_base_timer = 0;
//static bool sr_repeat = false;

static bool space_base_double_tap = false;



static bool dc_pressed = false;
static bool dc_hold_executed = false;



bool shift_toggle_pressed = false;
bool shift_toggle_is_hold = false;
uint16_t shift_toggle_timer = 0;

bool ms_acl0_active = false;



bool para_up_pressed = false;
bool para_up_sent = false;
uint16_t para_up_timer = 0;

// Variables para PAGE_PARAGRAPH_DOWN
bool para_down_pressed = false;
bool para_down_sent = false;
uint16_t para_down_timer = 0;




















bool voice_a_is_pressing = false;
uint16_t voice_a_timer = 0;
bool voice_a_hold_executed = false;


bool voice_mode = false;
bool voice_pressed = false;
uint16_t voice_timer = 0;
bool voice_hold_sent = false;


bool mouse_hold_handled = false;
uint16_t mouse_hold_timer = 0;
bool mouse_key_pressed = false;


static bool defer_copy = false;
static bool defer_cut = false;
static uint16_t defer_timer_copy = 0;
static uint16_t defer_timer_cut = 0;





bool chatgpt_pressed = false;
bool chatgpt_hold_executed = false;
uint16_t chatgpt_timer = 0;


// Prototypes quad

td_state_t cur_dance(tap_dance_state_t *state);
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);

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
//const uint16_t PROGMEM cb_ctrl_z[] = {KC_J, LT(2,KC_K), COMBO_END}; //base right ly //indice medio
//combo actions
//combo_t key_combos[] = {
// [CB_CTRL_Z]   = COMBO(cb_ctrl_z, LCTL(KC_Z)), //base right
 //};

void clear_all(void) {
    clear_mods();             // Libera Ctrl, Shift, Alt, etc.
    clear_keyboard();         // Libera cualquier tecla registrada

    if (is_alt_tab_active)   {
       unregister_code(KC_LALT);
       unregister_code(KC_TAB);
       is_alt_tab_active = false;
       }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

       switch (keycode) {


//PR record
        case ARROW_CTRL_LEFT:
            if (record->event.pressed) {
                timer_key_left = timer_read();
                is_hold_left = false;
                is_pressed_left = true;
            } else {
                is_pressed_left = false;
                if (!is_hold_left) {
                    // TAP → flecha izquierda
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
                    // TAP → flecha derecha
                    tap_code(KC_RGHT);
                }
            }
            return false;



            case LT(4, CTRLW_L4):
               if (record->event.pressed) {
                   if (!record->tap.count) {
                       return true; // HOLD: QMK activa capa 4
                   } else {
                          tap_code16(C(KC_W));  // TAP → Ctrl+W
                       return false; // No enviar el KC original
                   }
               }
               return true;

            case LT(2,SPACE_BASE):

                if (record->event.pressed) {
                    if (record->tap.count == 2) {
                        // DOBLE TAP → activar espacio sostenido
                        space_base_double_tap = true;
                        register_code(KC_SPC);
                        return false;
                    }
                    if (record->tap.count == 0) {
                        // HOLD sin taps → cambiar capa base
                            layer_invert(1);
                        return false;
                    }
                    if (record->tap.count == 1) {
                        // TAP simple → un espacio
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


            case LT(4, ALT_TAB):
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
                    SEND_STRING(SS_LGUI("d"));
                    }
                    break;

case  LT(2,EXC_DLR):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
        tap_code16(KC_DLR); // $

                       return false;
                    }else {
                        tap_code16(KC_EXLM); // !

                       return false;
                         }
                     }
                       return false;


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
                        // DOBLE TAP → activar espacio sostenido
                        space_base_double_tap = true;
                        register_code(KC_SPC);
                        return false;
                    }
                    if (record->tap.count == 0) {
                        // HOLD → activar momentáneamente capa 3
                        return true;
                    }
                    if (record->tap.count == 1) {
                        // TAP simple → soltar Alt si está activo y enviar Espacio
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




            case  LT(2,GUI_E):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        tap_code(KC_LGUI);
                       return false;
                    }else {
                        tap_code(KC_E);
                       return false;
                         }
                     }
                       return false;



            case  LT(2,EQUAL_DBL):
                             if (record->event.pressed) {
                                if (!record->tap.count) {
                                    tap_code16(KC_EQUAL);  // hold ==
                                    tap_code16(KC_EQUAL);
                                   return false;
                                }else {
                                    tap_code16(KC_EQUAL);  // TAP: =
                                   return false;
                                     }
                                 }
                                   return false;


            case  LT(12,Z_UNDO):
                     if (record->event.pressed) {
                        if (!record->tap.count) {
                            tap_code16(C(KC_Y));
                           return false;
                        }else {
                            tap_code16(C(KC_Z));
                           return false;
                             }
                         }
                    return false;


            case TRIPLE_WHLD:
                if (record->event.pressed) {
                    for (int i = 0; i < 8; i++) {
                        tap_code16(MS_WHLD);
                        wait_ms(10);
                    }
                    tap_code16(KC_ENT);
                }
                return false;




/*
      case LT(1, KC_RIGHT):
            if (record->event.pressed) {
                if (record->tap.count == 2) {
                    // DOBLE TAP + (mantener) → repetir en ciclos de 5
                    sr_repeat = true;
                    timer_key  = timer_read();
                    return false;
                }
                if (record->tap.count == 0) {
                    // HOLD → 10 flechas inmediatas
                    for (int i = 0; i < 40; i++) {
                        tap_code(KC_RGHT);
                    }
                    return false;
                }
                if (record->tap.count == 1) {
                    // TAP simple → 1 flecha
                    tap_code(KC_RGHT);
                    return false;
                }
            } else {
                // Al soltar, detener el ciclo (si estaba activo)
                if (sr_repeat) {
                    sr_repeat = false;
                }
            }
            return true;*/






            case  LT(2,HASH_CIRC):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        tap_code16(KC_CIRC);  // HOLD: ^
                       return false;
                    }else {
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


           case VOICE_A:
               if (record->event.pressed) {
                   voice_a_is_pressing = true;
                   voice_a_timer = timer_read();
                   voice_a_hold_executed = false;
               } else {
                   voice_a_is_pressing = false;
                   if (!voice_a_hold_executed) {
                       // TAP: Alt + Shift + B
                       register_code(KC_LALT);
                       register_code(KC_LSFT);
                       tap_code(KC_B);
                       unregister_code(KC_LSFT);
                       unregister_code(KC_LALT);
                   }
               }
               return false;

            case VOICE:
                if (record->event.pressed) {
                    voice_pressed = true;
                    voice_hold_sent = false;
                    voice_timer = timer_read();
                } else {
                    voice_pressed = false;
                    if (!voice_hold_sent) {
                        // TAP: Activar/Desactivar dictado y cambiar idioma
                        if (!voice_mode) {

//                            wait_ms(100);
//                            tap_code16(C(KC_L)); //
                            wait_ms(100);
                            tap_code16(G(KC_SPC));
                            wait_ms(100);
                            tap_code16(G(KC_H));   // Activar dictado
                                voice_mode = true;
                        } else {
                            wait_ms(100);
                            tap_code16(C(KC_L)); // Cambiar  IDIOMA
                            wait_ms(100);
                            tap_code(KC_ESC);      // Detener dictado
                            wait_ms(100);
                            tap_code16(G(KC_SPC));
                            voice_mode = false;
                        }
                    }
                }
                return false; // Ya lo manejamos


            case CHATGPT:
              if (record->event.pressed) {
                chatgpt_pressed = true;
                chatgpt_hold_executed = false;
                chatgpt_timer = timer_read();  // Inicia temporizador
            } else {
                chatgpt_pressed = false;

                if (!chatgpt_hold_executed) {
                    // TAP → 3 TABs + ENTER
                    tap_code_delay(KC_ENT, 30);
                    tap_code_delay(KC_TAB, 30);
                    tap_code_delay(KC_TAB, 30);
                    tap_code_delay(KC_TAB, 30);
                    tap_code_delay(KC_ENT, 30);
                }
            }
                return false;

            case DOUBLE_COLON:
                if (record->event.pressed) {
                    dc_pressed = true;
                    dc_hold_executed = false;
                    timer_key = timer_read();
                } else {
                    if (!dc_hold_executed) {
                        // TAP → un solo ":"
                        tap_code16(KC_COLN);
                    }
                    dc_pressed = false;
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
                     tap_code16(LGUI(KC_X));
                     wait_ms(400);
                     tap_code(KC_U);
                     wait_ms(300);
                     tap_code(KC_S);
                   }
              break;

            case HIBERNATE:
                  if (record->event.pressed) {
                     tap_code16 (LGUI(KC_X));
                     wait_ms(400);
                     tap_code(KC_U);
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
                   tap_code_delay(KC_HOME, 10);
                   register_code(KC_LSFT);
                   tap_code_delay(KC_END, 10);
                   tap_code_delay(KC_BSPC, 10);
                   unregister_code(KC_LSFT);
                 }
                 break;


            case LT(0,KC_DOWN):
              if (!record->tap.count && record->event.pressed) {
                  tap_code16(KC_RIGHT); // hold
                  return false;
              }
              return true; //normal tap


            case LT(2,KC_SLSH):
                if (!record->tap.count && record->event.pressed) {
                   SEND_STRING("\\"); // hold
                    return false;
                }
                return true;

            case  LT(2,KC_DQT):
                   if (record->event.pressed) {
                      if (!record->tap.count) {

                         tap_code16(KC_GRAVE); // holdO
                          return false;
                      }else{
                         SEND_STRING("\""); // tap
                         return false;
                          }
                      }
                      return true;

            case NOT_EQUAL:
                if (record->event.pressed) {
                    send_string("!="); // Envía  !=
                }
                return false;


            case  LT(2,KC_LABK):
                  if (record->event.pressed) {
                     if (!record->tap.count) {//hold
                        SEND_STRING("<=");
                        return false;
                     }
                     else {
                     tap_code16(KC_LABK); //tap <
                        return false;
                           }
                        }
                     return true;

            case  LT(2,KC_RABK):
                      if (record->event.pressed) {
                        if (!record->tap.count) {
                           SEND_STRING(">=");
                             return false;
                        }else {
                            tap_code16(KC_GT);
                             return false;
                            }
                        }
                    return true;

            case  LT(2,KC_MINS):
                  if (record->event.pressed) {
                       if (!record->tap.count) {
                         tap_code(KC_MINS);
                       return false;
                    }else{
                         tap_code16(S(KC_MINS));
                         return false;
                     }
                    }
                    return true;

            case  LT(2,KC_AT):
                  if (record->event.pressed) {
                    if (!record->tap.count) {
//                      SEND_STRING("?"); // hold
                      tap_code16(KC_QUES);
                       return false;
                    }else {
                       tap_code16(KC_AT);
                         return false;
                        }
                    }
                    return true;

            case  AMP_DOUBLE :
                 if (record->event.pressed) {
                        tap_code16(KC_AMPR); // &
                        tap_code16(KC_AMPR); // &
                    }
                    return false; // no procesar más

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

            case  LT(2,KC_LPRN):
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

            case  LT(2,KC_LCBR):
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

            case  LT(2,LBRC2):
                 if (record->event.pressed) {
                    if (!record->tap.count) {//hold
                      SEND_STRING("]");
                       return false;
                    }else {//tap
                       SEND_STRING("[");
                       return false;
                         }
                     }
                       return false;



            case  LT(3,KC_ENT):
                   if (record->event.pressed) {
                      clear_all();
                       }

                    return true;

            case  LT(0,HOME_END):
                 if (record->event.pressed) {
                    if (!record->tap.count) {

                         tap_code(KC_HOME);//HOLD

                       return false;
                    }else {

                        tap_code(KC_END);//TAP

                       return false;
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


            case PAGE_PARAGRAPH_UP:
                if (record->event.pressed) {
                    para_up_pressed = true;
                    para_up_sent = false;
                    para_up_timer = timer_read();
                } else {
                    if (!para_up_sent) {
                        // TAP
                        tap_code16(A(KC_PGUP));
                    }
                    para_up_pressed = false;
                }
                return false;

            case PAGE_PARAGRAPH_DOWN:
                if (record->event.pressed) {
                    para_down_pressed = true;
                    para_down_sent = false;
                    para_down_timer = timer_read();
                } else {
                    if (!para_down_sent) {
                        // TAP
                        tap_code16(A(KC_PGDN));
                    }
                    para_down_pressed = false;
                }
                return false;


            case  LT(0, SEL_WORD_PARAGRAPH):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        // HOLD → seleccionar párrafo (ejecutar inmediatamente)
                        tap_code16_delay(KC_END,10);
                        tap_code16_delay(KC_HOME,10);
                        tap_code16_delay(KC_HOME,30);
                        register_code(KC_LSFT);
                        tap_code16(LALT(KC_PGDN));
                        unregister_code(KC_LSFT);
                       return false;
                    }else {

                        // TAP → seleccionar palabra
                        tap_code16_delay(C(KC_LEFT), 10);
                        tap_code16_delay(C(S(KC_RIGHT)), 10);

                       return false;
                         }
                     }
                       return false;


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



            case  LT(2,N_ENIE):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_KP_1) SS_TAP(X_KP_6) SS_TAP(X_KP_4) SS_UP(X_LALT));
                       return false;
                    }else {
                        tap_code(KC_N);
                       return false;
                         }
                     }
                       return false;




            case  LT(2,B_V):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        tap_code(KC_V);
                       return false;
                    }else {
                        tap_code(KC_B);
                       return false;
                         }
                     }
                       return false;



            case  LT(2,F_W):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        tap_code(KC_W);
                       return false;
                    }else {
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

            case MAX_MIN_WIN:
                    if (record->event.pressed) {
                        timer_key = timer_read(); // Inicia el temporizador
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                            // TAP →  MAX WIN
                            tap_code16(RGUI(KC_UP));
                        }else {
                            // HOLD →  MIN WIN
                            tap_code16(RGUI(KC_DOWN));
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


            case  LT(2,RECENT_LOC):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        SEND_STRING(SS_LCTL(SS_LSFT("e")));
                       return false;
                    }else {
                        SEND_STRING(SS_LCTL("e"));
                       return false;
                         }
                     }
                       return false;





            case USAGES:
                    if (record->event.pressed) {
                        timer_key = timer_read();
                    }else {
                        if (timer_elapsed(timer_key) < TAPPING_TERM) {
                        //tap
                            tap_code16(C(A(KC_F7)));
                        }else {
                        //hold
                              tap_code16(A(KC_F7));

                        }
                    }
                    return false;

         case  LT(2,COMM):
             if (record->event.pressed) {
                if (!record->tap.count) {
                   if (shift_active) {
                         unregister_code(KC_LSFT);
                         shift_active = false;
                           }

                        tap_code16(LCTL(LSFT(KC_SLSH)));
                   return false;
                }else {
                    if (shift_active) {
                          unregister_code(KC_LSFT);
                          shift_active = false;
                        }

                        tap_code16(LCTL(KC_SLSH));
                   return false;
                     }
                 }
                   return false;



        case MOUSE_PRESSED_CLICK:
            if (record->event.pressed) {
                mouse_key_pressed = true;
                mouse_hold_handled = false;
                mouse_hold_timer = timer_read();
            } else {
                mouse_key_pressed = false;

                if (!mouse_hold_handled) {
                    // TAP - mouse click
                    tap_code(MS_BTN1);

                    register_code(MS_BTN1);  // Puedes quitar esto si hace doble clic
                }
            }
            return false;



            case  LT(2, SHOW_QUICK_ENT):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                      tap_code16(LCTL(KC_F1));  // HOLD: CTRL + F1
                       return false;
                    }else {
                       // TAP: ALT + ENTER (si no fue HOLD)
                        tap_code16(LALT(KC_ENT));
                       return false;
                         }
                     }
                       return false;




            case  LT(2,CODE_COMPLET):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        // HOLD - code completion adv
                         tap_code16(LCTL(LSFT(KC_SPACE)));

                       return false;
                    }else {
                            // TAP - code completion basic
                            tap_code16(LCTL(KC_SPACE));
                       return false;
                         }
                     }
                       return false;


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
                            tap_code16(KC_F22);
                        }else {
                            // HOLD - contrae o cierra todos
//                            tap_code16(LCTL(LSFT(KC_KP_MINUS)));

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


            case  LT(2,REFACTOR):
                 if (record->event.pressed) {
                    if (!record->tap.count) {
                        tap_code16(LSFT(KC_F6));  // Rename
                       return false;
                    }else {
                         tap_code16(LCTL(LALT(LSFT(KC_T))));  // Refactor This
                       return false;
                         }
                     }
                       return false;



        case SHIFT_TOGGLE:
            if (record->event.pressed) {
                shift_toggle_pressed = true;
                shift_toggle_is_hold = false;
                shift_toggle_timer = timer_read();
            } else {
                // Tecla soltada
                if (!shift_toggle_is_hold) {
                    // TAP → Toggle Shift
                    shift_active = !shift_active;
                    if (shift_active) {
                        register_code(KC_LSFT);
                    } else {
                        unregister_code(KC_LSFT);
                    }
                } else {
                    // HOLD → soltar Shift y Alt
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LALT);
                }
                shift_toggle_pressed = false;
                shift_toggle_is_hold = false;
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
                    if (shift_active) { unregister_code(KC_LSFT); shift_active = false; }
                    clear_all();
                    tap_code(KC_ESC);
                }
                break;

            case TO(0): clear_all(); break;

            case LT(2,TG_0):
                  if (record->event.pressed) {
                       if (!record->tap.count) {
                          return true; //hold
                       }else {
                           clear_all();
                           layer_invert(1); //tap

                            return false;
                               }
                           }
                       return true;



            case LT(12,TG_6):
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

//timer for  macro
void matrix_scan_user(void) {

//matrix
//    static uint16_t rpt_timer;

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

/*
    if (sr_repeat) {
        if (timer_elapsed(timer_key) > 130) {
            for (int i = 0; i < 5; i++) {
                tap_code(KC_RGHT);
            }
            timer_key = timer_read();
        }
    }*/



    if (dc_pressed && !dc_hold_executed) {
        if (timer_elapsed(timer_key) > 200) {  // Tiempo para detectar HOLD
            // HOLD → "::" inmediatamente
            tap_code16(KC_COLN);
            tap_code16(KC_COLN);
            dc_hold_executed = true;
        }
    }


    if (shift_toggle_pressed && !shift_toggle_is_hold) {
        if (timer_elapsed(shift_toggle_timer) > TAPPING_TERM) {
            shift_toggle_is_hold = true;
            register_code(KC_LSFT);
            register_code(KC_LALT);
        }
    }



    if (para_up_pressed && !para_up_sent && timer_elapsed(para_up_timer) > TAPPING_TERM) {
        // HOLD para PAGE_PARAGRAPH_UP
        tap_code16(A(KC_PGUP));
        tap_code(KC_DOWN);
        tap_code(KC_END);
        para_up_sent = true;
    }

    if (para_down_pressed && !para_down_sent && timer_elapsed(para_down_timer) > TAPPING_TERM) {
        // HOLD para PAGE_PARAGRAPH_DOWN
        tap_code16(A(KC_PGDN));
        tap_code(KC_UP);
        tap_code(KC_END);
        para_down_sent = true;
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




        if (chatgpt_pressed && !chatgpt_hold_executed) {
            if (timer_elapsed(chatgpt_timer) > TAPPING_TERM) {
                // HOLD → 2 TABs + ENTER
                tap_code_delay(KC_TAB, 30);
                tap_code_delay(KC_TAB, 30);
                tap_code_delay(KC_ENT, 30);

                chatgpt_hold_executed = true;
            }
        }

        if (voice_pressed && !voice_hold_sent) {
            if (timer_elapsed(voice_timer) > 500) { // HOLD (>500 ms)
                voice_hold_sent = true;
                tap_code16(G(KC_SPC)); // Solo cambia idioma a español
            }
        }

        if (voice_a_is_pressing && !voice_a_hold_executed) {
            if (timer_elapsed(voice_a_timer) > 120) {
                voice_a_hold_executed = true;
                // HOLD: Ctrl + Win + S
                register_code(KC_LCTL);
                register_code(KC_LGUI);
                tap_code(KC_S);
                unregister_code(KC_LGUI);
                unregister_code(KC_LCTL);
            }
        }


   if (mouse_key_pressed && !mouse_hold_handled && timer_elapsed(mouse_hold_timer) > TAPPING_TERM) {
       // HOLD detectado antes de soltar la tecla
       tap_code(MS_BTN1);
       mouse_hold_handled = true;  // Solo ejecutar una vez
   }


      /*if (shift_active) {
        if (timer_elapsed(shift_toggle_timer) > 30000) {
            unregister_code(KC_LSFT);
            shift_active = false;
      }
    }
*/




     }





//Quad actions
tap_dance_action_t tap_dance_actions[] = {

    [TDQ_COPY] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_copy_finished, x_reset),
    [TDQ_PASTE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_paste_finished, x_reset),
    [TDQ_CUT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_cut_finished, x_reset),
    [TDQ_GOTO] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_goto_finished, x_reset),
    [TDQ_BOOKMARK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_bookmark_finished, x_reset),
    [TDQ_FIND] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_find_finished, x_reset),
    [TDQ_REPLACE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_replace_finished, x_reset),
    [TDQ_OVERRIDE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_override_finished, x_reset),

};


 //KEY MAP
  //LY 0
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      KC_ESC, LT(2,TG_0), M_ALT_TAB, LT(4, ALT_TAB), TD(TDQ_CUT), QK_BOOT,          QK_BOOT, KC_F20, LT(4, CTRLW_L4), KC_UP, LT(2,KC_TAB), SHIFT_TOGGLE,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      LT(11,KC_ENT), LT(9,KC_TAB), TD(TDQ_COPY), TD(TDQ_PASTE), LT(12,Z_UNDO), C(KC_S),        SLEEP, C(KC_A), ARROW_CTRL_LEFT, KC_DOWN, ARROW_CTRL_RIGHT, LT(0,HOME_END),
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, LT(12,TG_6), LT(5,KC_F3), MOUSE_PRESSED_CLICK, ARROW_CTRL_LEFT, WIN_D,     HIBERNATE, XXXXXXX, LT(2, SHOW_QUICK_ENT), LT(2,CODE_COMPLET), KC_BSPC, LT(0, SEL_WORD_PARAGRAPH),
      //| ------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           LT(3,KC_SPACE), SHIFT_TOGGLE, KC_LALT,     TO(0), XXXXXXX, LT(3,KC_ENT)
                                           //`--------------------------'  `--------------------------'
      ),


     //alfa ly
    [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                       ,-----------------------------------------------------.
    KC_TRNS, LT(2,KC_Q), LT(2,GUI_E), KC_R, KC_T, XXXXXXX,                              XXXXXXX, KC_Y, KC_U, KC_I, LT(2,KC_O), KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                        |--------+--------+--------+--------+--------+--------|
    LT(11,KC_A), LT(12,KC_S), KC_D, LT(2,F_W), KC_G , C(KC_S),                          SLEEP,  KC_H, KC_J, KC_K, KC_L, KC_P,
  //|--------+--------+--------+--------+--------+--------|                         |--------+--------+--------+--------+--------+--------|
    KC_Z, KC_X, LT(5,KC_C), LT(2,B_V), XXXXXXX, WIN_D,                                  HIBERNATE, XXXXXXX,  KC_M, LT(2,CODE_COMPLET), KC_BSPC, LT(2,N_ENIE),
  //|--------+--------+--------+--------+--------+--------+--------|                |--------+--------+--------+--------+--------+--------+--------|
                                       LT(2,SPACE_BASE), LSFT_T(KC_CAPS), XXXXXXX,     TO(0),  TG(5),  RSFT_T(KC_ENT)
                                      //`--------------------------'  `--------------------------'


  ),

//symbols ly 1

    [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                           ,-----------------------------------------------------.
 KC_PERC, KC_PLUS, LT(2,KC_MINS), LT(2,KC_SLSH),  OPEN_EXCL, XXXXXXX,               XXXXXXX, OPEN_QUEST, LT(2,KC_AT), LT(2,EQUAL_DBL),  LT(2,KC_DQT), KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                           |--------+--------+--------+--------+--------+--------|
 KC_ASTR, LT(2,EXC_DLR), LLAMBDA, RLAMBDA, DOUBLE_PIPE, XXXXXXX,               XXXXXXX, AMP_DOUBLE, KC_DOT, LT(2,KC_LPRN), LT(2,KC_LCBR), LT(2,LBRC2),
  //|--------+--------+--------+--------+--------+--------|                           |--------+--------+--------+--------+--------+--------|
 S(KC_GRAVE), LT(2,HASH_CIRC) , LT(2,KC_LABK), LT(2,KC_RABK), XXXXXXX, QK_BOOT,                        QK_BOOT, XXXXXXX, KC_COMM, KC_SCLN, NOT_EQUAL,  DOUBLE_COLON,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          C(S(KC_ENT)), XXXXXXX,  XXXXXXX,     TO(0), XXXXXXX, TRIPLE_WHLD
                                      //`--------------------------'  `--------------------------'
 ),


  //dev ly 3
    [3] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      FOLDING, MULTICURSOR, A(KC_J) , S(A(KC_J)), INFOPARM, XXXXXXX,               XXXXXXX, NAV_ERROR, A(KC_F12), PROJECT_VIEW, NEW_FILE, SPLIT_WIN,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      C(A(KC_T)), LT(2,COMM), TD(TDQ_FIND), TD(TDQ_REPLACE), EDIT_OCCURR, XXXXXXX,               XXXXXXX, XXXXXXX, LAST_EDIT, C(A(KC_LEFT)), C(A(KC_RIGHT)), C(S(KC_F12)),
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      A(KC_Q), C(KC_D), LT(2,REFACTOR), TD(TDQ_OVERRIDE), XXXXXXX, QK_BOOT,              QK_BOOT, XXXXXXX, TD(TDQ_GOTO), USAGES, LT(2,RECENT_LOC), C(KC_F12),
      //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                 MAX_MIN_WIN, FULL_SCREEN, XXXXXXX,     TO(0),   C(S(KC_U)), EVERYW_ACT
                                          //`--------------------------'  `--------------------------'
   ),
   //bookmark ly 4

    [4] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     C(KC_F20), C(KC_F19), MO(8), C(S(KC_F21)), C(KC_F22), XXXXXXX,             XXXXXXX, XXXXXXX, C(KC_7), C(KC_8), C(KC_9), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    XXXXXXX, XXXXXXX, MO(8), C(KC_F17), C(KC_F18), XXXXXXX,                    XXXXXXX, XXXXXXX, TD(TDQ_BOOKMARK), C(KC_4), C(KC_5), C(KC_6),
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    XXXXXXX, C(KC_F13), C(KC_F14), C(KC_F15), XXXXXXX, XXXXXXX,                  XXXXXXX, XXXXXXX, C(KC_1), C(KC_2), C(KC_3), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                      MO(8), XXXXXXX,  XXXXXXX,     TO(0),   XXXXXXX, MO(8)
                       //`--------------------------'  `--------------------------'
),
 //numbers ly 5

    [5] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    XXXXXXX, LT(2,KC_PERC), LT(2,KC_MINS), LT(2,KC_SLSH), XXXXXXX, XXXXXXX,      XXXXXXX, KC_BSPC, KC_7, KC_8, KC_9, KC_ESC,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    LT(11,KC_ASTR), KC_DOLLAR, KC_EQL, KC_DOT, XXXXXXX, XXXXXXX,                        XXXXXXX, C(KC_G), KC_0, KC_4, KC_5, KC_6,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    XXXXXXX, KC_CIRC, XXXXXXX, KC_LALT, XXXXXXX, XXXXXXX,                        XXXXXXX, XXXXXXX, KC_1, KC_2, LT(9,KC_3), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                KC_ENT, XXXXXXX, XXXXXXX,   TO(0),  TG(5), TRIPLE_WHLD
                                 //`--------------------------'  `--------------------------'
),

//mouse ly 6 MS_ACL0_TOGGLE MS_ACL2 MS_WHLU MS_WHLD MS_WHLR MS_WHLL
    [6] = LAYOUT_split_3x6_3(
   //,-----------------------------------------------------.                    ,-----------------------------------------------------.
   XXXXXXX, C(A(KC_R)), C(KC_Z), XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, MS_WHLU, MS_UP, MS_WHLD, KC_ESC,
   //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
   MS_WHLL, MS_WHLR, S(KC_F7), KC_F7, XXXXXXX, C(KC_S),            SLEEP, XXXXXXX, MS_LEFT, MS_DOWN, MS_RGHT, XXXXXXX,
   //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     XXXXXXX, C(A(KC_R)), S(KC_F7), KC_F7, XXXXXXX , WIN_D,    HIBERNATE, XXXXXXX, MS_WHLL, XXXXXXX, MS_WHLR, XXXXXXX,
   //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         TG(6),  XXXXXXX,     XXXXXXX,     TO(0), XXXXXXX, XXXXXXX
                                       //`--------------------------'  `--------------------------'
  ), // mouse2 ly7 - single mouse hand left

    [7] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, MS_WHLU, XXXXXXX, MS_WHLD, MS_ACL2,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX , XXXXXXX,                   SLEEP, XXXXXXX , TD(TDQ_PASTE), TD(TDQ_COPY), TD(TDQ_CUT), MS_ACL0,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, RM_SATD, XXXXXXX, XXXXXXX, WIN_D,                      HIBERNATE, XXXXXXX,  MOUSE_PRESSED_CLICK, MS_WHLR, MS_WHLL, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_ENT, XXXXXXX,  _______,     TO(0),   KC_ESC, TG(7)
                                      //`--------------------------'  `--------------------------'
 ), // layer 8 super bookmark2

    [8] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        XXXXXXX, A(KC_G), A(KC_H), A(KC_I), A(KC_K), XXXXXXX,                    XXXXXXX, XXXXXXX, C(S(KC_7)), C(S(KC_8)), C(S(KC_9)), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, XXXXXXX, A(KC_D), A(KC_E), A(KC_F), XXXXXXX,                    XXXXXXX,XXXXXXX ,XXXXXXX , C(S(KC_4)), C(S(KC_5)), C(S(KC_6)),
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, A(KC_A), A(KC_B), A(KC_C), XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, C(S(KC_1)), C(S(KC_2)), C(S(KC_3)), XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            XXXXXXX, XXXXXXX,  _______,     TO(0),   XXXXXXX, XXXXXXX
                                        //`--------------------------'  `--------------------------'
   ), // super move ly

    [9] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
          XXXXXXX, XXXXXXX, VOICE_A, C(G(KC_S)), XXXXXXX, XXXXXXX,                  XXXXXXX, KC_F6, PAGE_PARAGRAPH_UP, A(KC_UP), PAGE_PARAGRAPH_DOWN, KC_F2,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          KC_LSFT, XXXXXXX, MO(10), VOICE, XXXXXXX, XXXXXXX,                    XXXXXXX, LCTL(LSFT(KC_M)), XXXXXXX, A(KC_DOWN), XXXXXXX, HOME_END,
      //|--------+--------+--- ----+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, PGUP_CTRLPG, C(KC_HOME) , PGDW_CTRLPG, C(KC_END),
      //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                              CHATGPT, XXXXXXX,  _______,     TO(0),   XXXXXXX, XXXXXXX
                                          //`--------------------------'  `--------------------------'
     ), //LY 10 super close window

    [10] = LAYOUT_split_3x6_3(
         //,-----------------------------------------------------.                    ,-----------------------------------------------------.
         XXXXXXX, XXXXXXX, XXXXXXX, A(KC_F4), XXXXXXX, XXXXXXX,                       XXXXXXX, KC_F5, C(KC_L), VOICE, C(KC_T), XXXXXXX,
        //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
         XXXXXXX, XXXXXXX, XXXXXXX, C(KC_F21), XXXXXXX, XXXXXXX,                       XXXXXXX, XXXXXXX, A(KC_LEFT), C(S(KC_TAB)), A(KC_RIGHT), C(KC_TAB),
        //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX , XXXXXXX, XXXXXXX,                       XXXXXXX, XXXXXXX, KC_F21, C(S(KC_T)),  A(KC_LEFT), A(KC_RIGHT),
        //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                 XXXXXXX, _______,  _______,     TO(0),   XXXXXXX, C(KC_F4)
                                            //`--------------------------'  `--------------------------'
        ), //LY 11 super DEL

    [11] = LAYOUT_split_3x6_3(
          //,-----------------------------------------------------.                    ,-----------------------------------------------------.
           XXXXXXX, DEL_LINE, DEL_WORD, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, A(S(KC_UP)), C(S(KC_UP)), XXXXXXX, XXXXXXX,
          //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
           XXXXXXX, DEL_LINE, KC_BSPC, KC_DEL, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, A(S(KC_DOWN)), C(S(KC_DOWN)), XXXXXXX,  XXXXXXX,
          //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
          //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                  LT(12,Z_UNDO), _______,  _______,     TO(0),   XXXXXXX, XXXXXXX
                                              //`--------------------------'  `--------------------------'
         ), // LY12 super move 3

//run debug
    [12] = LAYOUT_split_3x6_3(
           //,-----------------------------------------------------.                    ,-----------------------------------------------------.
            XXXXXXX, XXXXXXX, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,                 XXXXXXX, XXXXXXX, XXXXXXX, KC_UP, XXXXXXX, XXXXXXX,
           //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
            RM_TOGG, XXXXXXX, TO(0), RM_VALU, UG_TOGG, XXXXXXX,                       XXXXXXX, XXXXXXX, C(S(KC_F10)), S(KC_F10), A(S(KC_F10)), C(KC_F5),
           //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
            RM_NEXT, RM_HUED, RM_SATD, RM_VALD, XXXXXXX, XXXXXXX,                       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
           //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                   KC_SPC, _______,  _______,     TO(0),   XXXXXXX, C(KC_F2)
                                               //`--------------------------'  `--------------------------'
          )



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

        case TD_SINGLE_HOLD: //copy 1 line
                tap_code_delay(KC_HOME, 10);
                tap_code_delay(KC_HOME, 10);
                register_code(KC_LSFT);
                tap_code_delay(KC_END, 10);
                unregister_code(KC_LSFT);
                //wait_ms(50);
                tap_code16_delay(C(KC_C), 10);

         break;

        case TD_DOUBLE_TAP: //copy 1 word

                // Ctrl + Left
                register_code(KC_LCTL);
                tap_code(KC_LEFT);
                unregister_code(KC_LCTL);
                wait_ms(10);

               register_code(KC_LCTL);
               register_code(KC_LSFT);
               tap_code_delay(KC_RIGHT, 10);
               unregister_code(KC_LCTL);
               unregister_code(KC_LSFT);
               tap_code16_delay(C(KC_C), 10);

        break;

        case TD_DOUBLE_HOLD: //copy 1 paragraph

                tap_code(KC_HOME);
                register_code(KC_LSFT);
                tap_code16(LALT(KC_PGDN));
                unregister_code(KC_LSFT);
                tap_code16(LCTL(KC_C));

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

        case TD_SINGLE_HOLD:

//        DESACTIVAR CTRL Y SHIFT

                if (shift_active) {
                    unregister_code(KC_LSFT);
                    shift_active = false;
                }


        SEND_STRING(SS_LCTL(SS_LSFT("v"))); break; //portapapeles

        case TD_DOUBLE_TAP: //paste 1 word

                        // Ctrl + Left
                        register_code(KC_LCTL);
                        tap_code(KC_LEFT);
                        unregister_code(KC_LCTL);
                        wait_ms(10);

                        register_code(KC_LCTL);
                        register_code(KC_LSFT);
                        tap_code(KC_RIGHT); // Selecciona palabra
                        unregister_code(KC_LSFT); // Soltás shift antes de pegar
                        tap_code16_delay(C(KC_V), 10); // Pega normal
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
                     wait_ms(10);
                     tap_code(KC_HOME);
                     wait_ms(10);
                     // Ctrl + Shift + End para seleccionar
                     register_code(KC_LSFT);
                     tap_code(KC_END);
                     unregister_code(KC_LSFT);
                     wait_ms(10);

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
            if (shift_active) {
                unregister_code(KC_LSFT);
                shift_active = false;
            }


            defer_cut = true;
            defer_timer_cut = timer_read();
            break;

        case TD_SINGLE_HOLD: // Cut 1 line
            tap_code_delay(KC_HOME, 10);
            register_code(KC_LSFT);
            tap_code_delay(KC_END, 10);
            unregister_code(KC_LSFT);
            wait_ms(10);
            tap_code16(C(KC_X));
            break;

        case TD_DOUBLE_TAP: // Cut 1 word

            // Ctrl + Left
            register_code(KC_LCTL);
            tap_code(KC_LEFT);
            unregister_code(KC_LCTL);
            wait_ms(10);

            register_code(KC_LCTL);
            register_code(KC_LSFT);
            tap_code_delay(KC_RIGHT, 10);
            unregister_code(KC_LSFT);
            unregister_code(KC_LCTL);
            wait_ms(10);
            tap_code16(C(KC_X));
            break;

        case TD_DOUBLE_HOLD: //cut 1 paragraph

                 tap_code(KC_HOME);
                 register_code(KC_LSFT);
                 tap_code16(LALT(KC_PGDN));
                 unregister_code(KC_LSFT);
                 tap_code16(LCTL(KC_X));

         break;

        default: break;
    }
}

void tdq_bookmark_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: //show list markers floating window
                          register_code(KC_LSFT);   // Mantener Shift
                          tap_code(KC_F11);         // Presionar F11
                          unregister_code(KC_LSFT); // Soltar Shift
                          break;
        case TD_SINGLE_HOLD: SEND_STRING(SS_LALT("2")); break; //show list markers
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
        case TD_SINGLE_TAP:  tap_code16(C(KC_B)); break; //go to definition
        case TD_SINGLE_HOLD: tap_code16(C(A(KC_B))); break;//go to implemetation
        case TD_DOUBLE_TAP:  tap_code16(C(S(KC_B))); break;//Go to Type Declaration
        case TD_DOUBLE_HOLD: tap_code16(C(S(KC_T))); break;//go to test
        case TD_TRIPLE_HOLD: tap_code16(C(KC_U));  break;//Go to Super Method / Class
//        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE TAP x!!"); break;
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
     //if (keycode == TD(TDQ_SHIFT)) || keycode == TD(TDQ_MOVE_LY ))
//     {return 180;}

        if (QK_TAP_DANCE <= keycode && keycode <= QK_TAP_DANCE_MAX) {
        return 450;
    }

    if (keycode == LT(1, KC_RIGHT)) {
            return 200;  // prueba 110–130 ms
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
     debug_enable=true;
     debug_keyboard=true;
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
