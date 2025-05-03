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

#define MK_KINETIC_SPEED 50  // Aumenta la velocidad máxima
#define MK_KINETIC_ACCEL 15  // Aceleración más progresiva
#define MK_KINETIC_DELAY 15  // Inicia la aceleración más rápido
#define MK_KINETIC_INTERVAL 16  // Intervalo entre actualizaciones del cursor (baja el lag)



#define RGB_MATRIX_DISABLE_SHARED_KEYCODES
#define RGBLIGHT_LAYERS
//#define RGBLIGHT_SPLIT
//#define RGBLED_NUM 42


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

//#ifdef RGBLIGHT_ENABLE
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
//    #define RGBLIGHT_LIMIT_VAL 120
//    #define RGBLIGHT_HUE_STEP 10
//    #define RGBLIGHT_SAT_STEP 17
//    #define RGBLIGHT_VAL_STEP 17
//#endif
