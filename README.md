### TIYCS  

![Jonisk](./images/jonisk.png "Jonisk")

### Project
**This Is Your Captain Speaking**  
Daar zit je dan. Als één van de maximaal 16 passagiers op ruimteschip de Jonisk, op zoek naar Planet Bi. Dan verdwijnt tijdens een pitstop annex plaspauze op Planet iX de bemanning!
Op de twee monteurs in hun met synthesizers volgeladen machinekamer na.

De kapitein weet niet waar hij is, maar het lukt hem af en toe contact te leggen met de Jonisk.

Jouw hulp is nodig.
Start de automatische piloot en breng de missie naar Planet Bi tot een goed einde.
En als het éven meezit, red je ook nog de bemanning.

Succes…

### Modules
This repository contains code for:
- Driving the custom Jonisk lights (ESP32 / LED)
- Driving the custom LedBeams (ESP32 / WS2812B)
- Driving the visuals

## 2021
Code for visuals is total chaos. Used a MacBook Pro instead of Raspberry Pi's. 3 screens, 2 original views.

## 2022
### MainApp
MainApp is the app that runs on the Raspberry Pi. This will receive OSC messages to switch between states and show certain images.
To execute the program via SSH, first:  
`export DISPLAY=:0.0`  
Then run program with:  
`./bin/mainApp` (without sudo)  
The app uses ofAppGLFWWindow to be able to run fullscreen.

### To test @ OF / Raspberry Pi
- [ ] Star scene on Raspberry Pi. Probably no shaders... (for ofxPostProcessing).
- [ ] Sync video with Ableton Link?

### Ideas for new version (2022)
- [x] Battery powered. 4S 18650 circuit  
- [ ] ~~Drive LEDs directly from Battery~~  
  - [x] Use a 12V buck converter.
- [x] Use same voltage regulator to get a 3.3V  
- [x] Change MOSFETs to logic-level (3.3V) ones, or drive with NPN transistor  
- [ ] ~~Implement gyroscope~~  
  - [x] Board has pins for I2C
- [ ] ~~Research mic implementation~~  // Could be done with i2c board.

- [x] ESP32s2 or ESP32 WROOM? --> ESP32 WROOM
- [x] Rethink LED configuration (Strip + circle-panel for top?)

- [ ] ~~Build own cement base with screw holes~~  
  - [x] Use plastic cup that's already in Jonisks to attach PCB
- [ ] ~~Add screw from bottom to cement base~~  
- [x] RGBW instead of RGB?  
  - [x] Fix LED polarity issues

- [ ] Power on / off  
  - [ ] ~~Button (preferred)~~
  - [x] Battery connector
    - [ ] Test: (wake ESP32 when charger is plugged in)  

#### Software
- [ ] Set PWM freq in EEPROM, high freq for picture mode, low for performance mode  
- [ ] Implement internal lag for lower framerates  
- [x] Report battery status  

#### Barrier
Notes using [Barrier](https://github.com/debauchee/barrier), to control Linux machine with MacBook:  
Set MacBook to server, create the screens + names.  
Set Linux as client, with matching name.  
