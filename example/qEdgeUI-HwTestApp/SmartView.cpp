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

#include "SmartView.h"

TFT_eSPI tft = TFT_eSPI();  // TFT driver

PNG png;                    // PNG decoder driver
File pngfile;

bool SDactive = false;      // decision where to read PNG file
int16_t xpos = 0;           // position to write PNG
int16_t ypos = 0;

bool SDpresent;             // file systems active after init
bool LFSpresent;

uint8_t RasterXfields = 4;  // Raster fields for buttons/touch
uint8_t RasterYfields = 3;  // with default 4 x 3

String uniqueID, cryptKey;  // unique ID also for crypt

uint8_t defaultKeySound = 1; // sound if touch/key pressed

// objects from libraries needed
#ifdef RGB_LEDS
Adafruit_NeoPixel LED(RGB_LED_NO, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);  // RGB LEDs
#endif



/**
 *  set field raster x * y for buttons and touch
 */
void sViewSetFields(uint8_t x, uint8_t y)
{
  RasterXfields = x;
  RasterYfields = y;
}

/**
 *  Draw a Field/Button in a field in a raster xf * yf
 *  btype defines the colors 0 = normal,  1 = reverse, 2 .. whatever you expand
 *  dim is the raster dimension (1, n)
 */
void sViewDrawField(uint8_t field, String bname, uint8_t btype, uint8_t nrFields)
{
  uint16_t textFill, outline, text, w, w1, h, x1, y1;
  uint8_t r, d, font;

  // theme standard presets
  uint16_t themeText    = TFT_GREEN;
  uint16_t themeFill    = TFT_ANTHRACITE;
  uint16_t themeOutline = TFT_ANTHRACITE2;

  // select 
  switch(btype) {
    case FIELD_DEFAULT:
      text     = themeText;
      textFill = themeFill;
      outline  = themeOutline;
      font     = 2;
      break;
    case FIELD_REVERSE:
      text     = TFT_WHITE;
      textFill = themeOutline;
      outline  = themeFill;
      font     = 2;
      break;
    case FIELD_ERROR:
      text     = TFT_WHITE;
      textFill = TFT_RED;
      outline  = themeFill;
      font     = 2;
      break;
    case FIELD_BIG: // larger font
      text     = themeText;
      textFill = themeFill;
      outline  = themeOutline;
      font     = 4;
      break; 
    case FIELD_7SEG: // 7-segment font
      text     = themeText;
      textFill = themeFill;
      outline  = themeOutline;
      font     = 7;
      break; 
    // unboxed variants
    case FIELD_TRANS:
      text     = themeText;
      textFill = TFT_TRANSPARENT;
      outline  = TFT_TRANSPARENT;
      font     = 2;
      break;
    case FIELD_TRANS_BIG:  // larger font
      text     = themeText;
      textFill = TFT_TRANSPARENT;
      outline  = TFT_TRANSPARENT;
      font     = 4;
      break; 
    case FIELD_TRANS_7SEG: // 7-segment font
      text     = themeText;
      textFill = TFT_TRANSPARENT;
      outline  = TFT_TRANSPARENT;
      font     = 7;
      break; 
    case FIELD_DARK:
      text     = TFT_WHITE;
      textFill = TFT_BLACK;
      outline  = themeOutline;
      font     = 2;
      break;
    default:
      text     = themeText;
      textFill = themeFill;
      outline  = themeOutline;
      font     = 2;
      break;
  }

  uint16_t xSize = tft.width() / RasterXfields;     // width of one field horizontal in dots w1
  uint16_t ySize = tft.height() / RasterYfields;    // height of one field vertical in dots h
  uint16_t xButtonDots, yButtonDots;                // width of complete button
  
  if (nrFields > RasterXfields) {                   // more than one raster in vertical
    xButtonDots = RasterXfields * xSize;
    yButtonDots = (nrFields / RasterXfields) * ySize;
  } else {
    xButtonDots = nrFields * xSize;
    yButtonDots = ySize;
  }


  uint16_t cornerRad = min(xSize, ySize) / 8;        // Corner radius 20% from minimum dimension
  uint16_t buttonDist = cornerRad / 2;               // distance between buttons and to outline
  uint16_t shadowShift = 0;                          // shift for shadow
  
  uint16_t topLeftX = (field % RasterXfields) * xSize;
  uint16_t topLeftY = (field / RasterXfields) * ySize;
 
  if (outline != TFT_TRANSPARENT) {
    tft.fillSmoothRoundRect(topLeftX + buttonDist, 
                            topLeftY + buttonDist, 
                            xButtonDots - (2 * buttonDist), 
                            yButtonDots - (2 * buttonDist), 
                            cornerRad, 
                            outline);
    }
  if (textFill != TFT_TRANSPARENT) {
    tft.fillSmoothRoundRect(topLeftX + (2 * buttonDist) + shadowShift, 
                            topLeftY + (2 * buttonDist) + shadowShift, 
                            xButtonDots - (4 * buttonDist), 
                            yButtonDots - (4 * buttonDist), 
                            cornerRad, 
                            textFill);
  } else {
    textFill = TFT_BG_COL;
  }
  
  uint8_t tempdatum = tft.getTextDatum();         // save tft values 
  uint16_t tempPadding = tft.getTextPadding();
  tft.setTextDatum(MC_DATUM);                     // set to centered
  tft.setTextColor(text, textFill);               // set colors text/background
  tft.setTextPadding(tft.textWidth(bname, font)); // set overwrite
  tft.drawString(bname, 
                 topLeftX + (xButtonDots / 2) + shadowShift, 
                 topLeftY + (yButtonDots / 2) + shadowShift, 
                 font);
  tft.setTextDatum(tempdatum);      // restore tft vales
  tft.setTextPadding(tempPadding);
}

