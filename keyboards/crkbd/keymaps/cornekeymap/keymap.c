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

//macro enum
enum custom_keycodes {
    M_SEL_COPY = SAFE_RANGE,
    M_ALT_TAB,
    M_CTRL_TAB,
    M_ENIE,
    DBL_CLICK,
    ALL_COPY,
    CLEAR_WIN,
    WIN_D,
    CTRL_SHIFT_ENTER,
    LLAMBDA,
    RLAMBDA,
    DOUBLE_COLON,
    ENV_VAR,
    LBRC2

};

//combo enum
enum combos{
  CB_CTRL_Z,
  CB_TAB,
  CB_LAYER,
  CB_ALL_COPY,
  CB_CLEAR_WIN
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
    TDQ_MOUSE_LY
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

//#### prototypes tap dance
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

void tdq_mouse_ly_finished(tap_dance_state_t *state, void *user_data);
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
const uint16_t PROGMEM cb_ctrl_z[] = {LT(4, KC_J), LT(2,  KC_K), COMBO_END};
const uint16_t PROGMEM cb_tab[] = {KC_F, LT(4, KC_J), COMBO_END};

const uint16_t PROGMEM cb_layer[] = {LT(1, KC_S), LT(1, KC_L), COMBO_END}; //anulares
const uint16_t PROGMEM cb_all_copy[] = {KC_E, KC_I, COMBO_END}; //medios up
const uint16_t PROGMEM cb_clear_win[] = {LT(2, KC_D), LT(2, KC_K), COMBO_END}; //medios

//const uint16_t PROGMEM test_combo12[] = {TD(TDQ_SHIFT), KC_O, COMBO_END};
combo_t key_combos[] = {
   [CB_CTRL_Z]   = COMBO(cb_ctrl_z, LCTL(KC_Z)),
   [CB_TAB]      = COMBO(cb_tab, KC_TAB),
   [CB_LAYER]    = COMBO(cb_layer, TG(6)),
   [CB_ALL_COPY] = COMBO(cb_all_copy, ALL_COPY),
   [CB_CLEAR_WIN] = COMBO(cb_clear_win, CLEAR_WIN),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

        switch (keycode) {
            case M_SEL_COPY: SEND_STRING(SS_LCTL("ac")); break;
            case M_CTRL_TAB: if (record->event.pressed) { SEND_STRING(SS_LCTL(SS_TAP(X_TAB))); } break;
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

          case CTRL_SHIFT_ENTER :
               if (record->event.pressed) {
                   SEND_STRING(SS_LCTL(SS_LSFT(SS_TAP(X_ENTER))));
                   }
                   break;

          case LT(0,KC_DOWN):
              if (!record->tap.count && record->event.pressed) {
                  tap_code16(KC_RIGHT); // Intercept hold function to send Ctrl-X
                  return false;
              }
              return true;

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

          case  LT(1,KC_Z):
                if (!record->tap.count && record->event.pressed) {
                  tap_code16(KC_LWIN);
                   return false;
                }
                return true;

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
                if (!record->tap.count && record->event.pressed) {
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
                if (!record->tap.count && record->event.pressed) {
                  SEND_STRING("]");
                   return false;
                }else {
                   SEND_STRING("[");
                   return false;
                     }
                 }
                break;
        }





    return true;
};

//timer for alt_tab macro
void matrix_scan_user(void) {
  if (is_alt_tab_active) {
    if (timer_elapsed(alt_tab_timer) > 1000) {
      unregister_code(KC_LALT);
      is_alt_tab_active = false;
    }
  }
  }

// Tap Dance actions
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
    [TDQ_MOUSE_LY] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tdq_mouse_ly_finished, x_reset),

};


 //KEY MAP
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
   KC_Q, KC_W, KC_E,  KC_R, KC_T, XXXXXXX,                                     XXXXXXX, KC_Y, KC_U,   KC_I,  KC_O,  TD(TD_ESC_CAPS),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
  LSFT_T(KC_A), LT(1, KC_S), LT(2, KC_D), KC_F, KC_G, KC_CAPS,                 XXXXXXX, KC_H, LT(4, KC_J), LT(2, KC_K), LT(1, KC_L), RSFT_T(KC_P),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
  LT(1,KC_Z), KC_X, KC_C, KC_V,  KC_B, WIN_D,                                     XXXXXXX, KC_N,  KC_M, LT(0,KC_DOWN), TD(TD_BSPC), M_ENIE,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                      TD(TDQ_MOVE_LY), TG(5),  KC_ENT,     XXXXXXX,   TO(0), KC_ENT
                                      //`--------------------------'  `--------------------------'

  ), //symbols_1 layer

    [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                           ,-----------------------------------------------------.
  KC_ASTR,  LT(1,KC_PERC), LT(1,KC_MINS),  LT(1,KC_SLSH), LT(1,ENV_VAR), XXXXXXX,     XXXXXXX, XXXXXXX, LT(1,KC_EQL), LT(1,KC_AT),  LT(1,KC_DQT), LT(1,KC_TILD),
  //|--------+--------+--------+--------+--------+--------|                           |--------+--------+--------+--------+--------+--------|
  LT(1, KC_AMPR), DOUBLE_COLON, LT(1,KC_LABK), LT(1,KC_RABK), KC_PIPE, XXXXXXX,       XXXXXXX, LT(1,KC_EXLM) , KC_DOT, CTRL_SHIFT_ENTER,  LT(1,KC_LPRN), LT(1,KC_LCBR),
  //|--------+--------+--------+--------+--------+--------|                           |--------+--------+--------+--------+--------+--------|
  XXXXXXX, KC_CIRC, LLAMBDA, RLAMBDA, XXXXXXX, XXXXXXX,                               XXXXXXX, XXXXXXX, KC_COMM, LT(1,KC_SCLN),  LT(1,LBRC2),  XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_SPC, _______,  XXXXXXX,     XXXXXXX,   KC_TRNS, KC_ENT
                                      //`--------------------------'  `--------------------------'
  ), //symbols_2 layer

    [2] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
          XXXXXXX, KC_BSLS, KC_PSLS, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_LCBR, KC_RCBR, KC_QUOT, XXXXXXX,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          XXXXXXX, KC_LSFT, XXXXXXX, KC_PIPE, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, QK_BOOT,                      QK_BOOT, XXXXXXX, TD(TD_LABK_EQ), TD(TD_RABK_EQ), TD(TD_LAMBDA), KC_GRAVE,
      //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                                 XXXXXXX, _______,  XXXXXXX,     XXXXXXX,   KC_TRNS, XXXXXXX
                                          //`--------------------------'  `--------------------------'
  ), //move layer

    [3] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     XXXXXXX, M_SEL_COPY, LCTL(KC_V), LCTL(KC_X), XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, M_CTRL_TAB,  KC_UP, M_ALT_TAB, KC_ESC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     TG(3), KC_LSFT, KC_LCTL, LCTL(KC_Z), XXXXXXX, XXXXXXX,                    XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_RIGHT, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     XXXXXXX,  XXXXXXX, KC_LALT, LCTL(KC_Y), XXXXXXX, XXXXXXX,                 XXXXXXX, XXXXXXX, XXXXXXX
     , KC_TAB, XXXXXXX, XXXXXXX,
  //| ------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_SPC, _______,  TO(0),     XXXXXXX, KC_TRNS, KC_ENT
                                      //`--------------------------'  `--------------------------'
  ), //numbers layer

       [4] = LAYOUT_split_3x6_3(
     //,-----------------------------------------------------.                    ,-----------------------------------------------------.
         XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
     //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
         TG(4), KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
     //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
         KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
     //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                             KC_SPC, _______,  _______,     _______,   KC_TRNS, KC_ENT
                                         //`--------------------------'  `--------------------------'
     ), //mouse layer

     [5] = LAYOUT_split_3x6_3(
   //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       XXXXXXX, KC_LCTL, MS_WHLD, MS_WHLU, XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, MS_WHLU, MS_UP, MS_WHLD, KC_ESC,
   //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       TG(5), DBL_CLICK, MS_BTN1, MS_BTN2, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, MS_LEFT, MS_DOWN, MS_RGHT, XXXXXXX,
   //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       XXXXXXX, XXXXXXX, KC_LSFT, MS_BTN3, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, MS_WHLL, XXXXXXX, MS_WHLR, XXXXXXX,
   //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC, TG(5),  _______,     _______,   XXXXXXX, KC_ENT
                                       //`--------------------------'  `--------------------------'
  ), //  layer

    [6] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_SPC, _______,  _______,     _______,   XXXXXXX, KC_ENT
                                      //`--------------------------'  `--------------------------'
 ), //  layer

      [7] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            KC_SPC, _______,  _______,     _______,   XXXXXXX, KC_ENT
                                        //`--------------------------'  `--------------------------'
   ), //  layer

        [8] = LAYOUT_split_3x6_3(
      //,-----------------------------------------------------.                    ,-----------------------------------------------------.
          XXXXXXX, KC_COMMA, KC_DOT, LSFT(KC_MINS), XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, KC_7, KC_8, KC_9, KC_BSPC,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          KC_KP_SLASH, KC_KP_ASTERISK, KC_PLUS, KC_KP_MINUS, XXXXXXX, XXXXXXX,       XXXXXXX, XXXXXXX, KC_0, KC_4, KC_5, KC_6,
      //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
          KC_CIRC, KC_DOLLAR, KC_EQL, KC_PERCENT, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, KC_1, KC_2, KC_3, XXXXXXX,
      //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                              KC_SPC, _______,  _______,     _______,   XXXXXXX, KC_ENT
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
                                                KC_SPC, _______,  _______,     _______,   XXXXXXX, KC_ENT
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
                                                  KC_SPC, _______,  _______,     _______,   XXXXXXX, KC_ENT
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
                                                   KC_SPC, _______,  _______,     _______,   XXXXXXX, KC_ENT
                                               //`--------------------------'  `--------------------------'
          )



};



