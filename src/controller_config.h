#ifndef CONTROLLER_CONFIG_H
#define CONTROLLER_CONFIG_H

#define TURNTABLE_KEY_INDEX 7
#define SW_GPIO_SIZE 12               // Number of switches
#define LED_GPIO_SIZE 12              // Number of switch LEDs
#define ENC_GPIO_SIZE 1               // Number of encoders
//#define ENC_PPR 24                    // Encoder PPR
#define MOUSE_SENS 5                  // Mouse sensitivity multiplier
#define ENC_DEBOUNCE true            // Encoder Debouncing
#define SW_DEBOUNCE_TIME_US 4000      // Switch debounce delay in us
#define ENC_PULSE (512)       // how fast the turntable button spins (lower is faster)
#define REACTIVE_TIMEOUT_MAX 1000000  // HID to reactive timeout in us
#define WS2812B_LED_SIZE 10           // Number of WS2812B LEDs
#define WS2812B_LED_ZONES 2           // Number of WS2812B LED Zones
#define WS2812B_LEDS_PER_ZONE \
  WS2812B_LED_SIZE / WS2812B_LED_ZONES  // Number of LEDs per zone

#ifdef PICO_GAME_CONTROLLER_C

// MODIFY KEYBINDS HERE, MAKE SURE LENGTHS MATCH SW_GPIO_SIZE
const uint8_t SW_KEYCODE[] = {
    HID_KEY_Z, HID_KEY_S, HID_KEY_X, HID_KEY_D,
    HID_KEY_C, HID_KEY_F, HID_KEY_V,
    HID_KEY_Q, //unused
    HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4
};

const uint8_t SW_GPIO[] = {
    //7 main keys
    2, 4, 6, 8,
    10, 12, 14,
    //this is the turntable key
    26,
    //other keys
    16, 18, 20, 22
};

//const uint8_t ENC_GPIO[] = {0};  // TT(0, 1)
const bool ENC_REV[] = {false};  // Reverse Encoders
const uint8_t WS2812B_GPIO = 28;

#endif

extern bool joy_mode_check;

#endif