/**
 * Touch detection with raster, returns raster field, top left is 0
 */
int16_t sViewGetTouchField()
{
  uint16_t xt, yt, xw, yw;
  uint32_t dt, dpress = 0;

  if (!tft.getTouch(&xt, &yt, TFT_THRESH)) return(NO_EVENT); // nothing detected
  dt = millis(); // touch length detection
  while (tft.getTouch(&xt, &yt, TFT_THRESH)); // wait until released
  xw = tft.width();
  yw = tft.height();
  dpress = millis() - dt;
  if (dpress > 10000) {
    sViewTouchCal(true);  // force touch recalibration if pressed > 10s
  }
  D("Touch press ms = ");
  Dln(dpress);
  return( ((yt * RasterYfields) / yw) * RasterXfields +  ((xt * RasterXfields) / xw)  );
}

/**
 * Optical touch calibration test for the qEdgeUI touch hardware
 */
void qEdgeUiTouchTest()
{
  const uint8_t ESC = 79;
  uint16_t xt, yt, zt, i, field;
  bool refresh = true;

  sViewSetFields(10, 8);
  while (true) {    // until ESC touched
    if (tft.getTouch(&xt, &yt, TFT_THRESH)) {
      refresh = true;
      zt = tft.getTouchRawZ();
    } else {
      xt = tft.width()/2;
      yt = tft.height()/2;
      zt = 0;
    }
   #ifdef ENABLE_SCREEN_SHOT_FCT
    if (sViewGetKey() == PHYSICAL_KEY3) {
      sViewScreenshot();
      sViewSetFields(10, 8);
      refresh = true;
     }
   #endif
    if (refresh) {
      tft.fillScreen(TFT_BG_COL);
      for (i = 10; i < ESC; ++i) {
        sViewDrawField(i, String(i));
      }
      sViewDrawField(ESC, "ESC");
      field = ((yt * RasterYfields) / tft.height()) * RasterXfields + ((xt * RasterXfields) / tft.width());
      if (field == ESC) return;
      if (field >= 10) {
        qEdgeUiBuzzer();
        sViewDrawField(field, String(field), 1);
        sViewDrawField(0, "key=" + String(field) + " X=" + String(xt) + " Y=" + String(yt) + " Z=" + String(zt), 0, 10); // full size
      }
      tft.drawLine(0, yt, tft.width(), yt, TFT_WHITE);
      tft.drawLine(xt, 0, xt, tft.height(), TFT_WHITE);
      sViewDrawField(ESC, "ESC");   // draw ESC button
      refresh = false;
    }
  }
}

/*
 * load png file with filename pngfname to screen
 * from source (SD/LFS) at position x,y
 * returns milliseconds needed
 */
uint32_t sViewLoadPNG(bool onSD, int16_t x, int16_t y, String pngfname)
{
  int16_t rc;
  uint32_t dt = 0;

  xpos = x;
  ypos = y;
  SDactive = onSD;
  rc = png.open(pngfname.c_str(), pngOpen, pngClose, pngRead, pngSeek, pngDraw); // Pass support callback function names to library
  if (rc == PNG_SUCCESS) {
    dt = millis();
    if (png.getWidth() > MAX_IMAGE_WDITH) {
      Dln("Image too wide for allocated line buffer size!");
    } else {
      rc = png.decode(NULL, 0);
      png.close();
    }
    dt = millis() - dt;
  }
  return(dt);
}

