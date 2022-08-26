# industrial_joystick_project
The main objective of this project is to control the industrial joystick device CR1301 from the company 
IFM. This joystick is controlled over the CAN Bus by an ESP32 and uses the SAE J1939 protocol.
## System Description
### Standard Mode
When the joystick knob is rotated, the circular ring LEDs will be illuminated sequentially. 
For example, assuming the initial position of knob corresponds to 12 o’ clock. If you rotate the knob clockwise the LED in zone 2  will be turned on. If you keep rotating the knob until zone 3, the next LED will be turned on. If you rotate counter-clockwise, and you reach again a previous zone, the LED will be toggled (i.e., if it was on, it will now be off). By default, the colour of the LED should be white (RGB 255,255,255). \
### Color Change Mode
The demo also changes the colour of the circular ring LEDs.  When the knob button is pressed, the LED assigned to the current zone where the knob will be changed. To change the colour, the knob can now be rotated and select between colours. While the colour selection happens, the LED should change the colour at the same time.
Once the user finished selecting the colour, pressing the knob button again should save the settings and exit the Color Change Mode to go back to Standard Mode. \
### Reset Mode
To reset the joystick, pressing the top left side-button should turn off all the LEDS, restore the default colour (white) and the position of the knob to 12 o’ clock.
### Automatic Mode
When a assigned button is pressed all LEDs (side buttons + circular ring) are turned on and off frquently and their colour is changed at the same time.  Depending on the position of the knob, the time delay between each change of colour should increase or decrease. To return back to the manual 
mode, the same button must be pressed again.

## Further information
Will be added soon.
