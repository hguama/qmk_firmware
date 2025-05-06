/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

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
*/

#pragma once

//#define USE_MATRIX_I2C
//for reduce size firmware
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION
#define NO_ACTION_ONESHOT //desactiva funcion one shot


#define TAPPING_TERM 175
#define TAPPING_TERM_PER_KEY

//MOUSE MOVE
#define MK_COMBINED
#define MOUSEKEY_BASE_SPEED 150 //150 300 500 ok
#define MOUSEKEY_MAX_SPEED 5 //5 0k bajar //max speed en la que se detiene la aceleracion
#define MOUSEKEY_TIME_TO_MAX 150 //150 100 0k
#define MOUSEKEY_MOVE_DELTA 5 //5 2 0k mas lento
#define MOUSEKEY_INTERVAL 5 //5 ok
#define MOUSEKEY_WHEEL_DELTA 2 //Wheel movement step size
#define MOUSEKEY_WHEEL_MAX_SPEED 10 //Maximum number of scroll steps per scroll action


//#define RGB_MATRIX_DISABLE_SHARED_KEYCODES
//#define RGBLIGHT_LAYERS

//nuevos test
//#undef RGBLED_NUM
//#define RGBLED_NUM 12
//#undef RGBLED_SPLIT
//#undef RGBLIGHT_LED_COUNT
//#define RGBLIGHT_ANIMATIONS
//#define RGBLED_SPLIT {6, 6}
//#undef RGBLIGHT_LED_COUNT
//#define RGBLIGHT_LED_COUNT 12
#define RGBLIGHT_SPLIT
#define SPLIT_LED_STATE_ENABLE
#define SPLIT_LAYER_STATE_ENABLE

#undef RGBLED_SPLIT
#define RGBLED_SPLIT {27,27}



//#define RGBLIGHT_SPLIT
//#define RGBLED_NUM 42


#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#undef RGBLIGHT_EFFECT_STATIC_GRADIENT
#undef RGBLIGHT_EFFECT_RAINBOW_SWIRL
#undef RGBLIGHT_ANIMATIONS
#undef RGBLIGHT_EFFECT_BREATHING
#undef RGBLIGHT_EFFECT_RAINBOW_MOOD
#undef RGBLIGHT_EFFECT_RAINBOW_SWIRL
#undef RGBLIGHT_EFFECT_SNAKE
#undef RGBLIGHT_EFFECT_KNIGHT
#undef RGBLIGHT_EFFECT_CHRISTMAS
#undef RGBLIGHT_EFFECT_STATIC_GRADIENT
#undef RGBLIGHT_EFFECT_RGB_TEST
#undef RGBLIGHT_EFFECT_ALTERNATING
#undef RGBLIGHT_EFFECT_TWINKLE

//#define RGBLIGHT_ENABLE
//#define RGBLIGHT_LAYERS

// Opcional, pero muy recomendado:
//#define RGBLIGHT_ANIMATIONS        // (si quieres efectos bonitos, opcional)
//#define RGBLIGHT_LIMIT_VAL 120     // brillo máximo (evita que queme LEDs o consuma mucho)
//#define RGBLIGHT_SLEEP

#ifdef RGBLIGHT_ENABLE
//    #define RGBLIGHT_EFFECT_BREATHING
//    #define RGBLIGHT_EFFECT_RAINBOW_MOOD
//    #define RGBLIGHT_EFFECT_RAINBOW_SWIRL
//    #define RGBLIGHT_EFFECT_SNAKE
//    #define RGBLIGHT_EFFECT_KNIGHT
//    #define RGBLIGHT_EFFECT_CHRISTMAS
//    #define RGBLIGHT_EFFECT_STATIC_GRADIENT
//    #define RGBLIGHT_EFFECT_RGB_TEST
//    #define RGBLIGHT_EFFECT_ALTERNATING
//    #define RGBLIGHT_EFFECT_TWINKLE
    #define RGBLIGHT_LIMIT_VAL 120
    #define RGBLIGHT_HUE_STEP 10
    #define RGBLIGHT_SAT_STEP 17
    #define RGBLIGHT_VAL_STEP 17
#endif
