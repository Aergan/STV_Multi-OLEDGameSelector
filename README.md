# STV_Multi-OLEDGameSelector
![aergan com_Invader_Green_125](https://user-images.githubusercontent.com/7755236/144746737-bc5ba8c1-92f1-48f0-8d71-455ba5ff6e6e.png)

**ST-V Multi OLED Game Selector for Darksoft - 1.0**

Date: 2021.12.05

**Description**

  Allows game selection for Darksoft ST-V Multi using generic rotary encoder with serial (used for debug) and OLED 128x32 I2C output.

  Based on code for Darksoft ST-V Multi GAMESELECTOR Version 3.0C by miisalo (https://www.arcade-projects.com/threads/st-v-multicart-game-selector-with-lcd-screen.398/)

  Please note that this iteration is not pin compatible with the original that it is based upon.

  Hackity hacked by Simon Lock (https://bsky.app/profile/aergan.com)
  
  Code Refactoring by Dennis van den Broek (https://bsky.app/profile/pointerfunction.bsky.social)
  
  Using code examples & libraries from Adafruit (https://github.com/adafruit/Adafruit_SSD1306) & Erriez (https://github.com/Erriez/ErriezArduinoLibraries)

![PXL_20211204_180253421](https://user-images.githubusercontent.com/7755236/144752050-325a20f3-e83e-4963-b5d4-a85a39df9d8a.jpg)


![PXL_20211204_180337650](https://user-images.githubusercontent.com/7755236/144752066-0b16f91a-f645-4902-81f2-e6e502c369c0.jpg)


Demo video
https://youtu.be/TMKgDG9jXHc


**BOM**

Items will be availible cheaper elsewhere, this is just for consistency/availibility.
1. Arduino Nano

    https://www.amazon.co.uk/dp/B01MS7DUEM/

2. Arduino Breakout board

    https://www.amazon.co.uk/dp/B08T1ZXS7K/

3. 128x32 OLED Display (I2C) - Any colour will work (typically blue, white or yellow)

    https://www.amazon.co.uk/AZDelivery-Display-Pixels-Arduino-Raspberry/dp/B079H2C7WH/

4. DuPont 2.54mm kit

    https://www.amazon.co.uk/gp/product/B07CTHCG5T/

5. M3 Hex Nylon stand off kit

    https://www.amazon.co.uk/gp/product/B09FXPX6PX/

6. Rotary Encoder

    https://www.amazon.co.uk/AZDelivery-Drehwinkelgeber-Rotary-Encoder-Arduino/dp/B079H3C98M/
    
>**BOM via Aliexpress / Ebay for 1x approx costs**

    Nano: £4
    Breakout board: £3
    OLED: £4
    Dupont kit: £6
    Nylon Hex kit: £6
    Rotary encoder: £4
    [ Approx Total: £27* ]
    
    *Does not include assembly time or cups of tea.


**Programming**

This assumes you are familiar with the Arduino platform and IDE.
1. Clone repository to Arduino projects folder (typically Documents\Arduino)
2. Connect up a Arduino Nano via USB and check it is recognised in the Arduino IDE (https://www.arduino.cc/en/software)

    You may need to update the bootloader or select Tools > Processor > old bootloader if the Nano is not recognised.
3. Satisfy project library dependancies via Tools > Manage Libraries as required
4. Hit Upload
5. Disconnect when successful and proceed to wiring


**Wiring**


  Arduino Nano:

                                   +-----+
                      +------------| USB |------------+
                      |            +-----+            |
       (Built-in LED) | [ ]D13/SCK        MISO/D12[X] | PIN_RELAY - duplicate output of PIN_RESET (Not used)
          [OLED 3.3V] | [X]3.3V           MOSI/D11[X] | PIN_RESET - to Darksoft Reset PIN
                      | [ ]V.ref     ___    SS/D10[X] | PIN_32    - to Darksoft GAME SELECTOR +32 PIN
                      | [ ]A0       / N \       D9[X] | PIN_16    - to Darksoft GAME SELECTOR +16 PIN
                      | [ ]A1      /  A  \      D8[X] | PIN_8     - to Darksoft GAME SELECTOR +8 PIN
                      | [ ]A2      \  N  /      D7[X] | PIN_4     - to Darksoft GAME SELECTOR +4 PIN
                      | [ ]A3       \_0_/       D6[X] | PIN_2     - to Darksoft GAME SELECTOR +2 PIN
           [OLED SDA] | [X]A4/SDA               D5[X] | PIN_1     - to Darksoft GAME SELECTOR +1 PIN
           [OLED SCL] | [X]A5/SCL               D4[X] | ROTARY_BTN_PIN - Typically SW / SWITCH
                      | [ ]A6              INT1/D3[X] | ROTARY_PIN1    - Typically DT / DATA
                      | [ ]A7              INT0/D2[X] | ROTARY_PIN2    - Typically CLK / CLOCK
                  +5V | [X]5V                  GND[X] | GND
                      | [ ]RST                 RST[ ] |
           [OLED GND] | [X]GND   5V MOSI GND   TX1[ ] |
                      | [ ]Vin   [#] [ ] [#]   RX0[ ] |
                      |          [ ] [ ] [ ]          |
                      |          MISO SCK RST         |
                      | NANO                          |
                      +-------------------------------+
                      # Connect 5V to Darksoft +5V
                      # Connect GND to Darksoft GND

                      After programming, the Arduino is then hooked up to be powered and grounded to the Darksoft Multi.
                      
                      Please *do not* power the Arduino from a separate source when Arcade Board / Darksoft is powered.
                      Please *do not* use USB to attempt to power the Arduino + Darksoft + Arcade Board.
                      Please *do not* have the USB cable hooked up when supplying power to the Darksoft + Arcade Board.


Rotary encoders for Arduino use come in various specifications and some have PIN1 (CLK) and PIN2 (DT) reversed. If so, just reverse the pins. Works with or without pull down resistors or bypass ceramic capacitors on the module (from testing over four different encoder types & configurations for GOTEK use).

**Darksoft ST-V Multi Pinout**

![image](https://user-images.githubusercontent.com/7755236/144748890-47d2378e-9ca0-4aa6-ab59-a1b64267f1ab.png)

**SEGA ST-V Reset**

  >Solder to R440 or PIN 8 of IC12.

![Wiring - STV Reset](https://user-images.githubusercontent.com/7755236/144751772-7b484798-9ffe-4b61-8364-0e78fd221424.jpg)

**Legal Disclaimer**

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