/**
 *  save screenshot (bmp) to SD-Card
 */
bool sViewScreenshot()
{
  uint8_t fileIndex = 0;
  String fileName;
  File bmpFile;

#define BMP_HEADER_SIZE 14
#define BMP_INFO_SIZE   40

  uint8_t  bmpHeader[BMP_HEADER_SIZE] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
  uint8_t  bmpInfo[BMP_INFO_SIZE]     = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 16, 0 };
  uint32_t fileSize = BMP_HEADER_SIZE + BMP_INFO_SIZE + 2ul * (tft.width() * tft.height());
  uint16_t rgb, x, y;
  uint8_t highWrite, lowWrite;

  // create new filename and open
  do {
    fileIndex++;
    fileName = "sshot" + String(fileIndex) + ".bmp";
  } while (SD.exists(fileName));
  bmpFile = SD.open(fileName, "w");
  if (!bmpFile) return(false);

  // write header
  bmpHeader[2] = (uint8_t) (fileSize);
  bmpHeader[3] = (uint8_t) (fileSize >> 8);
  bmpHeader[4] = (uint8_t) (fileSize >> 16);
  bmpHeader[5] = (uint8_t) (fileSize >> 24);
  bmpFile.write(bmpHeader, sizeof(bmpHeader));
    
  // write info header
  bmpInfo[ 4] = (uint8_t)(tft.width()       );
  bmpInfo[ 5] = (uint8_t)(tft.width()  >>  8);
  bmpInfo[ 6] = (uint8_t)(tft.width()  >> 16);
  bmpInfo[ 7] = (uint8_t)(tft.width()  >> 24);
  bmpInfo[ 8] = (uint8_t)(tft.height()      );
  bmpInfo[ 9] = (uint8_t)(tft.height() >>  8);
  bmpInfo[10] = (uint8_t)(tft.height() >> 16);
  bmpInfo[11] = (uint8_t)(tft.height() >> 24);
  bmpFile.write(bmpInfo, sizeof(bmpInfo));

  // write picture
  for (y = tft.height(); y > 0; y--) {
    for (x = 0; x < tft.width(); x++) {
      rgb = tft.readPixel(x,y-1); // get pixel color in rgb565 format
      // RGB565 to RGB555 conversion... 555 is default for uncompressed BMP
      lowWrite  = (highByte(rgb) << 7) | ((lowByte(rgb) & 0xC0) >> 1) | (lowByte(rgb) & 0x1f);
      highWrite = highByte(rgb) >> 1;
      bmpFile.write(lowWrite);
      bmpFile.write(highWrite);
    }
  }

  // done
  bmpFile.close();
  tft.fillScreen(TFT_BLACK);
  sViewSetFields(1, 8);
  sViewDrawField(4, "screenshot " + fileName + " saved to SD", 0, 1);
  delay(2000);
  return(true);
}


/**
 *  init serial, tft ...
 */