//###tap dance implementation

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
      case 3: SEND_STRING(SS_LSFT(SS_TAP(X_DEL))); break;
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

//###Quad implementation
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
        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE TAP!!"); break;
        case TD_TRIPLE_HOLD: SEND_STRING("TRIPLE TAP HOLD!!"); break;
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_V); register_code(KC_V); break;
        default: break;
    }
}

void tdq_shift_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: SEND_STRING(SS_TAP(X_W)); break;
        case TD_SINGLE_HOLD: register_code(KC_LSFT); break;
        case TD_DOUBLE_TAP:  tap_code16_delay(KC_LPRN, 30);  // Escribe '('
                             tap_code16_delay(KC_RPRN, 30);  // Escribe ')'
                             tap_code16_delay(KC_SCLN, 30);
                             tap_code_delay(KC_LEFT, 30);  // Mueve el cursor dentro de los paréntesis break;
                             tap_code_delay(KC_LEFT, 30);  // Mueve el cursor dentro de los paréntesis
                             break;

        case TD_DOUBLE_HOLD: SEND_STRING("{}" SS_TAP(X_LEFT)); break;
        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE TAP!!"); break;
        case TD_TRIPLE_HOLD: SEND_STRING("TRIPLE TAP HOLD!!"); break;
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
        case TD_SINGLE_HOLD: layer_on(3); break;
        case TD_DOUBLE_TAP:SEND_STRING("Double hold!!"); break;

