/**
  Control of TFT, Touch, SD, LittleFS, RGB-LEDs for a user interface

  Copyright (c) 2022-2023 Ralf Wolters

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 3.0 of the License, or (at your option) any later version.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef SMARTVIEW_h
#define SMARTVIEW_h

// select board
#include "XD_qEdgeUI.h"

#include <hardware/flash.h>

#define DEBUG
#ifdef DEBUG
#define D(x) SYSSERIAL.print(x)     // always to system serial
#define Dln(x) SYSSERIAL.println(x) // with line feed
#else
#define D(x)                        // do nothing
#define Dln(x)
#endif // DEBUG

// Tones
#define NO_SOUND 0
#define BEEP 1
#define TRIAD 2
#define SHOOT 3
#define ALARM 4
#define MELODY 5
#define SPACE_GUN 6
#define FIBONACCI 7
#define NOISE 8
#define CLICK 9
#define NOISE_FALLING 10
#define NOISE_RISING 11

#define MAX_IMAGE_WDITH  320        // Adjust for your images -> PNG
#define TFT_BG_COL TFT_BLACK        // background color, for colors see TFT_eSPI
#define TFT_BACKLITE_MIN  30        // minimum backlight brightness
#define TFT_BACKLITE_INIT 200       // and init value
#define TFT_CAL_FILE "/touch.cal"   // touch calibration file
#define TFT_ANTHRACITE  0x2965      // special colors
#define TFT_ANTHRACITE2 0x4A49
#define TFT_THRESH       400        // touch threshold (Z)
#define RGB_BRIGHT        70        // initial RGB LED brightness 0-255
#define IDLE_TIME_MS   30000        // time to change to idle screeen

// color combinations for field display
#define FIELD_DEFAULT     0   // boxed
#define FIELD_REVERSE     1
#define FIELD_ERROR       2
#define FIELD_BIG         3
#define FIELD_7SEG        4
#define FIELD_TRANS       5  // transparent (no box) default
#define FIELD_TRANS_BIG   6  // transparent big
#define FIELD_TRANS_7SEG  7  // transparent bigger 7-segment
#define FIELD_DARK        8   // boxed


// RGB LED colors, colorwheel approximation
#define RGB_WHITE       0     // not really a color
#define RGB_RED         1
#define RGB_ORANGE     10
#define RGB_YELLOW     30
#define RGB_GREEN      85
#define RGB_CYAN      137
#define RGB_BLUE      170
#define RGB_PURPLE    213
#define RGB_BLACK     255   // also no color, off
#define RGB_UPDATE_MS  50

// external libraries used
#include "SPI.h"
#include <LittleFS.h>           // LittleFS for in memory filesystem
#include <SD.h>                 // SD-card

#include <PNGdec.h>             // PNG decoder for SD/LittleFS to read PNGs
extern PNG png;

#include <TFT_eSPI.h>           // TFT_eSPI
extern TFT_eSPI tft;
#include <TFT_eWidget.h>        // Widget library
#include <Adafruit_NeoPixel.h>  // RGB LED WS2812B

#include <Wire.h>               // I²C

#define LFS LittleFS                // abbreviation
#define FROM_SD   true              // to decide between SD or LittleFS
#define FROM_LFS  false

#define CONFIG_FILE_NAME  "config.cfg"
#define CONFIG_FILE_TEMP  "config.tmp"

// touch
#define NO_EVENT -1

/*
 *  for WIFI scan
 */
struct wifiEntry {
  String  ssid;
  int16_t rssi;
  int8_t  encryption;
};

struct wifiTable {
  int8_t count;
  wifiEntry entry[16];
};

// detection of file systems active
extern bool SDpresent;
extern bool LFSpresent;
extern String uniqueID, cryptKey;

extern uint8_t defaultKeySound;

/**
 *  functions in this scope
 */

void sViewSetFields(uint8_t x, uint8_t y);
void sViewDrawField(uint8_t field, String bname, uint8_t btype = FIELD_DEFAULT, uint8_t nrFields = 1);
int16_t sViewGetTouchField();

void qEdgeUiTouchTest();
uint32_t sViewLoadPNG(bool onSD, int16_t x, int16_t y, String pngfname);
bool sViewScreenshot();

void sViewInit();
void sViewInitLFS();
bool sViewInitSD();
bool sViewInitSerial(uint8_t portnr, uint32_t baudrate);
void sViewTouchCal(bool force);

void sViewRGBbright(uint8_t bright);
void sViewRGBset(uint8_t ledno, uint8_t color);
void sViewRGBsetRandom();

void sViewBacklite(uint8_t value);
uint8_t sViewKey();
uint8_t sViewGetKey();
void qEdgeUiBuzzer(uint8_t stype = 255);

/*
 * configuration management and crypt helpers
 */
String simpleEncrypt(String source);
String simpleDecrypt(String crypto);
void setConfigParameter(String parameter, String value, bool encrypted);
String getConfigParameter(String parameter, bool encrypted, String preset = "");

/*
 *  WiFi
 */
wifiTable wifiScan();
bool ntpConnect();
bool wifiConnect(String ssidStr = "");

// unique ID
String getUniqueID();

/**
 *  callback functions for PNGdec library to read PNG files
 */
void * pngOpen(const char *filename, int32_t *size);
void pngClose(void *handle);
int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length);
int32_t pngSeek(PNGFILE *page, int32_t position);
void pngDraw(PNGDRAW *pDraw);

/*
 * notes
 */

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978



#endif