void sViewInit()
{
  uint8_t smooth;
  String result;

  sViewInitSerial(0, SER0BAUD); // init all serial interfaces
  sViewInitSerial(1, SER1BAUD);
  sViewInitSerial(2, SER2BAUD);

  uniqueID = getUniqueID();     // get unique ID
  cryptKey = uniqueID + uniqueID + uniqueID + uniqueID + uniqueID + uniqueID + uniqueID + uniqueID; // expand to local key

  // Initialise pins
  pinMode(TFT_CS_PIN, OUTPUT);          // deselect chip selects
  digitalWrite(TFT_CS_PIN, HIGH);       // if no pull-ups present
  pinMode(TOUCH_CS_PIN, OUTPUT);
  digitalWrite(TOUCH_CS_PIN, HIGH);
  //pinMode(TFT_BACKLITE_PIN, OUTPUT);    // backlite
  
#ifdef DISCRETE_KEY1
  pinMode(DISCRETE_KEY1, INPUT_PULLUP);
  pinMode(DISCRETE_KEY2, INPUT_PULLUP);
  pinMode(DISCRETE_KEY3, INPUT_PULLUP);
  pinMode(DISCRETE_KEY4, INPUT_PULLUP);
#endif

#ifdef SDCARD
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
#endif

  pinMode(PICO_SMPS, OUTPUT);       // SMPS PWM mode
  digitalWrite(PICO_SMPS, HIGH);
  pinMode(PICO_LED, OUTPUT);        // pico LED active
  digitalWrite(PICO_LED, HIGH);

#ifdef QEDGE_BCTRL
  pinMode(QEDGE_BCTRL, OUTPUT);     // qEdge bus control (1)
  digitalWrite(QEDGE_BCTRL, LOW);
#endif

#ifdef I2CBUS
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();
#endif

  sViewInitLFS();                     // Initialise LittleFS
  tft.setRotation(TFT_ROTATION);      // Initialise the TFT
  tft.begin();
  tft.fillScreen(TFT_RED);
  sViewLoadPNG(FROM_LFS, 0, 0, "/qEdgeBootScreen.png");  // display bootscreen
  Dln("Bootscreen visible ?");
  for (smooth = 0; smooth <= TFT_BACKLITE_INIT; ++smooth) { // smooth Backlight on, in percent
    sViewBacklite(smooth);     // Backlight on, in percent
    delay(10);
  }
  //delay(1000); // to get serial infos

#ifdef RGB_LEDS // init RGB LEDs
  LED.begin();
  //sViewRGBbright(RGB_BRIGHT);
  sViewRGBsetRandom();
  Dln("LED init done");
#endif

#ifdef SDCARD
  sViewInitSD();                        // Initialise SD-Card
#endif

  SPI1_CTRL.setRX(SPI1_MISO_PIN);
  SPI1_CTRL.setSCK(SPI1_CLK_PIN);
  SPI1_CTRL.setTX(SPI1_MOSI_PIN);
  if (sViewKey()) {                     // if key pressed on boot
    Dln("force touch re-calibration");  // force recal
    sViewTouchCal(true);
  } else {
    Dln("load touch calibration");      // or read, if not recal
    sViewTouchCal(false);
  }

  sViewBacklite(getConfigParameter("+BACKLITE", false, String(TFT_BACKLITE_INIT)).toInt());   // init backlite
  sViewRGBbright(getConfigParameter("+RGB", false, String(RGB_BRIGHT)).toInt());              // init RGB brightness
  defaultKeySound = getConfigParameter("+SND", false, String(defaultKeySound)).toInt();       // init key sound

  wifiConnect();                        // start WiFi
  ntpConnect();                         // start NTP
  Dln("init SmartView Library done");
}

/**
 *  initialize LittleFS
 */
void sViewInitLFS()
{

  if (!LFS.begin()) {                   // Initialise LittleFS
    LFSpresent = false;
    Dln("LittleFS init failed!");
  } else {
    LFSpresent = true;
    Dln("LittleFS running");
  }
}

/**
 *  initialize SD-Card
 */
bool sViewInitSD()
{

#ifdef SDCARD
  SDFSConfig c2;
  c2.setCSPin(SD_CS_PIN);
  c2.setSPISpeed(SD_SCK_MHZ(20));
  SDFS.setConfig(c2);
  SDFS.end();
  if (!SDFS.begin()) {
    SDpresent = false;
    Dln("SD card failed");
    SDFS.end();
    return(false);
  } else {
    SDpresent = true;
    Dln("SD-Card running");
    return(true);
  }
#else   // no SD interface defined
  SDpresent = false;
  Dln("no SD card interface compiled");
  return(false);
#endif
}

/*
 *  init serial interface #portnr
 *  returns false if not defined
 */
bool sViewInitSerial(uint8_t portnr, uint32_t baudrate)
{

  switch(portnr) {
  case 0:
    if (SERIAL0) SERIAL0.end();         // close to reopen
    SERIAL0.begin(baudrate);
    break;
  case 1:
#ifndef SERIAL1
    D("serial port #1 not defined");
    return(false);      // not defined
#else
    if (SERIAL1) SERIAL1.end();         // close to reopen
#ifdef TXD1_EN_PIN
    pinMode(TXD1_EN_PIN, OUTPUT);     // if UART RS-485
    digitalWrite(TXD1_EN_PIN, LOW);   // set receive mode
#endif
    SERIAL1.setRX(RXD1_PIN);
    SERIAL1.setTX(TXD1_PIN);
    SERIAL1.begin(baudrate);
#endif    
    break;
  case 2:
#ifndef SERIAL2
    D("serial port #2 not defined");
    return(false);      // not defined
#else
    if (SERIAL2) SERIAL2.end();         // close to reopen
#ifdef TXD2_EN_PIN
    pinMode(TXD2_EN_PIN, OUTPUT);     // if UART RS-485
    digitalWrite(TXD2_EN_PIN, LOW);   // set receive mode
#endif
    SERIAL2.setRX(RXD2_PIN);
    SERIAL2.setTX(TXD2_PIN);
    SERIAL2.begin(baudrate);
#endif
    break;
  }
  return(true);
}