//        case TD_DOUBLE_HOLD: layer_on(4); break;
        case TD_DOUBLE_HOLD:
          // Check to see if the layer is already set
                       if (layer_state_is(3)) {
                           // If already set, then switch it off
                           layer_off(3);
                       } else {
                           // If not already set, then switch the layer on
                           layer_on(3);
                       } break;


        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE TAP!!"); break;
        case TD_TRIPLE_HOLD: SEND_STRING("TRIPLE TAP HOLD!!"); break;
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

void tdq_mouse_ly_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: tap_code(KC_F); break;
        case TD_SINGLE_HOLD: layer_on(5); break;
        case TD_DOUBLE_TAP:SEND_STRING("Double hold!!"); break;

//        case TD_DOUBLE_HOLD: layer_on(4); break;
        case TD_DOUBLE_HOLD:
          // Check to see if the layer is already set
                       if (layer_state_is(4)) {
                           // If already set, then switch it off
                           layer_off(4);
                       } else {
                           // If not already set, then switch the layer on
                           layer_on(4);
                       } break;


        case TD_TRIPLE_TAP: SEND_STRING("TRIPLE TAP!!"); break;
        case TD_TRIPLE_HOLD: SEND_STRING("TRIPLE TAP HOLD!!"); break;
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_W); register_code(KC_W); break;
        default: break;
    }
}

void x_reset(tap_dance_state_t *state, void *user_data) {
//  if (xtap_state.state == TD_DOUBLE_SINGLE_TAP) {
             clear_keyboard();
  //  }
    xtap_state.state = TD_NONE;
}




//tapping term
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {

//#cuando se quiere poner exception a un Tap dance
     //if (keycode == TD(TDQ_SHIFT)) || keycode == TD(TDQ_MOVE_LY )   {
     if (keycode == TD(TDQ_MOVE_LY))   {
        return TAPPING_TERM;
    }

    if (QK_TAP_DANCE <= keycode && keycode <= QK_TAP_DANCE_MAX) {
        return 450;
    }

     return TAPPING_TERM;


  }



//layer_state_t layer_state_set_user(layer_state_t state) {
//    switch (get_highest_layer(state)) {
//    case 3:
//        rgblight_setrgb (0xFF,  0x00, 0x00);
//        break;
//
//    }
//  return state;
//}
//###end quad implementation

























