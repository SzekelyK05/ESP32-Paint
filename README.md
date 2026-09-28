# ESP32 Paint
Esp32-based drawing app using 0.96 inch Oled display, joystick module and rotary encoder.

## Bill of Materials
The project needs an esp32 (ESP32-WROOM-32),  ky-040 rotary encoder, 0.96 inch oled display, ky-023 joystick module, breadboard and jumper wires.
No soldering needed.

## User guide
The speed and direction of the brush can be controlled using the joystick.
Pressing the joystick's built-in button will toggle between DRAWING and NO DRAWING.

The brush size can be changed using the rotary encoder, eg. turn right for larger brush.
Using the button of the rotary encoder, the screen can be reset.

## Code features
Using the joystick's analog values, the speed of the brush changes based on the stick's distance of the center.
The oled screen is updated using a 128x64 matrix, where each pixel is manipulated individually.
Hardware interrupt for the rotary encoder, which ensures smooth brush size changing.
Different brush depending on mode(drawing or not drawing).

## Wiring
Joystick module:    VRx -> Pin 32
                    VRy -> Pin 33
                    SW -> Pin 18
                    GND -> GND
                    +5 -> VIN

Rotary encoder:     CLK -> Pin 19
                    DT -> Pin 4
                    SW -> Pin 23
                    GND -> GND
                    + -> 3V3

Oled display:       SCL -> Pin 22
                    SDA -> Pin 21
                    GND -> GND
                    VCC -> 3V3

## Dependencies
Adafruit_GFX, Adafruit_SSD1306, Wire

## Installation

### Prerequisites
1. Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the ESP32 board package in the Arduino IDE (follow [Espressif's official guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) if you haven't done this before).

### Setup Steps
1. **Clone the repository:**
   ```bash
   git clone (https://github.com/SzekelyK05/ESP32 Paint.git)

(Or download the repository as a ZIP file and extract it).
2. Open the sketch: Open the src/main.ino file in the Arduino IDE.
3. Install dependencies: Go to Tools > Manage Libraries... in the Arduino IDE, search for and install the following:

        Adafruit GFX Library

        Adafruit SSD1306

Configure your board:

Go to Tools > Board and select ESP32 Dev Module (or your specific ESP32 variant).

Go to Tools > Port and select the USB port your ESP32 is connected to.

Flash the firmware: Click the Upload button to compile and push the code to your board.