/**
 * Calibrate touch and write calibration data to LittleFS on Flash and SD-Card
 */
void sViewTouchCal(bool force)
{
  #define XPOS 20 
  #define YPOS 0
  #define YPOS_MENUETXT 30
  #define YPOS_INFOTXT 100
  #define MENUEFONTSIZE 2
  #define MENUETXT_FONT 1
  #define INFOFONTSIZE 1
  #define INFOTXT_FONT 2
  uint16_t calData[5];
  bool  calDataOK = false;
  File f;

  if (SDpresent && SD.exists(TFT_CAL_FILE)) { // try SD-card
    if (force) {                      // remove cal file anyway
      SD.remove(TFT_CAL_FILE);
    } else {                          // try read cal data
      f = SD.open(TFT_CAL_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14) calDataOK = true;
        f.close();
      }
    }
  }
  if (LFSpresent && LittleFS.exists(TFT_CAL_FILE)) {  // try LittleFS, overwrite SD
    if (force) {                      // remove cal file anyway
      LittleFS.remove(TFT_CAL_FILE);
    } else {                          // try read cal data
      f = LittleFS.open(TFT_CAL_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14) calDataOK = true;
        f.close();
      }
    }
  }
  if (calDataOK) {
    tft.setTouch(calData);   // calibration data valid
    Dln("Touch Calibration set");
    return;
  }
  tft.fillScreen(TFT_BLACK);  // data not valid so recalibrate
  tft.setCursor(XPOS, YPOS + YPOS_MENUETXT, MENUETXT_FONT);
  tft.setTextFont(1);
  tft.setTextSize(MENUEFONTSIZE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Touch calibration menue");
  tft.setCursor(XPOS, YPOS + YPOS_INFOTXT, INFOTXT_FONT);
  tft.setTextSize(INFOFONTSIZE);
  tft.println("  Touch corners as indicated");
  tft.println();
  tft.calibrateTouch(calData, TFT_WHITE, TFT_BLACK, 15);
  tft.setTouch(calData);   // calibration data valid
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, YPOS, INFOTXT_FONT);
  if (SDpresent) {
    f = SD.open(TFT_CAL_FILE, "w");         // backup data on SD
    if (f){
      tft.println("  Saving touch calibration data on SD-Card");
      f.write((const unsigned char *)calData, 14);
      tft.println("  Data successfully saved on SD-Card\n");
      f.close();
      }
    else {
      tft.println("  Error while opening SD-Card!");
         }
    }
  if (LFSpresent) {
    f = LittleFS.open(TFT_CAL_FILE, "w");   // store data on LittleFS
    if (f) {
      tft.println("  Saving touch calibration data on FLASH");
      f.write((const unsigned char *)calData, 14);
      tft.println("  Data successfully saved on FLASH\n");
      f.close();
           }
    else {
      tft.println("  Error while opening LittleFS!");
         }
  }
}

/**
 * set brightness of all RGB LEDs to 0..255
 */
void sViewRGBbright(uint8_t bright)
{
#ifdef RGB_LEDS
  LED.setBrightness(bright);
  LED.show();
#endif
}

/**
 * set RGB LED ledno (0..n) to color (0..255)
 */
void sViewRGBset(uint8_t ledno, uint8_t color)
{
#ifdef RGB_LEDS
  // from Adafruit colorwheel, extended
  color = 255 - color;
  if (color == RGB_WHITE) {   // white
    LED.setPixelColor(ledno, LED.Color(255, 255, 255));
    LED.show();
    return;
  }
  if (color == RGB_BLACK) {   // black
    LED.setPixelColor(ledno, LED.Color(0, 0, 0));
    LED.show();
    return;
  }
  if (color < 85) {
    LED.setPixelColor(ledno, LED.Color(255 - color * 3, 0, color * 3));
    LED.show();
    return;
  }
  if (color < 170) {
    color -= 85;
    LED.setPixelColor(ledno, LED.Color(0, color * 3, 255 - color * 3));
    LED.show();
    return;
  }
  color -= 170;
  LED.setPixelColor(ledno, LED.Color(color * 3, 255 - color * 3, 0));
  LED.show();
#endif
}

/**
 * set all RGBs to random color
 */
void sViewRGBsetRandom()
{
#ifdef RGB_LEDS
  uint8_t ledno;

  for (ledno = 0; ledno < RGB_LED_NO; ledno++) {
    sViewRGBset(ledno, random(255));
  }
#endif
}

/**
 * set backlight brightness MIN-255
 */
