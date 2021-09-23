### TIYCS  

## Hostnames  
tiycsone.local, tiycstwo.local, etc.
(Not on the backup, then just raspberrypi.local)

## New version
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
