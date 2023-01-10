/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2020 Ploopy Corporation
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

Source for tapdance hold with custom keycode:
https://github.com/bmijanovich/qmk_firmware/blob/ploopy_classic_mac/keyboards/ploopyco/trackball/keymaps/bmijanovich/keymap.c
https://www.reddit.com/r/ploopy/comments/oqwyzs/help_drag_scroll_as_held_key_in_tap_dance/

Best way to tap-hold:
https://getreuer.info/posts/keyboards/triggers/index.html#tap-vs.-long-press

the following must be added to /ploopyco/trackball/rules.mk
MOUSEKEY_ENABLE = yes       # Mouse keys
TAP_DANCE_ENABLE = yes     # tap dance key codes
 */
#include QMK_KEYBOARD_H
#include "quantum.h"
// safe range starts at `PLOOPY_SAFE_RANGE` instead.
#define TH_BTN3_BTN4 LT(0, KC_4)

// custom keycodes
enum custom_keycodes {
  TD_DRAG_SCROLL,
  RAPIDF = PLOOPY_SAFE_RANGE,
};

// rapidfire state
bool rapid_fire = false;

// tapdance states
typedef enum {
  TD_NONE,
  TD_UNKNOWN,
  TD_SINGLE_TAP,
  TD_SINGLE_HOLD,
  TD_DOUBLE_SINGLE_TAP
} td_state_t;

// create global instance of tapdance state type
static td_state_t td_state;

// declare tapdance functions:

// function to determine current tapdance state
td_state_t cur_dance(qk_tap_dance_state_t *state);

// record type to handle custom keycode in tapdance
// Dummy keyrecord_t for hooking process_record_kb() with custom keycodes
static keyrecord_t dummy_record = {
  .event = {
    .key = {
      .col = 0,
      .row = 0,
    },
    .pressed = false,
    .time = 0,
  },
  .tap = {0},
};

// setup dummy_record for process_record_kb()
void setup_dummy_record(uint8_t col, uint8_t row, bool pressed) {
  dummy_record.event.key.col = col;
  dummy_record.event.key.row = row;
  dummy_record.event.pressed = pressed;
  dummy_record.event.time = timer_read();
}

// register a custom keycode with process_record_kb()
void register_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
  setup_dummy_record(col, row, true);
  process_record_kb(keycode, &dummy_record);
}

// unregister a custom keycode with process_record_kb()
void unregister_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
  setup_dummy_record(col, row, false);
  process_record_kb(keycode, &dummy_record);
}

// tap a custom keycode with process_record_kb()
void tap_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
  register_custom_keycode(keycode, col, row);
  wait_ms(10);
  unregister_custom_keycode(keycode, col, row);
}

// declare functions for advanced tapdance
void td_drag_scroll_finished(qk_tap_dance_state_t *state, void *user_data);
void td_drag_scroll_reset(qk_tap_dance_state_t *state, void *user_data);

// combo - combo count must be updated in config.h too
const uint16_t PROGMEM layer2_combo[] = {KC_BTN1, TH_BTN3_BTN4, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
    COMBO(layer2_combo, TG(2)),
};

// custom key intercepts
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    // rapidfire leftmouse
    case RAPIDF:
      if (record->event.pressed) {
        rapid_fire = true;
      } else {
        rapid_fire = false;
      }
      break;
    // tap to send BTN4 (back), hold to press-and-hold middle mouse
    case TH_BTN3_BTN4:
      if (record->tap.count > 0) {
        // key is being tapped    
        if (record->event.pressed) {
          // handle tap press event
          register_code16(KC_BTN4);
        } else {
          // handle tap release event
          unregister_code16(KC_BTN4);
        }
      } else {                        
        // key is being held
        if (record->event.pressed) {
          // handle hold press event
          register_code16(KC_BTN3);
        } else {
          // handle hold release event
          unregister_code16(KC_BTN3);
        }
      }
      return false;  // skip default handling
  }
  return true; // let other functions handle the event
}

void matrix_scan_user(void) {
  if (rapid_fire) {
    tap_code(KC_MS_BTN1);
  } else {
  }
}

// keymap:
// button order: left, scroll, right, back, forward
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( /* Base */
        KC_BTN1, KC_BTN3, TH_BTN3_BTN4, /*tap back, hold middle mouse*/
          TD(TD_DRAG_SCROLL), LT(1, KC_BTN5)
    ),
    [1] = LAYOUT(
        LALT(KC_BTN1), QK_BOOT, RAPIDF,
          DPI_CONFIG, _______
    ),
    [2] = LAYOUT(
      KC_BTN1, KC_BTN3, DRAG_SCROLL,
        KC_BTN2, LT(3, KC_BTN5)
    ),
    [3] = LAYOUT(
      LCTL(KC_BTN1), QK_BOOT, RAPIDF,
        DPI_CONFIG, _______
    ),
};

// determine the tapdance state to return
td_state_t cur_dance(qk_tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
    else return TD_SINGLE_HOLD;
  }

  if (state->count == 2) return TD_DOUBLE_SINGLE_TAP;
  else return TD_UNKNOWN; //any number higher than the maximum state value you return above
}

// Handle the possible states for each tapdance keycode you define:
void td_drag_scroll_finished(qk_tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  switch (td_state) {
    case TD_SINGLE_TAP:
      register_code16(KC_BTN2);
      break;
    case TD_SINGLE_HOLD:
      register_custom_keycode(DRAG_SCROLL, 3, 0);
      break;
    case TD_NONE:
    case TD_UNKNOWN:
    case TD_DOUBLE_SINGLE_TAP:
      // Nothing to do
      break;
  }
}

void td_drag_scroll_reset(qk_tap_dance_state_t *state, void *user_data) {
  switch (td_state) {
    case TD_SINGLE_TAP:
      unregister_code16(KC_BTN2);
      break;
    case TD_SINGLE_HOLD:
      unregister_custom_keycode(DRAG_SCROLL, 3, 0);
      break;
    case TD_NONE:
    case TD_UNKNOWN:
    case TD_DOUBLE_SINGLE_TAP:
      // Nothing to do
      break;
  }
}

// Define `ACTION_TAP_DANCE_FN_ADVANCED()` for each tapdance keycode, passing in `finished` and `reset` functions
qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_DRAG_SCROLL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_drag_scroll_finished, td_drag_scroll_reset),
};