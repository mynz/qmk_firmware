/*
 * tap dance ref: keyboards/planck/keymaps/altgr/common/keycode_functions.h
 * tap dance ref: keyboards/bpiphany/frosty_flake/keymaps/nikchi/keymap.c
 *
 * TL() で PERMISSIVE_HOLD が効かない件
 * http://okapies.hateblo.jp/entry/2019/02/02/133953
 * https://gist.github.com/okapies/5d13a174cbb13ce34dbd9faede9d0b71#file-keymap-c-L99-L164
 *
 */

#include QMK_KEYBOARD_H
#include "bootloader.h"
#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

extern keymap_config_t keymap_config;

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _QWERTY 0
#define _LOWER 3
#define _RAISE 4
#define _ADJUST 16

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  ADJUST,
  BACKLIT,
  RGBRST,
  MTYUN, // my macro
};

enum macro_keycodes {
  KC_SAMPLEMACRO,
};

enum TapDance {
	TD_EQL,  // =
	TD_LCBR, // {
	TD_SCLN, // ;
};

// helper
static void tap_key(uint16_t keycode, bool shift) {
	if (shift) {
		register_code(KC_LSHIFT);
		register_code(keycode);
		unregister_code(keycode);
		unregister_code(KC_LSHIFT);
	} else {
		register_code(keycode);
		unregister_code(keycode);
	}
}

// helper
static void tap_key_double_max(uint16_t keycode, bool shift, uint8_t count) {
	tap_key(keycode, shift);
	if (count > 1) {
		tap_key(keycode, shift);
	}
}

// helper
void tap_key_ntimes(uint16_t keycode, bool shift, uint8_t count) {
    if (count == 0) {
        return;
    }

    if (shift) {
        register_code(KC_LSFT);
    }

	while (count-- > 0) {
		register_code(keycode);
		unregister_code(keycode);
	}

    if (shift) {
        unregister_code(KC_LSFT);
    }
}

void td_brace_fn(qk_tap_dance_state_t *state, void *user_data) {
	if (state->count >= 3 ) {
		tap_key(KC_LBRC, true);
		tap_key(KC_ENT, false);
		tap_key(KC_ENT, false);
		tap_key(KC_RBRC, true);
		tap_key(KC_UP, false);
	} else {
		tap_key_double_max(KC_LBRC, true, state->count);
	}
	reset_tap_dance(state);
}

void td_eql_fn(qk_tap_dance_state_t *state, void *user_data) {
	if (state->count >= 3 ) {
		tap_key(KC_SPC, false);
		tap_key(KC_EQL, false);
		tap_key(KC_SPC, false);
	} else {
		tap_key_double_max(KC_EQL, false, state->count);
	}
	reset_tap_dance(state);
}

void td_scln_fn(qk_tap_dance_state_t *state, void *user_data) {
    uint8_t shift = (get_mods() & MOD_MASK_SHIFT);
    if (shift) {
        // type `:`
        // シフトキーが押されているときは擬似的にタップダンスを無効化する。
        // この中で unregister_code(KC_LSFT) を呼び出すようなコードがあると物理的にシフトキーを押しても離した状態に遷移してしまう。
        // その場合、再度物理的にシフトキーを押し直さないと有効にならないので（できる限り）そのような実装を含まないように気をつける。
        // この箇所に関しては、シフトキーが押されていることが前提になっているので、 `KC_SCLN` を発行すれば `:` が出力される。
        tap_key_ntimes(KC_SCLN, false, state->count);
    } else {
        if (state->count >= 2 ) {
            // type `:`
            tap_key_ntimes(KC_SCLN, true, state->count-1);
        } else {
            // type `;` as like a regular key.
            tap_key(KC_SCLN, false);
        }
    }
    reset_tap_dance(state);
}

qk_tap_dance_action_t tap_dance_actions[] = {
	[TD_EQL] = ACTION_TAP_DANCE_FN(td_eql_fn),
	[TD_LCBR] = ACTION_TAP_DANCE_FN(td_brace_fn),
	[TD_SCLN] = ACTION_TAP_DANCE_FN(td_scln_fn),
};

