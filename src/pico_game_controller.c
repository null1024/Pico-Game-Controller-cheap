/*
 * Pico Game Controller
 * @author SpeedyPotato
 */

/*
Modified by null1024.
A lot of my changes are brutally, hideously hacky.
There is definitely dead code everywhere.
WS2812B lighting is probably broken since I don't have it.

TODO list:
* digital turntable modes
* modifier to configure turntable speed in analog mode
* support for per-button LEDs (once I get them, lol)
* clean up some of the dead code
*/
#define PICO_GAME_CONTROLLER_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "controller_config.h"
#include "encoders.pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"
// clang-format off
#include "debounce/debounce_include.h"
#include "rgb/rgb_include.h"
// clang-format on

PIO pio, pio_1;
uint32_t enc_val[ENC_GPIO_SIZE];
uint32_t prev_enc_val[ENC_GPIO_SIZE];
int cur_enc_val[ENC_GPIO_SIZE];

bool prev_sw_val[SW_GPIO_SIZE];
uint64_t sw_timestamp[SW_GPIO_SIZE];

//switch between sending keyboard or mouse data every poll
bool kbm_report;

uint64_t reactive_timeout_timestamp;

void (*ws2812b_mode)();
void (*loop_mode)();
uint16_t (*debounce_mode)();

//joystick or keyboard
bool joy_mode_check = true;

//TODO: actually implement digital turntable mode
//analog output (mouse/analog stick) or digital output (d-pad/button/keyboard)
bool turntable_digital_mode=false;
//use toggling directions if false (eg, swap between up/down) or emit a single button press if true
bool turntable_digital_single_key=false;

int8_t turntable_dir=1; //1 for turning forward, -1 for turning back
int8_t turntable_held=0; //0 for unpressed, 1 for just pressed, 2 for held

union {
  struct {
    uint8_t buttons[LED_GPIO_SIZE];
    RGB_t rgb[WS2812B_LED_ZONES];
  } lights;
  uint8_t raw[LED_GPIO_SIZE + WS2812B_LED_ZONES * 3];
} lights_report;

/**
 * WS2812B Lighting
 * @param counter Current number of WS2812B cycles
 **/
void ws2812b_update(uint32_t counter) {
  if (time_us_64() - reactive_timeout_timestamp >= REACTIVE_TIMEOUT_MAX) {
    ws2812b_mode(counter);
  } else {
    for (int i = 0; i < WS2812B_LED_ZONES; i++) {
      for (int j = 0; j < WS2812B_LEDS_PER_ZONE; j++) {
        put_pixel(urgb_u32(lights_report.lights.rgb[i].r,
                           lights_report.lights.rgb[i].g,
                           lights_report.lights.rgb[i].b));
      }
    }
  }
}

/**
 * HID/Reactive Lights
 * TODO: re-enable, test to make sure it works still
 **/
/*
void update_lights() {
  for (int i = 0; i < LED_GPIO_SIZE; i++) {
    if (time_us_64() - reactive_timeout_timestamp >= REACTIVE_TIMEOUT_MAX) {
      if (!gpio_get(SW_GPIO[i])) {
        gpio_put(LED_GPIO[i], 1);
      } else {
        gpio_put(LED_GPIO[i], 0);
      }
    } else {
      if (lights_report.lights.buttons[i] == 0) {
        gpio_put(LED_GPIO[i], 0);
      } else {
        gpio_put(LED_GPIO[i], 1);
      }
    }
  }
}
*/

//holds controller state info
struct report {
  uint16_t buttons; //bitfield of 16 buttons
  uint8_t joy0; //0-255 x direction
  uint8_t joy1; //0-255 y direction
} report;

