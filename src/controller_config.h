#ifndef CONTROLLER_CONFIG_H
#define CONTROLLER_CONFIG_H

#define TURNTABLE_KEY_INDEX 7         // I don't know if I actually used this lol
#define SW_GPIO_SIZE 12               // Number of switches
#define LED_GPIO_SIZE 12              // Number of switch LEDs
#define ENC_GPIO_SIZE 1               // Number of encoders
//#define ENC_PPR 24                    // Encoder PPR
#define MOUSE_SENS 5                  // Mouse sensitivity multiplier
#define ENC_DEBOUNCE true            // Encoder Debouncing
#define SW_DEBOUNCE_TIME_US 5000      // Switch debounce delay in us
#define ENC_PULSE (512)       // how fast the turntable button spins (lower is faster)
#define REACTIVE_TIMEOUT_MAX 1000000  // HID to reactive timeout in us
#define WS2812B_LED_SIZE 10           // Number of WS2812B LEDs
#define WS2812B_LED_ZONES 2           // Number of WS2812B LED Zones
#define WS2812B_LEDS_PER_ZONE \
  WS2812B_LED_SIZE / WS2812B_LED_ZONES  // Number of LEDs per zone

#ifdef PICO_GAME_CONTROLLER_C

// MODIFY KEYBINDS HERE, MAKE SURE LENGTHS MATCH SW_GPIO_SIZE!

//TODO: extra settings key maybe? dunno if I'll bother

//used in keyboard mode (press 1 during startup)
const uint8_t SW_KEYCODE[] = {
    HID_KEY_Z, HID_KEY_S, HID_KEY_X, HID_KEY_D,
    HID_KEY_C, HID_KEY_F, HID_KEY_V,
    HID_KEY_Q, //used in 1-key turntable keyboard mode (press 1+2 during startup)
    HID_KEY_Q, HID_KEY_2, HID_KEY_3, HID_KEY_4,
};

//used in 2-key turntable keyboard (press 1+3 during startup) mode
const uint8_t SW_KEYCODE_TURNTABLE[] = {
    HID_KEY_ARROW_UP,
    HID_KEY_ARROW_DOWN,
};

const uint8_t SW_KEYCODE_TURNTABLE_SINGLE = HID_KEY_SPACE;

//GPIO pins for each key
const uint8_t SW_GPIO[] = {
    //7 main keys
    2, 4, 6, 8,
    10, 12, 14,
    //turntable key
    26,
    //4 extra keys
    16, 18, 20, 22,
};

//GPIO pins for LEDs, use 0 for no light
//TODO: unimplemented
const uint8_t SW_GPIO_LED[] = {
    //7 main keys
    3, 5, 7, 9,
    11, 13, 15,
    //turntable key
    27,
    //4 extra keys
    0, 0, 0, 0
};

//const uint8_t ENC_GPIO[] = {0};  // TT(0, 1)
const bool ENC_REV[] = {false};  // Reverse Encoders
const uint8_t WS2812B_GPIO = 28;

#endif

extern bool joy_mode_check;

#endif