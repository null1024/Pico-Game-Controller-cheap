# All-button IIDX controller

This repository is for a DIY controller that uses only buttons to play Beatmania IIDX and similar games. In particular, it uses one button for the turntable, toggling direction between presses. The controller could theoretically be made of whatever cheap buttons you have lying around, hastily attached to a Raspberry Pi Pico or other compatible board. I hope to also include measurements for example controllers.

GPIO 2, 4, 6, 8, 10, 12, and 14 are keys 1-7 respectively. GPIO 26 is the turntable button. GPIO 16, 18, 20, and 22 are the four additional keys.

## Features

done:

- analog reporting of turntable key in joystick mode

doesn't work:

- keyboard+mouse mode (TODO)
- LEDs
- analog input devices have been completely removed

roadmap:

- digital reporting of turntable key
- support for extra buttons to specifically rotate the turntable axis in a given direction
- turntable key defaults to spinning forwards if left alone for a second
- support for a lot of buttons
- sample blueprint for controller with proper rectangle buttons for the keys and a 50mm square SDVX style button for the turntable area
- sample blueprint for controller with 30mm arcade buttons for everything.

distant roadmap:

- sdvx all button config? this wouldn't need anything fancy, just mapping two digital buttons to the analog axis reported
- no idea if I'm going to actually support the lights because I don't have them
- remember last used mode/switch modes while on

---

Original README below:

# Pico-Game-Controller

This branch was developed with IIDX in mind. Capable of handling 11 buttons, 11 LEDs, 1 WS2812B RGB strip, and 1 encoder. Note: 12 buttons/LEDs are reported but B8 should probably not be used (https://github.com/veroxzik/arduino-konami-spoof)

Demo of this firmware running on Pocket SDVX Pico, purchasable at https://discord.gg/MmuKd73XbY

![Pocket SDVX Pico](demo.gif)

Currently working/fixed:

- Gamepad mode - default boot mode
- NKRO Keyboard & Mouse Mode - hold first button(gpio4) to enter kb mode
- HID LEDs with Reactive LED fallback
- ws2812b rgb on second core
- 2 ws2812b hid descriptor zones
- sdvx/iidx spoof - Tested on EAC - checkout branches release/pocket-sdvx-pico or release/pocket-iidx
- 1000hz polling
- Reversable Encoders with debouncing
- Switch debouncing
- Switch and LED pins are now staggered for easier wiring
- Fix 0-~71% encoder rollover in gamepad mode, uint32 max val isn't divisible evenly by ppr\*4 for joystick - thanks friends
- HID LEDs now have labels, thanks CrazyRedMachine
- refactor ws2812b into a seperate file for cleaner code & implement more RGB modes (added turbocharger mode) - hold second button (gpio 6) to swap to turbocharger mode; hold 9th button (gpio 20) to turn off RGB
- refactor debouncing algorithms into separate files for cleaner code

TODO:

- Store last mode in flash memory (probably implement into above TODO if possible) https://www.raspberrypi.org/forums/viewtopic.php?t=305570
- store configuration settings in a text file? consider implementing littlefs https://github.com/littlefs-project/littlefs https://www.raspberrypi.org/forums/viewtopic.php?t=313009 https://www.raspberrypi.org/forums/viewtopic.php?p=1894014#p1894014

How to Use:

- For basic flashing, see README in build_uf2
- Otherwise, setup the C++ environment for the Pi Pico as per https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf
- Build pico-examples directory once to ensure all the tinyusb and other libraries are there. You might have to move the pico-sdk folder into pico-examples/build for it to play nice.
- Move pico-sdk back outside to the same level directory as Pico-Game-Controller.
- Open Pico-Game-Controller in VSCode(assuming this is setup for the Pi Pico) and see if everything builds.
- Tweakable parameters are in controller_config.h

Thanks to:

- Everyone in the Cons & Stuff Discord for providing near instant support to questions.
- https://github.com/hathach/tinyusb/tree/master/examples/device/hid_composite
- https://github.com/mdxtinkernick/pico_encoders for encoders which performed better than both interrupts and polling.
- My SE buddies who helped come up with a solution for the encoder rollover edge case scenario.
- https://github.com/Drewol/rp2040-gamecon for usb gamepad descriptor info.
- https://github.com/veroxzik/arduino-konami-spoof for konami spoof usb descriptor info.
- https://github.com/veroxzik/roxy-firmware for nkro descriptor and logic info.
- KyubiFox for bringing clkdiv to my attention for encoder debouncing
- 4yn for turbocharger lighting
- SushiRemover for alternate debounce mode
