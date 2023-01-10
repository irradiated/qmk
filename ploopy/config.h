#pragma once

#include "config_common.h"

#define TAPPING_TERM 175
// DPI array w/ indices { 0, 1, 2 }
#define PLOOPY_DPI_OPTIONS { 600, 800, 1200 }
#define PLOOPY_DPI_DEFAULT 1

// drag scroll settings:

#define PLOOPY_DRAGSCROLL_MOMENTARY // Makes the key into a momentary key, rather than a toggle. OFF for tapdance config
// #define PLOOPY_DRAGSCROLL_MULTIPLIER 0.25 // Sets the DPI multiplier to use when drag scroll is enabled.
#define PLOOPY_DRAGSCROLL_FIXED // Normally, when activating Drag Scroll, it uses a fraction of the current DPI. You can define this to use a specific, set DPI rather than a fraction of the current DPI.
#define PLOOPY_DRAGSCROLL_DPI 100 // When the fixed DPI option is enabled, this sets the DPI to be used for Drag Scroll.
#define PLOOPY_DRAGSCROLL_INVERT // This reverses the direction that the scroll is performed.

// combo keys
#define COMBO_COUNT 1
#define COMBO_ONLY_FROM_LAYER 0 // combos always available on all layers

// mouse wheel
//#define MK_W_OFFSET_UNMOD 1 // 1 line per wheel action