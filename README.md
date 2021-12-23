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
- Star scene on Raspberry Pi. Probably no shaders... (for ofxPostProcessing).
- Sync video with Ableton Link?

### Ideas for new version (2022)
- Set PWM freq in EEPROM, high freq for picture mode, low for performance mode  
- Battery powered. 4S 18650 circuit  
- Drive LEDs directly from Battery  
- Use same voltage regulator to get a 3.3V  
- Change MOSFETs to logic-level (3.3V) ones, or drive with NPN transistor  
- Implement gyroscope  
- Research mic implementation  

- ESP32s2 or ESP32 WROOM?  
- Rethink LED configuration (Strip + circle-panel for top?)  

- Build own cement base with screw holes.  
- Add screw from bottom to cement base  
- RGBW instead of RGB?  
- Implement internal lag for lower framerates  

- Turn on / off  
  - Button (preferred)
  - Battery connector