#define KC______ KC_TRNS
#define KC_XXXXX KC_NO
#define KC_LOWER LOWER
#define KC_RAISE RAISE
#define KC_RST   RESET
// #define KC_LRST  RGBRST
// #define KC_LTOG  RGB_TOG
// #define KC_LHUI  RGB_HUI
// #define KC_LHUD  RGB_HUD
// #define KC_LSAI  RGB_SAI
// #define KC_LSAD  RGB_SAD
// #define KC_LVAI  RGB_VAI
// #define KC_LVAD  RGB_VAD
// #define KC_LMOD  RGB_MOD

// マクロ: ユーザーネームを印字する
#define KC_MTYUN  MTYUN

// tap dance: タップダンス
#define KC_TEQL   TD(TD_EQL)   // '='
#define KC_TLCBR  TD(TD_LCBR)  // '{'
#define KC_TSCLN  TD(TD_SCLN)  // ':'

// #define KC_GUIEI GUI_T(KC_LANG2)
#define KC_ALTKN ALT_T(KC_LANG1)

#define KC_CTLTB CTL_T(KC_TAB)
#define KC_CTLES CTL_T(KC_ESC)

#if defined(FOR_MAC)
// for Mac
#define KC_CMD KC_LGUI
#define KC_OPTTB ALT_T(KC_TAB)
/* #define KC_LOWEI LT(_LOWER, KC_LANG2) */
/* #define KC_RAIKN LT(_RAISE, KC_LANG1) */
#else
// for Windows
#define KC_CMD KC_LALT
#define KC_OPTTB CTL_T(KC_TAB)
/* #define KC_LOWEI LT(_LOWER, KC_MHEN) */
/* #define KC_RAIKN LT(_RAISE, KC_HENK) */
#endif

#define KC_LOWEI KC_LOWER
#define KC_RAIKN KC_RAISE

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
        TAB,     Q,     W,     E,     R,     T,                      Y,     U,     I,     O,     P,  LEAD,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      CTLES,     A,     S,     D,     F,     G,                      H,     J,     K,     L, TSCLN,  QUOT,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       LSFT,     Z,     X,     C,     V,     B,                      N,     M,  COMM,   DOT,  SLSH,  RSFT,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                    CMD, LOWEI,   SPC,      ENT, RAIKN, OPTTB\
                              //`--------------------'  `--------------------'
  ),

  [_LOWER] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
      XXXXX,     1,    AT,  HASH,   DLR,  PERC,                   EXLM,  AMPR,  ASTR,  CIRC,     0,   DEL,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      XXXXX,    UP, XXXXX, TLCBR,  RCBR,   GRV,                   TEQL,  MINS,  LPRN,  RPRN,  PIPE, XXXXX,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       LEFT,  DOWN, RIGHT, XXXXX, XXXXX,  TILD,                   PLUS,  UNDS,  LBRC,  RBRC,  BSLS,  RSFT,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                    CMD, LOWEI,  BSPC,      SPC, RAIKN, OPTTB\
                              //`--------------------'  `--------------------'
  ),

  [_RAISE] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
          0,     1,     2,     3,     4,     5,                      6,     7,     8,     9,     0,   DEL,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      CTLES,    F7,    F8,    F9,   F10,   F11,                    F12,     4,     5,     6, XXXXX, XXXXX,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       LSFT,    F1,    F2,    F3,    F4,    F5,                     F6,     1,     2,     3,   DOT,  RSFT,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                    CMD, LOWEI,  BSPC,      DEL, RAIKN, OPTTB\
                              //`--------------------'  `--------------------'
  ),

  [_ADJUST] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
        TAB, XXXXX, XXXXX, XXXXX, XXXXX,  VOLU,                  XXXXX,  PGDN,  PGUP,   INS,   APP,  LGUI,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      CTLES, XXXXX, XXXXX, XXXXX, XXXXX,  VOLD,                   LEFT,  DOWN,    UP, RIGHT, XXXXX, XXXXX,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       LSFT, XXXXX, XXXXX, XXXXX, XXXXX,  MUTE,                  XXXXX,  HOME,   END, XXXXX, XXXXX,  RSFT,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                    CMD, LOWEI,   SPC,      ENT, RAIKN, OPTTB\
                              //`--------------------'  `--------------------'
  )
};

int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        iota_gfx_init(!has_usb());   // turns on the display
    #endif
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
void set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);