void sViewBacklite(uint8_t value)
{
  if (value < TFT_BACKLITE_MIN) value = TFT_BACKLITE_MIN; // avoid invisible screen
  analogWrite(TFT_BACKLITE_PIN, value);
}

/**
 * get a physical key 0=no, 1-n keys
 */
uint8_t sViewKey() 
{
#ifdef ADC_KEY // ADC keys present
  uint8_t i;
  uint16_t adcval = 0;

  for (i = 0; i < 8; ++i) {
    adcval += analogRead(ADC_KEY);
  }
  adcval = adcval / 8;
  if (adcval > ADC_KEYVAL0) return(0);  // no key
  D("ADC-Key: ");
  Dln(adcval);  // something valid
  if (adcval > ADC_KEYVAL4) return(PHYSICAL_KEY3);
  if (adcval > ADC_KEYVAL3) return(PHYSICAL_KEY2);
  if (adcval > ADC_KEYVAL2) return(PHYSICAL_KEY1);
  return(PHYSICAL_KEY4);
#endif
#ifdef DISCRETE_KEY1 // discrete keys present
  if (digitalRead(DISCRETE_KEY1) == LOW) return(PHYSICAL_KEY1);
  if (digitalRead(DISCRETE_KEY2) == LOW) return(PHYSICAL_KEY2);
  if (digitalRead(DISCRETE_KEY3) == LOW) return(PHYSICAL_KEY3);
  if (digitalRead(DISCRETE_KEY4) == LOW) return(PHYSICAL_KEY4);
  return(0);
#endif
  return(0);  // no ADC keys present
}


/**
 * get key, wait until released
 */
uint8_t sViewGetKey()
{
  uint8_t key = sViewKey();
  
  if (!key) return(0);
  while (sViewKey()) {};  // wait key released
  return(key);
}

/*
 * simple Buzzer
 */
void qEdgeUiBuzzer(uint8_t stype)
{
#ifdef BUZZER
  uint16_t i;
  uint32_t f, f1 = 19, f2 = 31;

  if (stype == 255) { stype = defaultKeySound; } // switch to default
  switch (stype) {
  case NO_SOUND: // no sound
    break;
  case BEEP: // piep
    tone(BUZZER_PIN,2080,15);
    delay(15);
    noTone(BUZZER_PIN);
    break;
  case TRIAD: // rising
    tone(BUZZER_PIN,NOTE_G4,35);
    delay(35);
    tone(BUZZER_PIN,NOTE_G5,35);
    delay(35);
    tone(BUZZER_PIN,NOTE_G6,35);
    delay(35);
    noTone(BUZZER_PIN);
    break;
  case SHOOT: // shoot
    for (i=800; i>300; i=10*i/9) {
      tone(BUZZER_PIN, i, 1);
      delay(1);
    }
    noTone(BUZZER_PIN);
    break;
  case ALARM: // tatü
    tone(BUZZER_PIN,NOTE_B5,100);
    delay(100);
    tone(BUZZER_PIN,NOTE_E6,850);
    delay(800);
    noTone(BUZZER_PIN);
    break;
  case MELODY: // melody
    tone(BUZZER_PIN,NOTE_E6,125);
    delay(130);
    tone(BUZZER_PIN,NOTE_G6,125);
    delay(130);
    tone(BUZZER_PIN,NOTE_E7,125);
    delay(130);
    tone(BUZZER_PIN,NOTE_C7,125);
    delay(130);
    tone(BUZZER_PIN,NOTE_D7,125);
    delay(130);
    tone(BUZZER_PIN,NOTE_G7,125);
    delay(125);
    noTone(BUZZER_PIN);
    break;
  case SPACE_GUN: // space gun
    for(i = 1; i <300; i++){
      digitalWrite(BUZZER_PIN, HIGH);
      delayMicroseconds(i);
      digitalWrite(BUZZER_PIN, LOW);
      delayMicroseconds(i);
    }
    break;  
  case FIBONACCI: // fibonacci
    for(i = 1; i < 20; i++){
      f  = f1 + f2;
      f1 = f2;
      f2 = f;
      tone(BUZZER_PIN, f);
      delay(30);
    }
    noTone(BUZZER_PIN);
    break;      
  case NOISE: // random noise
    for(i = 1; i < 300; i++){
      digitalWrite(BUZZER_PIN, HIGH);
      delayMicroseconds(random(50, 1000));
      digitalWrite(BUZZER_PIN, LOW);
      delayMicroseconds(random(50, 1000));
    }
    break;   
  case CLICK: // click
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(5000); //250
    digitalWrite(BUZZER_PIN, LOW);
    break; 
  case NOISE_FALLING: // random noise falling amplitude
    for(i = 1; i < 100; i++) {
      f = random(50, 1000);
      digitalWrite(BUZZER_PIN, HIGH);
      delayMicroseconds(f);
      digitalWrite(BUZZER_PIN, LOW);
      delayMicroseconds(f * 100 / (101 - i));
    }
    break;   
  case NOISE_RISING: // random noise rising amplitude
    for(i = 1; i < 100; i++) {
      f = random(50, 1000);
      digitalWrite(BUZZER_PIN, HIGH);
      delayMicroseconds(f);
      digitalWrite(BUZZER_PIN, LOW);
      delayMicroseconds(f * 100 / i);
    }
    break;     
    
  default: // nosound
    break;
  }
#endif
}