//Gamepad mode
void joy_mode() {
  if (tud_hid_ready()) {
    //read the turntable key
    int turntable_current=(report.buttons>>7)&1;
    if (turntable_current) {
        turntable_held++;
        //set direction
        if (turntable_held==1) {
          turntable_dir=-turntable_dir;
        }
        //clamp
        if (turntable_held>2) { turntable_held=2; }
    }
    //release turntable
    else {
      turntable_held=0;
    }
    cur_enc_val[0]+=turntable_current*turntable_dir;
    while (cur_enc_val[0] < 0) cur_enc_val[0] = ENC_PULSE + cur_enc_val[0];
    cur_enc_val[0] %= ENC_PULSE;
    //two-direction turntable key vs single input mode
    if (!turntable_digital_single_key) {
      report.buttons=report.buttons & ~((uint16_t)1<<7); //clear the turntable button
      if (!turntable_digital_mode) {
        report.joy0 = ((double)cur_enc_val[0] / ENC_PULSE) * (UINT8_MAX + 1); //emit a looping analog value
      } else {
        if (turntable_held>0) {
          report.joy0=127+(127*turntable_dir); //emit hard left/right
        } else {
          report.joy0=127; //emit center
        }
      }
    }
    //single-direction turntable
    else {
      //we don't need to do anything else since we didn't clear the turntable's key (8 by default)
      report.joy0=127; //emit center
    }
    report.joy1=127; //wasn't getting Windows to recognize the turntable properly without this
    tud_hid_n_report(0x00, REPORT_ID_JOYSTICK, &report, sizeof(report));
  }
}


//Keyboard Mode
void key_mode() {
  //TODO: handle digital turntable mode
  if (tud_hid_ready()) {  // Wait for ready, updating mouse too fast hampers
                          // movement
    if (kbm_report) {
      /*------------- Keyboard -------------*/
      report.buttons=report.buttons & ~((uint16_t)1<<7); //clear the turntable button
      uint8_t nkro_report[32] = {0};
      for (int i = 0; i < SW_GPIO_SIZE; i++) {
        if ((report.buttons >> i) % 2 == 1) {
          uint8_t bit = SW_KEYCODE[i] % 8;
          uint8_t byte = (SW_KEYCODE[i] / 8) + 1;
          if (SW_KEYCODE[i] >= 240 && SW_KEYCODE[i] <= 247) {
            nkro_report[0] |= (1 << bit);
          } else if (byte > 0 && byte <= 31) {
            nkro_report[byte] |= (1 << bit);
          }
        }
      }
      tud_hid_n_report(0x00, REPORT_ID_KEYBOARD, &nkro_report,
                       sizeof(nkro_report));
    } else {
      /*------------- Mouse -------------*/
      /*
      int delta[ENC_GPIO_SIZE] = {0};
      
      // find the delta between previous and current enc_val
      int delta[ENC_GPIO_SIZE] = {0};
      for (int i = 0; i < ENC_GPIO_SIZE; i++) {
        delta[i] = (enc_val[i] - prev_enc_val[i]) * (ENC_REV[i] ? 1 : -1);
        prev_enc_val[i] = enc_val[i];
      }
      
      int turntable_current=(report.buttons>>7)&1;
      if (turntable_current) {
          turntable_held++;
          //set direction
          if (turntable_held==1) {
            turntable_dir=-turntable_dir;
          }
          //clamp
          if (turntable_held>2) { turntable_held=2; }
      }
      //release turntable
      else {
        turntable_held=0;
      }
      cur_enc_val[0]+=turntable_current*turntable_dir;
      */
      //while (cur_enc_val[0] < 0) cur_enc_val[0] = ENC_PULSE + cur_enc_val[0];
      //cur_enc_val[0] %= ENC_PULSE;
      int turntable_current=(report.buttons>>7)&1;
      if (turntable_current) {
          turntable_held++;
          //set direction
          if (turntable_held==1) {
            turntable_dir=-turntable_dir;
          }
          //clamp
          if (turntable_held>2) { turntable_held=2; }
      }
      //release turntable
      else {
        turntable_held=0;
      }
      //TODO: speed control
      tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, turntable_current*turntable_dir, 0, 0,
                           0);
    }
    // Alternate reports
    kbm_report = !kbm_report;
  }
}

/**
 * Update Input States
 * Note: Switches are pull up, negate value
 **/
void update_inputs() {
  for (int i = 0; i < SW_GPIO_SIZE; i++) {
    // If switch gets pressed, record timestamp
    if (prev_sw_val[i] == false && !gpio_get(SW_GPIO[i]) == true) {
      sw_timestamp[i] = time_us_64();
    }
      prev_sw_val[i] = !gpio_get(SW_GPIO[i]);    
  }
}

/**
 * DMA Encoder Logic For 2 Encoders
 **/
