# All-button IIDX controller

This repository is for a DIY controller that uses only buttons to play Beatmania IIDX and similar games. In particular, it uses one button for the turntable, toggling direction between presses. The controller could theoretically be made of whatever cheap buttons you have lying around, hastily attached to a Raspberry Pi Pico or other compatible board. I hope to also include measurements for example controllers at some point.

This project is not finished, but is in a state where you could hook up 8 or more buttons to a Pi Pico and play IIDX (or your favorite .BMS player).

My main goal is to make it particularly cheap and easy to get started with playing IIDX/BMS at home due to the expense of pre-built controllers and complex nature of constructing your own turntable if you decide do the DIY route.

## Features

GPIO 2, 4, 6, 8, 10, 12, and 14 are keys 1-7 respectively. GPIO 26 is the turntable button. GPIO 16, 18, 20, and 22 are the four additional keys.

Press the turntable button to move the simulated turntable. The direction toggles between presses.

Hold key 1 (GPIO 2) while plugging in the unit to enable keyboard+mouse mode.

doesn't work:

- keyboard+mouse mode (TODO)
- LEDs
- support for rotary encoders has been completely removed

roadmap:

- support for extra buttons to specifically rotate the turntable axis in a given direction (for selecting songs)
- make it so that the turntable key defaults to spinning forwards if left alone for a second
- support for a lot of buttons
- sample blueprint for controller with proper rectangle buttons for the keys and a 50mm square SDVX style button for the turntable area
- sample blueprint for controller with 30mm arcade buttons for everything.

distant roadmap of features that may or may not ever happen:

- support for digital reporting of turntable key (either just as one button or two)
- remember last used mode/switch modes while on
- sdvx all button config? this wouldn't need anything fancy, just mapping two digital buttons to the analog axis reported
- no idea if I'm going to actually support LED lights because I don't have them
- maybe allow rotary input again? eg, to have a cheap knob to use when selecting songs?
- way to toggle turntable speed?
- actual CAD files if you wanted to get things laser cut

# Parts

This is a work in progress. All prices listed in USD, and were checked in Feburary 2024.

This project is designed around a Raspberry Pi Pico, and there is little benefit in using a different board, as I won't be testing on it or using any additional features of other boards. 

For inputs, the extreme low-budget version will just use any 30mm round arcade buttons, such as Sanwa OBSF-30s. These typically go for $2.50-$3 each and are readily available at just about any online retailer that sells arcade parts. You could get cheaper, but a game like IIDX demands quality inputs, so there isn't much reason to spend less.

The more arcade accurate unit uses the following:

- [Samducksa CWB 405 Button](https://www.us.istmall.co.kr/Product/Detail/view/pid/71/cid/164) x7 for the main keys (they're $3.40 each as of this writing, but shipping is expensive from Korea). Typically 3 black on top and 4 white on the bottom, but as a DIY project, you can do as you wish. They're 50x33 buttons, so if you are willing to risk using cheaper ones, you can. Take note that the switches and springs on many 50x33 buttons are extremely heavy and hard to press for use in a rhythm game.
- [IST 60UK Button](https://www.us.istmall.co.kr/Product/Detail/view/pid/67/cid/161) x1 for the turntable button (should be less than $8 each, also shipped from Korea). Any 60x60 arcade button would work. You might prefer a button like the 100mm round [Samducksa CWB 401B Button](https://www.us.istmall.co.kr/Product/Detail/view/pid/111/cid/161) (should be $9 or so), which is also flat on top, unlike typical 100mm buttons, but the blueprint will expect a 60mm one. I would use the CWB 401B if I had one.
- The remaining 4 buttons can be whatever, although you could use the [Samducksa CWB 406 Button](https://www.us.istmall.co.kr/Product/Detail/view/pid/72/cid/164) if you prefer. I will likely use OBSF-30s on the blueprints I'm planning to create, but unlike the spacing between the 7 main keys and the turntable, the position of these doesn't matter nearly as much.


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