/*
 *  encrypt string from source string
 */
String simpleEncrypt(String source)
{
  String result = "";
  String hexChar = "0123456789ABCDEF";
  uint8_t crypt, loNibble, hiNibble;

  for (int i = 0; i < source.length(); i++) {
    crypt = (uint8_t) source[i] ^ (uint8_t) cryptKey[i];
    loNibble = crypt % 16;
    crypt = crypt - loNibble;
    hiNibble = crypt / 16;
    result = result + hexChar[hiNibble] + hexChar[loNibble];
  }
  return(result);
}

/*
 *  decrypt string from crypto string
 */
String simpleDecrypt(String crypto)
{
  String result = "";
  String hexChar = "0123456789ABCDEF";
  uint8_t decrypt, loNibble, hiNibble;

  for (int i = 0; i < crypto.length(); i = i + 2) {
    hiNibble = hexChar.indexOf(crypto[i]);
    loNibble = hexChar.indexOf(crypto[i + 1]);
    decrypt = (16 * hiNibble + loNibble) ^ (uint8_t) cryptKey[i / 2];
    result = result + (char) decrypt;
  }
  return(result);
}

/*
 *  set configuration parameter to SD-file
 */

void setConfigParameter(String parameter, String value, bool encrypted)
{
  File sourceFile, destFile;
  String lineWrite, lineRead;
  bool done = false;

  if (encrypted) {  // prepare line to write
    lineWrite = parameter + ":" + simpleEncrypt(value);
  } else {
    lineWrite = parameter + ":" + value;
  }
  if (LittleFS.exists(CONFIG_FILE_NAME)) {
    sourceFile = LittleFS.open(CONFIG_FILE_NAME, "r");
    if (!sourceFile) {
      Dln("SD ERR open source file");
      return;
    }
    destFile   = LittleFS.open(CONFIG_FILE_TEMP, "w");
    if (!sourceFile) {
      Dln("SD ERR open temp file");
      return;
    }
    while (sourceFile.available()) {
      lineRead = sourceFile.readStringUntil('\n');
      lineRead.trim();
      if (lineRead.indexOf(parameter) == -1) { // other parameter pass through
        destFile.println(lineRead);
        D("passthrough - ");
        Dln(lineRead);
      } else {
        destFile.println(lineWrite);          // overwrite
        D("overwrite - ");
        Dln(lineWrite);
        done = true;
      }
    }
    if (!done) {
      destFile.println(lineWrite);          // new entry
      Dln("new entry - ");
      Dln(lineWrite);
    }
    sourceFile.close();
    destFile.close();
    LittleFS.remove(CONFIG_FILE_NAME);
    LittleFS.rename(CONFIG_FILE_TEMP, CONFIG_FILE_NAME);

  } else { // new file, only one entry
    Dln("SD new File");
    destFile = LittleFS.open(CONFIG_FILE_NAME, "w");
    destFile.println(lineWrite);
    destFile.close();
  }
  Dln("");
}

/*
 *  get configuration parameter from SD-file
 */

String getConfigParameter(String parameter, bool encrypted, String preset)
{
  File sourceFile;
  String lineRead, value;
  uint16_t hit;

  if (LittleFS.exists(CONFIG_FILE_NAME)) {
    sourceFile = LittleFS.open(CONFIG_FILE_NAME, "r");
    if (!sourceFile) {
      Dln("SD ERR open source file");
      return("");
    }
    while (sourceFile.available()) {
      lineRead = sourceFile.readStringUntil('\n');
      lineRead.trim();
      if ( lineRead.indexOf(parameter) != -1) { // found parameter
        hit = lineRead.indexOf(":") + 1;        // get value position
        value = lineRead.substring(hit);
        D("found - ");
        Dln(lineRead);
        if (encrypted) {
          sourceFile.close();
          return(simpleDecrypt(value));
        } else {
          sourceFile.close();
          return(value);
        }
      }
    }
    sourceFile.close();
    Dln("nothing found");
  }
  return(preset);
}

