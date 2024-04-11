# All-button IIDX controller

This repository is for a DIY controller that uses only buttons to play Beatmania IIDX and similar games. In particular, it uses one button for the turntable, toggling direction between presses. The controller could theoretically be made of whatever cheap buttons you have lying around, hastily attached to a Raspberry Pi Pico or other compatible board. I hope to also include measurements for example controllers at some point.

This project is not finished, and a lot of important configurable settings (like turntable speed) aren't there, but it is in a state where you could hook up 8 to 12 buttons to a Pi Pico and play IIDX (or use your favorite .BMS player).

My main goal is to make it particularly cheap and easy to get started with playing IIDX/BMS at home due to the expense of pre-built controllers and complex nature of constructing your own turntable if you decide do the DIY route. It's definitely not an accurate experience (especially if you do the low-cost 30mm button build), but I do want a controller with the right button spacing and distance from where an actual turntable would be.

## Features

7 main gameplay keys, 1 turntable button, and 4 extra keys. GPIO pins 2, 4, 6, 8, 10, 12, and 14 are keys 1-7 respectively. GPIO 26 is the turntable button. GPIO 16, 18, 20, and 22 are the four additional keys. The controller will report 12 buttons, but HID button 8 is unused by default.

Press the turntable button to move the simulated turntable, which is output as either an analog joystick position by default or as a mouse movement in keyboard+mouse mode. The direction toggles between presses.

Hold key 1 (GPIO 2) while plugging the unit in to enable keyboard+mouse mode.
Hold key 3 (GPIO 6) while plugging the unit in to enable bi-directional digital turntable mode, which reports a left or right joystick input when the turntable key is pressed. Currently, this mode cannot be combined with kb+m mode. This does not report a POV hat/d-pad input.
Hold key 5 (GPIO 10) while plugging the unit in to enable single-input digital turntable mode, which reports HID button 8 when the turntable key is pressed.

You can combine these three options. 1+3 would be bi-directional digital keyboard input mode (no mouse), which reports an up or down arrow key press when the turntable key is pressed. 1+5 is single button turntable mode, reporting the Q key when pressed.

roadmap of planned features:

- sample blueprint for controller with proper rectangle buttons for the keys and a 60mm square SDVX style button for the turntable area (in progress, check the [layouts](https://github.com/null1024/Pico-Game-Controller-cheap/tree/main/layouts) folder)
- sample blueprint for a "premium" option using a 100mm flat round button for the turntable (you could probably try a dome button)
- sample blueprint for controller with 30mm arcade buttons for everything.
- general code cleanup (it is a mess right now)
- support for an extra key ("key 0", dunno what GPIO I'll assign) to allow for hacky support for spinning the controller around for P2 side and Pop'n; technically this will be 2cm too close for P2 players in the blueprints (the key 0 cutout hole is ~74mm away from the turntable button hole and thus a bit too close, key 1's cutout is ~9mcm away which is about correct)
- reimplement LED lights for keypresses, I might have broken them

distant roadmap of features that may or may not ever happen:

- configuration button, you'd press it and one of the 7 keys to toggle settings
- support for extra buttons to specifically rotate the turntable axis in a given direction (for selecting songs)
- make it so that the turntable key defaults to spinning forwards if left alone for a bit (this shouldn't affect gameplay and would make menuing with the one button much nicer)
- remember last used mode/switch modes while on (dunno if I'll bother)
- sdvx all button config? this wouldn't need anything fancy, just mapping two digital buttons to the analog axis reported
- way to toggle turntable speed? (might move this to the planned feature list)
- full CAD files if you wanted to get things laser cut or 3D printed (you can probably use the current file as-is as a base)

doesn't work:

- LEDs don't work because I don't have any available to test with
- support for rotary encoders was removed

# Installation

Download Pico_Game_Controller.uf2 from inside [build_uf2](https://github.com/null1024/Pico-Game-Controller-cheap/tree/main/build_uf2). Hold the BOOTSEL button on your Pi Pico and plug it into your computer. The Pico should appear as a removable drive on your computer. Copy Pico_Game_Controller.uf2 onto the drive, and that's it. The Pico will automatically restart and appear as a controller.

When wiring up the controls, GPIO 2, 4, 6, 8, 10, 12, and 14 are keys 1-7 respectively. GPIO 26 is the turntable button. GPIO 16, 18, 20, and 22 are the four additional keys. Mount each button in some kind of enclosure and plug it all together. I still need to complete the blueprints, but accurate button spacing for the keys is available in the included one. The turntable button distance and position isn't final, however.

# Parts

This is a work in progress. All prices listed in USD, and were checked in Feburary 2024.

This project is designed around a Raspberry Pi Pico. There is little benefit in using a different board, as I won't be testing on it or using any additional features of other boards.

For inputs, the extreme low-budget version will just use any 30mm round arcade buttons, such as Sanwa OBSF-30s. These typically go for $2.50-$3 each and are readily available at just about any online retailer that sells arcade parts. You could get cheaper, but a game like IIDX demands quality inputs, so there isn't much reason to spend less. You will need at least 8 buttons. You might still be willing to get a larger button for the turntable button, too, even if it's more expensive.

The more arcade accurate unit uses the following:

- [Samducksa CWB 405 Button](https://www.us.istmall.co.kr/Product/Detail/view/pid/71/cid/164) x7 for the main keys (they're $3.40 each as of this writing, but shipping is expensive from Korea). Typically 3 black on top and 4 white on the bottom, but as a DIY project, you can do as you wish. They're 50x33 buttons, so if you are willing to risk using cheaper ones, you can. Take note that the switches and springs on many 50x33 buttons are extremely heavy and hard to press for use in a rhythm game.
- [IST 60UK Button](https://www.us.istmall.co.kr/Product/Detail/view/pid/67/cid/161) x1 for the turntable button (should be less than $8 each, also shipped from Korea). Any 60x60 arcade button would work. You might prefer a button like the 100mm round [Samducksa CWB 401B Button](https://www.us.istmall.co.kr/Product/Detail/view/pid/111/cid/161) (should be $9 or so), which is also flat on top, unlike typical 100mm buttons dome buttons, but the blueprint will expect a 60mm one. I have CWB 401B buttons, but they just aren't common, while 60x60 square is extremely ordinary. The springs on the CWB 401B are VERY heavy, you'll almost certainy need to swap them for something lighter. The button is also quite heavy in general. It looks nicer than the 60UK by a lot, and it's satisfyingly big, but it's not ideal.
- The remaining 4 buttons can be whatever, although you could use the [Samducksa CWB 406 Button](https://www.us.istmall.co.kr/Product/Detail/view/pid/72/cid/164) if you prefer. I will likely use OBSF-30s on the blueprints I'm planning to create, but unlike the spacing between the 7 main keys and the turntable, the position of these doesn't matter nearly as much.

You _could_ use arcade accurate Sanwa buttons, but it kind of defeats the point of this being a budget build. [Sanwa 60x60 buttons](https://www.us.istmall.co.kr/Product/Detail/view/pid/48/cid/165) are like $18.50 each right now, and [the 50x33 ones](https://www.us.istmall.co.kr/Product/Detail/view/pid/39/cid/165) are $12.50 each. $106 in buttons alone, before shipping. You would also need to replace the microswitches on the buttons, they're rather heavy for IIDX.

I am also designing an ambidextrous layout, using 50x33 buttons for the turntable on both sides of the key area.

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
