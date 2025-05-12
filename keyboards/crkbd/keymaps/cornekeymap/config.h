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


//for reduce size firmware
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION
#define NO_ACTION_ONESHOT //desactiva funcion one shot

//Tapping term for tap dance
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



//RGB LIGHT
//leds total 54. 27 leds one half
#define SPLIT_LAYER_STATE_ENABLE //para RGB LIGHT y RGB MATRIX
#define RGBLIGHT_LAYERS //for RGBLIGHT
#define RGBLIGHT_DISABLE_EFFECTS //for RGB LIGHT
#define RGBLIGHT_SLEEP //si el pc sleep, los leds tambien.
//#define DISABLE_RGB_MATRIX_EFFECTS //para RGB MATRIX
//#define RGB_MATRIX_DISABLE_SHARED_KEYCODES //for RGB MATRIX desactiva botones de RGB MATRIX, como RM_TOGG, RM_SATU, ETC.


#ifdef RGBLIGHT_ENABLE
    #undef RGBLIGHT_LIMIT_VAL
    #define RGBLIGHT_LIMIT_VAL 80 //ok 255 default
#endif