/*
 *  scan for WIFI networks
 */
wifiTable wifiScan()
{
  int rssi;
  int8_t wifiNr, wifiMax;
  String ssidStr, passwd;
  wifiTable scanresult;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifiMax = WiFi.scanNetworks();
  scanresult.count = 0;
  for (rssi = 0; rssi > -90; rssi--) {
    for (wifiNr = 0; wifiNr < wifiMax; ++wifiNr) {
      if (WiFi.RSSI(wifiNr) == rssi) {
        scanresult.entry[scanresult.count].ssid = WiFi.SSID(wifiNr);
        scanresult.entry[scanresult.count].rssi = WiFi.RSSI(wifiNr);
        scanresult.entry[scanresult.count].encryption = WiFi.encryptionType(wifiNr);
        scanresult.count++;
      }
    }
  }
  return(scanresult);
}

/*
 *  connect to NTP server
 */
bool ntpConnect()
{
  uint32_t stime;
  time_t now;
  String timezone = "CET-1CEST,M3.5.0,M10.5.0/3";  // Europe/Berlin
  struct tm actual;
  uint8_t maxTries = 20;

  while (true) {
    // try to set ntp time
    configTime(0, 0, "pool.ntp.org", "pool.ntp.org"); // get NTP time
    setenv("TZ", timezone.c_str(), 1);  // set local timezone
    tzset();
    localtime(&now);
    localtime_r(&now,&actual);
    if (actual.tm_year != 70) return(true);
    delay(500);
    --maxTries;
    if (maxTries == 0) return(false);
  }
  return(false);
}

/*
 *  connect Wifi with dedicated SSID, or to the strongest valid
 *  and sync time to NTP server
 */
bool wifiConnect(String ssidStr)
{
  String password;
  int8_t wifiNr, wifiMax;
  wifiTable scanresult;
  uint32_t stime;

  if (ssidStr != "") { // dedicated SSID
    password = getConfigParameter(ssidStr, true);   // get key
    if (password == "") return(false);
  } else {                                          // strongest valid
    scanresult = wifiScan();
    if (scanresult.count == 0) return(false);
    for (wifiNr = 0; wifiNr < scanresult.count; ++wifiNr) {
      ssidStr = scanresult.entry[wifiNr].ssid;
      password = getConfigParameter(ssidStr, true); // get key
      if (password != "") break;                    // found
    }
  }
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(ssidStr.c_str(), password.c_str());
  
  stime = millis();
  while (true) {
    if ((millis() - stime) > 30000) return(false);  // timeout
    switch (WiFi.status()) {
      case WL_NO_SHIELD:        // errors
      case WL_NO_SSID_AVAIL:
      case WL_CONNECT_FAILED:
        return(false);
      case WL_CONNECTION_LOST:  
      case WL_DISCONNECTED:  
      case WL_IDLE_STATUS:      // running
        break;
      case WL_CONNECTED:        // done, OK
        return(true);
        break;
    }
    delay(500);
  }
  return(false);
}

/*
 *  get unique ID
 */
String getUniqueID()
{
  uint8_t flash_id[8], i, loNibble, hiNibble;
  String hexChar = "0123456789ABCDEF";
  String uID;

  uID = "";
  flash_get_unique_id(flash_id);  // get from flash
  for (i = 0; i < 8; i++) {
    loNibble = flash_id[i] % 16;
    flash_id[i] = flash_id[i] - loNibble;
    hiNibble = flash_id[i] / 16;
    uID = uID + hexChar[hiNibble] + hexChar[loNibble];
  }
  return(uID);
}


/**
 *  callback functions for decPNG library
 *  see also: https://github.com/bitbank2/PNGdec
 */

void * pngOpen(const char *filename, int32_t *size)
{
  if (SDactive) {
    pngfile = SD.open(filename, "r");
  } else {
    pngfile = LFS.open(filename, "r");
  }
  *size = pngfile.size();
  return &pngfile;
}

void pngClose(void *handle)
{
  File pngfile = *((File*)handle);
  if (pngfile) pngfile.close();
}

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length)
{
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *page, int32_t position)
{
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.seek(position);
}

void pngDraw(PNGDRAW *pDraw)
{
  uint16_t lineBuffer[MAX_IMAGE_WDITH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  if (png.hasAlpha()) {
    tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer, png.getTransparentColor());
  } else {
    tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
  }
}