// const char *read_mode_icon(bool swap);
// const char *read_host_led_state(void);
// void set_timelog(void);
// const char *read_timelog(void);

LEADER_EXTERNS();

void matrix_scan_user(void) {
   iota_gfx_task();

   LEADER_DICTIONARY() {
       leading = false;

       SEQ_TWO_KEYS(KC_U, KC_U) {
           // ユーザーネームのタイプ
            SEND_STRING("morimoto_atsushi");
       }
       SEQ_TWO_KEYS(KC_U, KC_M) {
           // ユーザーネームのタイプ
            SEND_STRING("rollingteapot@gmail.com");
       }

       SEQ_TWO_KEYS(KC_V, KC_R) {
           // VS: 実行
           register_code(KC_RCTRL);
           tap_key(KC_F5, false);
           unregister_code(KC_RCTRL);
       }
       SEQ_TWO_KEYS(KC_V, KC_D) {
           // VS: デバッグ実行
           tap_key(KC_F5, false);
       }
       SEQ_TWO_KEYS(KC_V, KC_B) {
           // VS: ビルド
           tap_key(KC_F7, false);
       }
       SEQ_TWO_KEYS(KC_V, KC_K) {
           // VS: ブレークポイント
           tap_key(KC_F9, false);
       }
       SEQ_TWO_KEYS(KC_V, KC_C) {
           // VS: コンパイル
           register_code(KC_RCTRL);
           tap_key(KC_F7, false);
           unregister_code(KC_RCTRL);
       }

       leader_end();
   }
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
    // If you want to change the display of OLED, you need to change here
    matrix_write_ln(matrix, read_layer_state());
    matrix_write_ln(matrix, read_keylog());
    matrix_write_ln(matrix, read_keylogs());
    //matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
    //matrix_write_ln(matrix, read_host_led_state());
    //matrix_write_ln(matrix, read_timelog());
  } else {
    matrix_write(matrix, read_logo());
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  matrix_clear(&matrix);
  matrix_render_user(&matrix);
  matrix_update(&display, &matrix);
}
#endif//SSD1306OLED

// _RAISE, _LOWER の同時押しで _ADJUST に切り替える
uint32_t layer_state_set_user(uint32_t state) {
  return update_tri_layer_state(state, _RAISE, _LOWER, _ADJUST);
}

static bool lower_pressed = false;
static bool raise_pressed = false;
static uint16_t lower_pressed_time = 0;
static uint16_t raise_pressed_time = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  /* bool pressed = record->event.pressed; */

  if (record->event.pressed) {
#ifdef SSD1306OLED
    set_keylog(keycode, record);
#endif
    // set_timelog();
  }

  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        persistent_default_layer_set(1UL<<_QWERTY);
      }
      return false;
      break;

#if 1
    case LOWER:
      if (record->event.pressed) {
        lower_pressed = true;
        lower_pressed_time = record->event.time;

        layer_on(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);

        if (lower_pressed && (TIMER_DIFF_16(record->event.time, lower_pressed_time) < TAPPING_TERM)) {
          register_code(KC_LANG2); // for macOS
          register_code(KC_MHEN);
          unregister_code(KC_MHEN);
          unregister_code(KC_LANG2);
        }
        lower_pressed = false;
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        raise_pressed = true;
        raise_pressed_time = record->event.time;

        layer_on(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);

        if (raise_pressed && (TIMER_DIFF_16(record->event.time, raise_pressed_time) < TAPPING_TERM)) {
          register_code(KC_LANG1); // for macOS
          register_code(KC_HENK);
          unregister_code(KC_HENK);
          unregister_code(KC_LANG1);
        }
        raise_pressed = false;
      }
      return false;
      break;
#else
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case ADJUST:
        if (record->event.pressed) {
          layer_on(_ADJUST);
        } else {
          layer_off(_ADJUST);
        }
        return false;
        break;
#endif
    case RGB_MOD:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          rgblight_mode(RGB_current_mode);
          rgblight_step();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      return false;
      break;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      break;
	case MTYUN:
      if (record->event.pressed) {
          SEND_STRING("morimoto_atsushi");
      }
      return false;
	  break;

    default:
      if (record->event.pressed) {
          lower_pressed = raise_pressed = false;
      }
  }
  return true;
}

/* vim: set sts=2: */
