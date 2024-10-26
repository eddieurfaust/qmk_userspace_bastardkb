/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// ********************************************************************
// https://jayliu50.github.io/qmk-cheatsheet/
// ********************************************************************

#include QMK_KEYBOARD_H

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_POINTER,
    LAYER_SYMBOL,
    LAYER_NAVIGATION,
    LAYER_NUMBERS
};

/** \brief Automatically enable sniping-mode on the pointer layer. */
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#define SYMBOL MO(LAYER_SYMBOL)
#define NAVIGATION MO(LAYER_NAVIGATION)
#define NUMBERS MO(LAYER_NUMBERS)
#define PT_X LT(LAYER_POINTER, KC_X)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

// Macros
#define C_S(a) (LCTL(LSFT(a)))  // Control + Shift
#define LCG(a) (LCTL(LGUI(a)))  // Control + Gui

// ********************************************************************
// * TAP DANCE
// ********************************************************************
// taken from example 3: https://docs.qmk.fm/features/tap_dance#examples
#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

enum {
    CT_CLN,
};
typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

tap_dance_action_t tap_dance_actions[] = {
    [CT_CLN] = ACTION_TAP_DANCE_TAP_HOLD(KC_COLN, KC_SCLN),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;

    switch (keycode) {
        case TD(CT_CLN):  // list all tap dance keycodes with tap-hold configurations
            action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
    }
    return true;
}

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX,    KC_W,    KC_F,    KC_M,    KC_P,    KC_V,    LSFT(KC_7), KC_DOT, LSFT(KC_SLASH), LSFT(KC_8), LSFT(KC_2), XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX,    KC_R,    KC_S,    KC_N,    KC_T,    KC_B,    KC_COMMA,   KC_A,    KC_E,    KC_I,    KC_H, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX,    PT_X,    KC_C,    KC_L,    KC_D,    KC_G,    TD(CT_CLN),    KC_U,    KC_O,    KC_Z,    KC_K, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                               NAVIGATION, KC_LSFT, XXXXXXX,  KC_SPC, SYMBOL,
                                           XXXXXXX, XXXXXXX,     NUMBERS
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_SYMBOL] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_ESCAPE, RALT(KC_7), RALT(KC_8), LSFT(KC_8), RALT(KC_RBRC),  /**/  KC_GRAVE,   LSFT(KC_9),   RALT(KC_9),   RALT(KC_0), LSFT(KC_EQUAL), XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_SLASH, LSFT(KC_RBRC), LSFT(KC_0), LSFT(KC_SLASH), LSFT(KC_4),  /**/  KC_NUHS,   OSM(MOD_LCTL),   OSM(MOD_LSFT), OSM(MOD_LALT), OSM(MOD_LGUI), XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_RBRC, RALT(KC_NUBS), RALT(KC_Q), LSFT(KC_7), LSFT(KC_5),  /**/  XXXXXXX,   RALT(KC_MINUS),   LSFT(KC_6),   LSFT(KC_MINUS), LSFT(KC_1), XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX,
                                           XXXXXXX, XXXXXXX,    XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_NAVIGATION] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_TAB, LALT(KC_TAB), C_S(KC_TAB), LCTL(KC_TAB), XXXXXXX,  /**/   XXXXXXX, KC_HOME, KC_UP, KC_END, KC_DELETE, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, OSM(MOD_LGUI), OSM(MOD_LALT), OSM(MOD_LSFT), OSM(MOD_LCTL), XXXXXXX,  /**/   XXXXXXX, KC_LEFT, KC_DOWN, KC_RIGHT, KC_BSPC, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, LCG(KC_LEFT), LCG(KC_RIGHT), XXXXXXX,  LCTL(KC_F), XXXXXXX,  /**/  XXXXXXX, KC_PGDN, KC_PGUP, XXXXXXX, KC_ENTER, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  XXXXXXX, XXXXXXX, XXXXXXX,    _______, XXXXXXX,
                                           XXXXXXX, XXXXXXX,    XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_POINTER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, DPI_MOD, S_D_MOD,    S_D_MOD, DPI_MOD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX,    XXXXXXX, KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, _______, DRGSCRL, SNIPING, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, SNIPING, DRGSCRL, _______, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_BTN2, KC_BTN1, KC_BTN3,    KC_BTN3, KC_BTN1,
                                           XXXXXXX, KC_BTN2,    KC_BTN2
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_NUMBERS] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX,    KC_7,    KC_5,    KC_3,    KC_1,    KC_9,    KC_8, KC_0, KC_2,    KC_4,   KC_6, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, OSM(MOD_LGUI), OSM(MOD_LALT), OSM(MOD_LSFT), OSM(MOD_LCTL), KC_F11,  /**/  KC_F10,   OSM(MOD_RGUI), OSM(MOD_RALT), OSM(MOD_RSFT), OSM(MOD_RCTL), XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX,    KC_F7,    KC_F5,    KC_F3,    KC_F1,    KC_F9,    KC_F8, KC_F12, KC_F2,    KC_F4,   KC_F6, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                   SYMBOL, KC_LSFT,   XXXXXXX,      KC_SPC,  NAVIGATION,
                                           XXXXXXX, XXXXXXX,     XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),
};

// ********************************************************************
// COMBOS
// ********************************************************************

const uint16_t PROGMEM overview[] = {KC_F, KC_M, COMBO_END};
const uint16_t PROGMEM cut[] = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM copy[] = {KC_C, KC_L, COMBO_END};
const uint16_t PROGMEM paste[] = {KC_L, KC_D, COMBO_END};
const uint16_t PROGMEM letter_q[] = {KC_U, KC_Z, COMBO_END};
const uint16_t PROGMEM letter_z[] = {KC_W, KC_F, COMBO_END};
const uint16_t PROGMEM letter_j[] = {KC_Z, KC_I, COMBO_END};
const uint16_t PROGMEM letter_ae[] = {KC_A, KC_O, COMBO_END};
const uint16_t PROGMEM letter_oe[] = {KC_O, KC_E, COMBO_END};
const uint16_t PROGMEM letter_ue[] = {KC_U, KC_E, COMBO_END};
const uint16_t PROGMEM letter_ss[] = {KC_R, KC_S, COMBO_END};
const uint16_t PROGMEM left_angle_bracket[] = {RALT(KC_8), LSFT(KC_0), COMBO_END};
const uint16_t PROGMEM right_angle_bracket[] = {RALT(KC_9), OSM(MOD_LSFT), COMBO_END};

const uint16_t PROGMEM num_layer[] = {NAVIGATION, SYMBOL, COMBO_END};

combo_t key_combos[] = {
    COMBO(overview, LGUI(KC_TAB)),
    COMBO(cut, LCTL(KC_X)),
    COMBO(copy, LCTL(KC_C)),
    COMBO(paste, LCTL(KC_V)),
    COMBO(letter_q, KC_Q),
    COMBO(letter_z, KC_Y),
    COMBO(letter_j, KC_J),
    COMBO(letter_ae, KC_QUOTE),
    COMBO(letter_oe, KC_SEMICOLON),
    COMBO(letter_ue, KC_LEFT_BRACKET),
    COMBO(letter_ss, KC_MINUS),
    COMBO(left_angle_bracket, KC_NONUS_BACKSLASH),
    COMBO(right_angle_bracket, LSFT(KC_NONUS_BACKSLASH)),
    COMBO(num_layer, NUMBERS)
};

// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#        endif // RGB_MATRIX_ENABLE
    }
}
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif
