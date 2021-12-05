/*
    ST-V Multi OLED Game Selector for Darksoft - 1.0
    Date: 2021.12.05

    Description:
    Allows game selection for Darksoft ST-V Multi using rotary encoder with serial and OLED 128x32 output.

    Based on code for Darksoft ST-V Multi GAMESELECTOR Version 3.0C by miisalo
    URL: https://www.arcade-projects.com/threads/st-v-multicart-game-selector-with-lcd-screen.398/
    Please note that this iteration is not pin compatible with the original that it is based upon.

    Hackity hacked by Simon Lock (twitter.com/@a3rgan)
    Code Refactoring by Dennis V.B (twitter.com/@pointerfuction)

    Using code examples & libraries from Adafruit & Erriez (https://github.com/Erriez/ErriezArduinoLibraries)


  Arduino Nano:

                                   +-----+
                      +------------| USB |------------+
                      |            +-----+            |
       (Built-in LED) | [ ]D13/SCK        MISO/D12[X] | PIN_RELAY
          [OLED 3.3V] | [X]3.3V           MOSI/D11[X] | PIN_RESET
                      | [ ]V.ref     ___    SS/D10[X] | PIN_32
                      | [ ]A0       / N \       D9[X] | PIN_16
                      | [ ]A1      /  A  \      D8[X] | PIN_8
                      | [ ]A2      \  N  /      D7[X] | PIN_4
                      | [ ]A3       \_0_/       D6[X] | PIN_2
           [OLED SDA] | [X]A4/SDA               D5[X] | PIN_1
           [OLED SCL] | [X]A5/SCL               D4[X] | ROTARY_BTN_PIN
                      | [ ]A6              INT1/D3[X] | ROTARY_PIN1
                      | [ ]A7              INT0/D2[X] | ROTARY_PIN2
                  +5V | [X]5V                  GND[X] | GND
                      | [ ]RST                 RST[ ] |
           [OLED GND] | [X]GND   5V MOSI GND   TX1[ ] |
                      | [ ]Vin   [ ] [ ] [ ]   RX0[ ] |
                      |          [ ] [ ] [ ]          |
                      |          MISO SCK RST         |
                      | NANO                          |
                      +-------------------------------+
*/

#include <EEPROM.h>
#include <Wire.h>
#include <aergan_splash.h>          // Aergan.com - Logos
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ErriezRotaryHalfStep.h>   // Erriez Rotary Encoder Half Step library

#define VERSION            "ST-V Multi Game Selector for DarkSoft - 1.0 \n:: OLED Implementation ::\n"

// PINS
#define SERIAL_BAUD        115200 // Serial output baud rate - used for debug
#define ROTARY_PIN1        3      // Rotary Encoder CLK
#define ROTARY_PIN2        2      // Rotary Encoder DT
#define ROTARY_BTN_PIN     4      // Rotary Encoder SW
#define PIN_RESET          11     // Darksoft Reset PIN
#define PIN_RELAY          12     // Mirror of PIN_RESET [Unused]
#define PIN_1              5      // Darksoft Game Selector PIN +1
#define PIN_2              6      // Darksoft Game Selector PIN +2
#define PIN_4              7      // Darksoft Game Selector PIN +4
#define PIN_8              8      // Darksoft Game Selector PIN +8
#define PIN_16             9      // Darksoft Game Selector PIN +16
#define PIN_32             10     // Darksoft Game Selector PIN +32

// Defines
#define NUMBER_OF_GAMES    55    // Update this if you increate or decrease the games in the array below
#define MAX_NAME_SIZE      60    // Maximum number of characters per game entry
#define OLED_MAX_CHAR      21    // Maximum number of characters displayed simultaneously on a 128x32 OLED @ text size 1
#define DELAY_DSWRITE      20000 // Wait 20 seconds - roughly the time it takes for a full reset, game writes can often take longer
#define DELAY_INIT         1000
#define DELAY_BEFORE_RESET 200
#define DELAY_RESET        250

// Global Variables
int x, min_x, scroll_time, game_name_char, rotary_count_last, game_num = 0;
volatile int rotary_count = 0;
char show_game[MAX_NAME_SIZE];

// Erriez Rotary Code
RotaryHalfStep rotary(ROTARY_PIN1, ROTARY_PIN2, false, 25);
void rotaryInterrupt();

// Game definitions  note game_size has the trailing zero in it.
struct GAME {
  char game_name[MAX_NAME_SIZE];
  byte game_dips;
};
const GAME games[NUMBER_OF_GAMES] PROGMEM = {

  { "Astra Super Stars", B000001 },
  { "Baku Baku Animal", B000010 },
  { "Batman Forever ", B000011 },
  { "Columns 97 ", B000100 },
  { "Cotton 2 ", B000101 },
  { "Cotton Boomerang", B000110 },
  { "Danchi de Hanafuda", B000111 },
  { "Decathlete ", B001000 },
  { "Die Hard Arcade ", B001001 },
  { "Touryuu Densetsu Elan Doree", B001010 },
  { "Final Fight Revenge", B001011 },
  { "Funky Head Boxers", B001100 },
  { "Zenkoku Seifuku Bishojo Grand-Prix: Find Love", B001101 },
  { "Final Arch ", B001110 },
  { "Golden Axe: The Duel", B001111 },
  { "Guardian Force ", B010000 },
  { "Groove On Fight ", B010001 },
  { "Hanagumi Taisen Columns: Sakura Wars ", B010010 },
  { "Karaoke Quiz Intro Don Don!", B010011 },
  { "Pro Mahjong Kiwame S", B010100 },
  { "Maru-Chan de Goo!", B010101 },
  { "Mausuke no Ojama the World", B010110 },
  { "My Fair Lady: Virtual Mahjong II", B010111 },
  { "Othello Shiyouyo", B011000 },
  { "Pebble Beach Golflinks: The Great Shot", B011001 },
  { "Princess Kurara Daisakusen", B011010 },
  { "Puyo Puyo Sun ", B011011 },
  { "Radiant Silvergun", B011100 },
  { "Sando-R: Puzzle & Action ", B011101 },
  { "Sea Bass Fishing", B011110 },
  { "Shanghai: The Great Wall", B011111 },
  { "Shienryu ", B100000 },
  { "Soukyugurentai ", B100001 },
  { "Steep Slope Sliders", B100010 },
  { "Suikoenbu ", B100011 },
  { "Tecmo World Cup '98", B100100 },
  { "Virtua Fighter Kids", B100101 },
  { "Virtua Fighter Remix", B100110 },
  { "Winter Heat ", B100111 },
  { "Zen Nippon Pro Wrestling Featuring Virtua", B101000 },
  { "Ejihon Tantei Jimusyo", B101001 },
  { "DaeJeon! SanJeonSuJeon", B101010 },
  { "Critter Crusher ", B101011 },
  { "Magical Zunou Power", B101100 },
  { "Microman Battle Charge", B101101 },
  { "Name Club ", B101110 },
  { "Print Club 2 ", B101111 },
  { "Print Club 2 Vol. 3", B110000 },
  { "Print Club Pokemon B", B110001 },
  { "Stress Busters ", B110010 },
  { "Technical Bowling", B110011 },
  { "Virtual Mahjong ", B110100 },
  { "Choro Q ", B110101 },
  { "Dynamite Deka ", B111100 },
  { "Super Major League", B111101 },
};

// OLED Setup & Defines
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
#define OLED_Clear        { display.clearDisplay(); }
#define OLED_Start        { display.begin(SSD1306_SWITCHCAPVCC, 0x3C); OLED_Clear; display.display(); display.drawBitmap(0, (display.height() - Aergan_large_height) / 2, Aergan_large, Aergan_large_width, Aergan_large_height, 1); display.setTextWrap(false); display.setTextColor(WHITE); display.dim(true); display.display(); delay(1000); }
#define OLED_STV          { OLED_Clear; display.drawBitmap(0, (display.height() - OLED_STV_LOGO_height) / 2, OLED_STV_LOGO, OLED_STV_LOGO_width, OLED_STV_LOGO_height, 1); display.display(); delay(1000); }
#define OLED_STV_SCROLL   { OLED_Clear; display.drawBitmap(0, (display.height() - OLED_STV_LOGO_SCROLL_height) / 2, OLED_STV_LOGO_SCROLL, OLED_STV_LOGO_width, OLED_STV_LOGO_height, 1); display.display(); delay(250); }


// Functions
// ----------------------------------------------------------------------------------
// Decide to set PIN to LOW (Ground) or HIGH (Floating - as PIN is Active Low)
void checkPinSettings(int pin_mode, byte pin_value, byte value) {
  int mode = (pin_value & value) ? HIGH : LOW;
  pinMode(pin_mode, (mode == HIGH) ? OUTPUT : INPUT);   // Setting PIN mode to INPUT will leave PIN Floating
  if (mode == LOW) {
    digitalWrite(pin_mode, mode);                       // Setting PIN mode to OUTPUT and LOW will set PIN to Ground
  }
}

// Set each Game Selector PIN based on selected game
void setPins(int Game)
{
  byte pinValue = pgm_read_byte_near(&games[Game].game_dips);

  // checkPinSettings tests for LOW and LOW is 0 so actually (pinValue & <bit>)
  checkPinSettings(PIN_1,  pinValue, B000001);
  checkPinSettings(PIN_2,  pinValue, B000010);
  checkPinSettings(PIN_4,  pinValue, B000100);
  checkPinSettings(PIN_8,  pinValue, B001000);
  checkPinSettings(PIN_16, pinValue, B010000);
  checkPinSettings(PIN_32, pinValue, B100000);
}

// Reset Darksoft ST-V Multi (Device is Active Low reset)
void resetSTV() {
  // Send reset command to Darksoft, which will handle the reset of the ST-V itself
  delay(DELAY_BEFORE_RESET);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);
  digitalWrite(PIN_RESET, LOW);
  delay(DELAY_RESET);
  pinMode(PIN_RESET, INPUT);
  pinMode(PIN_RELAY, INPUT);
}


