# The Obsidian Oracle

The Obsidian Oracle is an interactive, theatrical Magic 8-Ball built with Arduino. Encased in a mystical obelisk, the Oracle uses hardware sensors to listen to your voice, converting the sound waves into a chaotic seed to generate truly randomized fortunes. A servo-driven mechanical shutter and passive buzzer provide a dramatic, physical reveal.

## Hardware Requirements
*   1x Arduino Uno R3 (or Nano)
*   1x I2C LCD Display 16x2
*   1x Passive Buzzer
*   1x Push Button
*   1x Analog Joystick Module
*   1x Sound Sensor Module (Analog out)

## Circuit / Pin Wiring
| Component | Arduino Pin |
| :--- | :--- |
| **I2C LCD** | SDA -> A4, SCL -> A5 |
| **Joystick (VRy)** | A0 |
| **Sound Sensor (A0)** | A1 |
| **Push Button** | Digital 4 (Uses internal pull-up) |
| **Passive Buzzer** | Digital 3 (PWM) |

## Dependencies
You will need to install the following libraries in your Arduino IDE (`Sketch -> Include Library -> Manage Libraries`):
*   `LiquidCrystal I2C` by Frank de Brabander
*   `Wire` (Built-in Arduino library)

## How to Run the Project
1. Clone this repository to your local machine.
2. Open `src/Obsidian_Oracle/Obsidian_Oracle.ino` in the Arduino IDE.
3. Ensure your wiring matches the pinout table above. 
4. Select your board (e.g., Arduino Uno) and the correct COM port.
5. Click **Upload**.
6. **Interaction Guide:**
   - **Scroll Menu:** Push the joystick Up/Down to select the Oracle's personality (Classic, Sarcastic, Cryptic).
   - **Wake Oracle:** Press the Push Button.
   - **Ask Question:** Speak out loud into the sound sensor for 3 seconds.
   - **The Reveal:** Wait for the mechanical shutter to open and reveal your fate!