void dma_handler() {
  uint i = 1;
  int interrupt_channel = 0;
  while ((i & dma_hw->ints0) == 0) {
    i = i << 1;
    ++interrupt_channel;
  }
  dma_hw->ints0 = 1u << interrupt_channel;
  if (interrupt_channel < 4) {
    dma_channel_set_read_addr(interrupt_channel, &pio->rxf[interrupt_channel],
                              true);
  }
}

/**
 * Second Core Runnable
 **/
void core1_entry() {
  uint32_t counter = 0;
  while (1) {
    ws2812b_update(++counter);
    sleep_ms(5);
  }
}

/**
 * Initialize Board Pins
 **/
void init() {
  // LED Pin off, blinks with turntable input
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  gpio_put(25, 0);

  // Set up the state machine for encoders
  pio = pio0;
  uint offset = pio_add_program(pio, &encoders_program);

  // Setup Encoders
  for (int i = 0; i < ENC_GPIO_SIZE; i++) {
    enc_val[i], prev_enc_val[i], cur_enc_val[i] = 0;
    /*
    encoders_program_init(pio, i, offset, ENC_GPIO[i], ENC_DEBOUNCE);

    dma_channel_config c = dma_channel_get_default_config(i);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio, i, false));

    dma_channel_configure(i, &c,
                          &enc_val[i],   // Destination pointer
                          &pio->rxf[i],  // Source pointer
                          0x10,          // Number of transfers
                          true           // Start immediately
    );
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    dma_channel_set_irq0_enabled(i, true);
    */
  }

  reactive_timeout_timestamp = time_us_64();

  // Set up WS2812B
  pio_1 = pio1;
  uint offset2 = pio_add_program(pio_1, &ws2812_program);
  ws2812_program_init(pio_1, ENC_GPIO_SIZE, offset2, WS2812B_GPIO, 800000,
                      false);

  // Setup Button GPIO
  for (int i = 0; i < SW_GPIO_SIZE; i++) {
    prev_sw_val[i] = false;
    sw_timestamp[i] = 0;
    gpio_init(SW_GPIO[i]);
    gpio_set_function(SW_GPIO[i], GPIO_FUNC_SIO);
    gpio_set_dir(SW_GPIO[i], GPIO_IN);
    gpio_pull_up(SW_GPIO[i]);
  }
/*
  // Setup LED GPIO
  for (int i = 0; i < LED_GPIO_SIZE; i++) {
    gpio_init(LED_GPIO[i]);
    gpio_set_dir(LED_GPIO[i], GPIO_OUT);
  }
*/
  // Set listener bools
  kbm_report = false;

  // hold 1 during startup for kb+m mode
  if (!gpio_get(SW_GPIO[0])) {
    loop_mode = &key_mode;
    joy_mode_check = false;
  } else {
    loop_mode = &joy_mode;
    joy_mode_check = true;
  }

  //hold 3 during startup for 2 button digital turntable mode
  if (!gpio_get(SW_GPIO[2])) {
    turntable_digital_mode=true;
  }

  //hold 5 during startup for 1 button digital turntable mode
  if (!gpio_get(SW_GPIO[4])) {
    turntable_digital_mode=true;
    turntable_digital_single_key=true;
  }

  // RGB Mode Switching
  if (!gpio_get(SW_GPIO[1])) {
    ws2812b_mode = &turbocharger_color_cycle;
  } else {
    ws2812b_mode = &ws2812b_color_cycle;
  }

  // Debouncing Mode
  debounce_mode = &debounce_eager;

  // Disable RGB
  if (gpio_get(SW_GPIO[8])) {
    multicore_launch_core1(core1_entry);
  }
}

/**
 * Main Loop Function
 **/
int main(void) {
  board_init();
  init();
  tusb_init();

  while (1) {
    tud_task();  // tinyusb device task
    update_inputs();
    report.buttons = debounce_mode();
    loop_mode();
    //update_lights();
  }

  return 0;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t* buffer,
                               uint16_t reqlen) {
  // TODO not Implemented
  (void)itf;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const* buffer,
                           uint16_t bufsize) {
  (void)itf;
  if (report_id == 2 && report_type == HID_REPORT_TYPE_OUTPUT &&
      bufsize >= sizeof(lights_report))  // light data
  {
    size_t i = 0;
    for (i; i < sizeof(lights_report); i++) {
      lights_report.raw[i] = buffer[i];
    }
    reactive_timeout_timestamp = time_us_64();
  }
}