void setup() {
  // Set initial PIN modes
  pinMode(PIN_1, OUTPUT);
  pinMode(PIN_2, OUTPUT);
  pinMode(PIN_4, OUTPUT);
  pinMode(PIN_8, OUTPUT);
  pinMode(PIN_16, OUTPUT);
  pinMode(PIN_32, OUTPUT);
  pinMode(PIN_RESET, INPUT);
  pinMode(PIN_RELAY, INPUT);

  // Enable internal pull-up for the rotary BTN pin
  pinMode(ROTARY_BTN_PIN, INPUT_PULLUP);

  // Initialize pin change interrupt on both rotary encoder pins
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN1), rotaryInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN2), rotaryInterrupt, CHANGE);

  // Start Serial Output & Show Version
  Serial.begin(SERIAL_BAUD);
  delay(50);
  Serial.print("\n\n" + String(VERSION));

  // Delay before setting pins just to let everything initialize
  delay(DELAY_INIT);

  // Read last selected game from EEPROM
  game_num = EEPROM.read(0);

  // if not valid value, set it as first game
  if (game_num < 0 or game_num >= NUMBER_OF_GAMES) {
    game_num = 0;
  }
  rotary_count = game_num;
  Serial.print("Game Boot Number: ");
  Serial.println(rotary_count);
  // Set defaults
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setTextWrap(false);
  Init();
  update_game();
  Serial.print(String(show_game) + ": ");
  Serial.println(pgm_read_byte_near(&games[game_num].game_dips), BIN);
  // Set pins for last selected game
  setPins(game_num);
  // Reset Darksoft ST-V which will sync selected game
  resetSTV();
  // Start OLED & Show Splash
  OLED_Start;
  OLED_STV;
}

void update_game() {
  // Set game title based on selected game and calculate length
  game_num = rotary_count;
  strcpy_P(show_game, games[game_num].game_name);
  game_name_char = strlen(show_game);
  min_x = -((game_name_char - OLED_MAX_CHAR) * 6);
}

void Init()
{
  // Reset some variables on change
  scroll_time = 0;
  x = 0;
}

void rotaryInterrupt()
{
  int rotaryState;
  // Read rotary state (Counter clockwise) -2, -1, 0, 1, 2 (Clockwise)
  rotaryState = rotary.read();
  // Count up or down by using rotary speed
  if (rotaryState == 0) {
    // No change
    return;
  } else if (abs(rotaryState) >= 2) {
    rotary_count += rotaryState * 2;
    Init();
  } else {
    rotary_count += rotaryState;
    Init();
  }
  // Limit count to a minimum and maximum value
  if (rotary_count >= NUMBER_OF_GAMES - 1) {
    rotary_count = NUMBER_OF_GAMES - 1;
  }
  if (rotary_count <= 0) {
    rotary_count = 0;
  }
  // Print count value when count value changed
  if (rotary_count_last != rotary_count) {
    rotary_count_last = rotary_count;
  }
}

void loop() {
  update_game();
  // Poll Rotary BTN pin
  if (digitalRead(ROTARY_BTN_PIN) == 0) {
    Serial.print("Game Selected Number: ");
    Serial.println(game_num);
    setPins(game_num);
    Serial.print(String(show_game) + ": ");
    Serial.println(pgm_read_byte_near(&games[game_num].game_dips), BIN);
    // Check if selected game is not saved in EEPROM
    if (EEPROM.read(0) != game_num) {
      EEPROM.write(0, game_num);
      Serial.println("Saving to EEPROM");
    }
    resetSTV();
    OLED_STV;
    OLED_STV_SCROLL;
    display.startscrolldiagleft(0x07, 0x07);        // Hack to get display to scroll vertically
    delay(DELAY_DSWRITE);
    display.stopscroll();
    rotary_count = game_num;                        // Reset rotary count to current game number incase rotary was messed with during selection process
  }
  display.clearDisplay();
  display.drawBitmap(0, (display.height() - OLED_SEGA_STV_GUI_height) / 2, OLED_SEGA_STV_GUI, OLED_SEGA_STV_GUI_width, OLED_SEGA_STV_GUI_height, 1);
  display.setCursor(4, 23);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.print("   " + String(game_num + 1));      // Display current game number
  display.setCursor(78, 23);
  display.setTextColor(BLACK);
  display.print("   " + String(NUMBER_OF_GAMES));   // Display maximum number of games
  display.setTextSize(1);
  display.setCursor(x, 0);
  display.setTextColor(WHITE);
  display.print(show_game);                         // Display current game title
  display.display();
  // Handle OLED Game Title Scrolling
  if (game_name_char > OLED_MAX_CHAR) {
    if (x == 0 && scroll_time < 30) {
      scroll_time = scroll_time + 1;
    }
    else if (x < min_x && scroll_time > 1) {
      scroll_time = scroll_time - 1;
      if (scroll_time <= 1) {
        Init();     // Reset game title potition
      }
    }
    else {
      x = x - 2;    // Scroll game title left at 4 pixels per tick
    }
  }
  if (game_name_char <= OLED_MAX_CHAR) {
    Init();        // Reset game title potition
  }
}
