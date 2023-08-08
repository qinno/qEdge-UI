/**
  Hardware Test Application for qEdgeUI - functions

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
#include "HwTestApp.h"

/**
 * main menu from PNG
 */
void HwTestAppMenue()
{
  const uint8_t XF  = 4;
  const uint8_t YF  = 3;
  int16_t field, i;     // touch key fields
  int8_t key;
  uint8_t rgbc = 1;     // RGB led color
  bool refresh = true;
  uint32_t rgbTime;     // rgb change timer
  uint32_t idleTime;    // idle timer

  // menu touch keys
  const uint8_t TTEST       = 0;   // Touch test
  const uint8_t SLIDESHOW   = 1;   // slideshow
  const uint8_t UART        = 2;
  const uint8_t RS485       = 3;
  const uint8_t SDSHOW      = 4;   // SD
  const uint8_t LFSSHOW     = 5;   // LittleFS
  const uint8_t RTC         = 6;
  const uint8_t WIFI        = 7;
  const uint8_t TEMPARATURE = 8;
  const uint8_t INFO        = 9;
  const uint8_t KEYBOARD   = 10;
  const uint8_t SETTING    = 11;

  rgbTime = millis();
  idleTime = millis();

  while (true) {
    if (refresh) {
      sViewLoadPNG(FROM_LFS, 0, 0, "/HwTestAppMenueScreen.png"); //mmenu.png
      sViewSetFields(XF, YF);
      refresh = false;
    }
    field = HwTestAppEvent();
    switch(field) {
    case TTEST:
      qEdgeUiTouchTest();
      refresh = true;
      break;
    case SLIDESHOW:
      HwTestAppSlideshow(FROM_SD);
      refresh = true;
      break;
    case UART:
      HwTestAppTerminal(1);
      refresh = true;
      break;
    case RS485:
      HwTestAppTerminal(2);
      refresh = true;
      break;
    case SDSHOW:
      HwTestAppFSinfo(FROM_SD);
      refresh = true;
      break;
    case LFSSHOW:
      HwTestAppFSinfo(FROM_LFS);
      refresh = true;
      break;
    case RTC:
      HwTestAppLoRa();
      refresh = true;
      break;
    case WIFI:
      HwTestAppWiFiScan();
      refresh = true;
      break;
    case TEMPARATURE:
      gameSetup();
      gameLoop();
      //HwTestAppTemperature();
      refresh = true;
      break;
    case INFO:
      HwTestAppSysInfo();
      refresh = true;
      break;
    case KEYBOARD:
      HwTestAppKeyboard("qinno ");
      refresh = true;
      break;
    case SETTING:
      HwTestAppSettings();
      refresh = true;
      break;
    // keys
    case HOME:
      HwTestAppIdle();
      refresh = true;
      break;
    case SPECIFIC:
      HwTestAppSettings();
      refresh = true;
      break;
    case SCREENSHOT:
      sViewSetFields(XF, YF);
      refresh = true;
      break;      
    }
    if (millis() - rgbTime > RGB_UPDATE_MS) { // rotate RGBs
      for (i = 0; i < RGB_LED_NO; ++i) sViewRGBset(i, rgbc);
      if (++rgbc == 255) rgbc = 1; // avoid black and white
      rgbTime = millis(); }
    if (refresh) idleTime = millis();
    if (millis() - idleTime > IDLE_TIME_MS) { // switch to idle screen
      HwTestAppIdle();
      idleTime = millis();
      refresh = true;
    }
#ifdef PICO_LED
      digitalWrite(PICO_LED, !digitalRead(PICO_LED)); // toggle onboard LED
#endif
  }
}

/**
 * Application which runs in "idle" mode and displays qEdge logo, time and date information on the display
 */
void HwTestAppIdle()
{
  const uint8_t XF  = 1;
  const uint8_t YF  = 10;
  const uint16_t UPDATE_TIME_MS = 997;    // update status display
  const uint16_t RESYNC_TIME_MS = 60000;  // resync to WiFi after lost
  const uint8_t  FONT_7SEG      = 7;
  const uint8_t  FONT_LARGE     = 4;
  const uint8_t  FONT_MEDIUM    = 2;
  time_t now;
  char buff[80];
  String wifiInfo;
  uint32_t updateTime;    // update refresh timer
  uint32_t resyncTime;    // resync time to WiFi
  uint8_t i, rgbc, wifiStat;
  const String dayOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  tm * timeinfo;

  sViewLoadPNG(FROM_LFS, 0, 0, "/qEdgeIdleScreen.png");
  updateTime = millis();
  resyncTime = millis();
  sViewSetFields(XF, YF);
  
  while (HwTestAppEvent() == NO_EVENT) {
    if (millis() - updateTime > UPDATE_TIME_MS) { // update screen
      time(&now);
      timeinfo = localtime(&now);
      uint8_t wday = timeinfo->tm_wday;
      strftime(buff, sizeof(buff), "%H:%M:%S", localtime(&now));  // show time
      sViewDrawField(0, buff, FIELD_TRANS_7SEG, 4);
      strftime(buff, sizeof(buff), "%d.%m.%Y", localtime(&now));  // show date
      sViewDrawField(4,dayOfWeek[wday] + " " + buff, FIELD_TRANS_BIG, 2);
      wifiStat = WiFi.status();
      switch (wifiStat) {
        case WL_NO_SHIELD:
          wifiInfo = "      no WIFI hardware      ";
          break;
        case WL_NO_SSID_AVAIL:
          wifiInfo = "  no WIFI SSIDs available   ";
          break;
        case WL_CONNECT_FAILED:
          wifiInfo = "    WIFI connect failed     ";
          break;
        case WL_CONNECTION_LOST:  
          wifiInfo = "    WIFI connection lost    ";
          break;
        case WL_DISCONNECTED:  
          wifiInfo = "     WIFI disconnected      ";
          break;
        case WL_IDLE_STATUS:
          wifiInfo = "         WIFI idle          ";
          break;
        case WL_CONNECTED:        // done, OK
          wifiInfo = WiFi.SSID() + ": " + String(WiFi.RSSI()) + "dBm";
          break;
      }
      if ( (wifiStat != WL_CONNECTED) and ((millis() - resyncTime > RESYNC_TIME_MS)) ) { // resync WiFi
        wifiInfo = "try to reconnect to WiFi ...";

        //tft.drawString(wifiInfo, tft.width() / 2, 160, FONT_MEDIUM);
        //sViewDrawField(6, wifiInfo, FIELD_DEFAULT, 2);
        sViewDrawField(6, wifiInfo, FIELD_TRANS, 2);
        
        wifiConnect();
        ntpConnect();
        resyncTime = millis();
      } 

      else sViewDrawField(6, wifiInfo, FIELD_TRANS, 2);
      //tft.drawString(wifiInfo, tft.width() / 2, 160, FONT_MEDIUM);

      for (i = 0; i < RGB_LED_NO; ++i) sViewRGBset(i, rgbc);      // rotate RGB
      if (++rgbc == 255) rgbc = 1; // avoid black and white
      updateTime = millis();
    }
  }
  //tft.setTextPadding(0);
}

/**
 * System Info
 */
void HwTestAppSysInfo()
{
  const uint8_t XF  = 4;
  const uint8_t YF  = 6;
  time_t now;
  char buff[80];
  uint8_t I2Caddr;
  const uint8_t ESC = 23;
  String displayString;
  uint16_t LFSused, LFStotal;
  uint8_t i;
  FSInfo64 LFSinfo;
  //FSInfo64 SDinfo;

  sViewSetFields(XF, YF);
  tft.fillScreen(TFT_BLACK);
  sViewDrawField(ESC, "ESC");
  displayString = String(PF_NAME) + " - " + String(rp2040.f_cpu() / 1000000) + " MHz";
  sViewDrawField(0, displayString, FIELD_DEFAULT, 4);
  displayString = "RAM free " + String(rp2040.getFreeHeap() / 1024) + " kB - CPU " + String(analogReadTemp()) + " C";
  sViewDrawField(XF, displayString, FIELD_DEFAULT, 4);
  displayString = "SSID: " + String(WiFi.SSID()) + " - IP: " + String(WiFi.localIP()[0]) + ".";
  displayString = displayString + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
  sViewDrawField(2 * XF, displayString, FIELD_DEFAULT, 4);
  displayString = "MAC: " + String(WiFi.macAddress());
  displayString.toUpperCase();
  sViewDrawField(3 * XF, displayString, FIELD_DEFAULT, 4);
  displayString = "UniqueID: " + uniqueID;
  sViewDrawField(4 * XF, displayString, FIELD_DEFAULT, 4);
  displayString = "";
  //if (SDFS.info64(SDinfo)) { // fails
  //  displayString = "SD OK ";
  //} else {
  //  displayString = "No SD ";;
  //}
  if (LittleFS.info64(LFSinfo)) {
    LFSused = LFSinfo.usedBytes / 1024L;
    LFStotal = LFSinfo.totalBytes / 1024L;
    displayString = displayString + " LFS: " + String(LFSused) + "/" + String(LFStotal) + " kB";
  } else {
    displayString = displayString + " No LFS";
  }
  sViewDrawField(20, displayString, FIELD_DEFAULT, 3);
  while (HwTestAppEvent() == NO_EVENT) {}; // wait touched or key
}

/**
 * show Filesystem Info on SD or LittleFS, no directories
 */
void HwTestAppFSinfo(bool onSD)
{
  const uint8_t MAXDISPLAY = 12; // max files to display
  const uint8_t COLS = 4;
  const uint8_t FIELDS = (MAXDISPLAY * 2) + COLS;
  const uint8_t ROWS = FIELDS / COLS;
  const uint8_t ESC  = FIELDS - 1;
  const uint8_t INFO = ESC - 1;
  const uint8_t FWD  = INFO - 1;
  const uint8_t REW  = FWD - 1;
  const uint8_t NOTOUCH = -1;
  bool refresh = true;
  bool modify = false;
  bool noSD = false;
  String fileNames[64];         // max. 64 files overall
  uint8_t fileCounter = 0;
  uint8_t offset = 0;
  int16_t i, field;
  String fileSelected, fileNew;
  File file, SDfile, source, target;

  sViewSetFields(4, (ESC + 1) / 4);       // display menu
  tft.fillScreen(TFT_BLACK);
  sViewLoadPNG(false, 0, 0, "qEdgeWallpaperScreen.png");
  sViewDrawField(ESC, "WAIT");
  if (onSD) sViewDrawField(INFO, "SD-Card");
  else sViewDrawField(INFO, "LittleFS");

  SDfile = SD.open("/", "r");       // check SD-Card present
  file = SDfile.openNextFile();
  if (!file) {
    Dln("no SD-Card file, try to re-init");
    if (!sViewInitSD()) {
      noSD = true;
      if (onSD) {
        sViewDrawField(0, "SD-card not present", FIELD_ERROR, COLS);
        delay(3000);
        return;
      }
    }
  }
  if (onSD) source = SD.open("/", "r");
  else source = LittleFS.open("/", "r");
  while (file = source.openNextFile()) {  // collect all filenames
    if (!file.isDirectory()) {
      fileNames[fileCounter] = file.name();
      ++fileCounter;
    }
  }

  while (true) {
    if (refresh) {
      tft.fillScreen(TFT_BLACK);
      sViewLoadPNG(false, 0, 0, "qEdgeWallpaperScreen.png");
      sViewSetFields(4, (ESC + 1) / 4);
      sViewDrawField(ESC, "ESC");
      if (modify) {
        sViewDrawField(0, fileSelected, FIELD_REVERSE, 2);
        if (onSD) {
          sViewDrawField(INFO, "copy->LFS");
          source = SD.open(fileSelected, "r");
        } else {
          sViewDrawField(INFO, "copy->SD");
          source = LittleFS.open(fileSelected, "r");
        }
        sViewDrawField(2, String(source.size()) + " Byte", FIELD_REVERSE, 2);
        source.close();
        sViewDrawField(FWD, "Rename");
        sViewDrawField(REW, "Delete");
      } else {
        if (onSD) sViewDrawField(INFO, "SD-Card");
        else sViewDrawField(INFO, "LittleFS");
        if ((offset + MAXDISPLAY) < fileCounter) sViewDrawField(FWD, ">>");
        else sViewDrawField(FWD, "");
        if (offset > 0) sViewDrawField(REW, "<<");
        else sViewDrawField(REW, "");
        for (i = 0; i < MAXDISPLAY; i++) {
          if ((i + offset) < fileCounter) sViewDrawField(i * 2, fileNames[i + offset], FIELD_DEFAULT, 2);
        }
      }
      refresh = false;
    }
    field = HwTestAppEvent();
    switch(field) {
    case NO_EVENT:
      break;
    case ESC:
      if (modify) {
        modify = false;
        refresh = true;
      } else return;
      break;
    case INFO:      // info + copy
      if (modify) {
        if (onSD) {
          source = SD.open(fileSelected, "r");
          target = LittleFS.open(fileSelected, "w");
        } else {
          source = LittleFS.open(fileSelected, "r");
          target = SD.open(fileSelected, "w");
        }
        while (source.available()) {
          target.write(source.read());
        }
        source.close();
        target.close();
        sViewDrawField(0, "copied: " + fileSelected, FIELD_REVERSE, 4);
        delay(2000);
      }
      break;
    case FWD:       // forward + rename
      if (modify) {
        fileNew = HwTestAppKeyboard("");
        sViewSetFields(4, (ESC + 1) / 4);
        if (fileNew != "") {
          if (onSD) SD.rename(fileSelected, fileNew);
          else LittleFS.rename(fileSelected, fileNew);
          sViewDrawField(0, "renamed: " + fileSelected + " to: " + fileNew, FIELD_REVERSE, 4);
          delay(2000);
          return;
        }
      } else {
        if ((offset + MAXDISPLAY) < fileCounter) offset = offset + MAXDISPLAY; // new page ?
      }
      refresh = true;
      break;
    case REW:       // rewind + delete
      if (modify) {
        sViewDrawField(COLS, "confirm deletion " + fileSelected + " any other to break", FIELD_REVERSE, COLS);
        delay(200);
        field = NOTOUCH;
        while (field == NOTOUCH) { // wait touched
          field = sViewGetTouchField();
        }
        if ((field >= COLS) and (field < (2 * COLS))) {
          if (onSD) SD.remove(fileSelected);
          else LittleFS.remove(fileSelected);
          sViewDrawField(COLS, fileSelected + " deleted ...", FIELD_REVERSE, COLS);
          delay(2000);
          return;
        } else {
          sViewDrawField(COLS, fileSelected + " deletion aborted ...", FIELD_ERROR, COLS);
          delay(2000);
        }
      } else {
        if (offset != 0) offset = 0;
      }
      refresh = true;
      break;
    // keys
    case HOME:
      return;
    case SPECIFIC:
    case SCREENSHOT:
      refresh = true;
      break;
    default:
      Dln(fileNames[offset + field / 2]);
      if ((offset + field / 2) < fileCounter) {
        fileSelected = fileNames[offset + field / 2];
        modify = true;
        refresh = true;
      }
      break;
    }
  }
}

/**
 *  keyboard starts with InitStr to edit
 *  returns edited string, password hides input
 */
String HwTestAppKeyboard(String InitStr, bool password)
{
  const uint8_t XF  = 10;               // X*Y raster
  const uint8_t YF  = 7;
  const uint8_t RET1 =  (XF * YF) - 2;  // return
  const uint8_t RET2 =  RET1 + 1;
  const uint8_t BS1 =  RET1 - XF;       //backspace
  const uint8_t BS2 =  BS1 + 1;
  const uint8_t ABC1 =  XF * (YF - 1);  // set changer
  const uint8_t ABC2 =  ABC1 + 1;
  const uint8_t SPC1 =  RET1 - 6;       // space key
  const uint8_t KEY_COL = 2;            // key color
  const uint8_t KEY_COLR = 3;           // and reverse for flash
  const uint8_t FN_COL = 0;             // function key color
  const uint8_t FN_COLR = 1;            // and reverse for flash
  int16_t field, key, i;

  String set1 = "!\"+-./()=?0123456789qwertzuiopasdfghjkl#<yxcvbnm            ";
  String set2 = "{[]}\\+-_~*0123456789QWERTZUIOPASDFGHJKL'>YXCVBNM            ";
  String set3 = "{[]}\\+-*/~0123456789.,:;-_#'<>|?=)(/&%$\"!°²³                 ";
  String aset = set1; // active character set
  String select = "ABC";
  String passStr;
  int8_t cset = 1;
  const int8_t csetmax = 3;
  bool redraw = true;
  bool showStr = true;
  bool flash = false;

  sViewSetFields(XF, YF);
  tft.fillScreen(TFT_BG_COL);

  while (true) {
    if (flash) {
      switch (field) {
      case BS1:
        sViewDrawField(BS1, "BACK", FIELD_REVERSE, 2);    // Backspace key
        delay(200);
        sViewDrawField(BS1, "BACK", FIELD_DEFAULT, 2);
        break;
      case ABC1:
        sViewDrawField(ABC1, select, FIELD_REVERSE, 2);   // set selector key
        delay(200);
        sViewDrawField(ABC1, select, FIELD_DEFAULT, 2);
        break;
      case SPC1:
        sViewDrawField(SPC1, " ", FIELD_REVERSE, 6);      // space key
        delay(200);
        sViewDrawField(SPC1, " ", FIELD_DARK, 6);
        break;
      default:
        sViewDrawField(field, String(aset[field - XF]), FIELD_REVERSE);  // all characters
        delay(200);
        sViewDrawField(field, String(aset[field - XF]), FIELD_DARK);
        break;
      }
      flash = false;
    }
    if (redraw) { // redraw buttons
      switch (cset) {
      case 1:
        aset = set1;
        select = "ABC";
        break;
      case 2:
        aset = set2;
        select = "</>";
        break;
      case 3:
        aset = set3;
        select = "abc";
        break;
      }
      for (int i = XF; i < BS1; ++i) {
        sViewDrawField(i, String(aset[i - XF]), FIELD_DARK);
      }
      sViewDrawField(SPC1, " ", FIELD_DARK, 6);      // space key
      sViewDrawField(RET1, "RETURN", FIELD_DEFAULT, 2); // Return key
      sViewDrawField(BS1, "BACK", FIELD_DEFAULT, 2);    // Backspace key
      sViewDrawField(ABC1, select, FIELD_DEFAULT, 2);    // set selector key
      redraw = false;
    }
    if (showStr) { // show input field
      if (password) {
        passStr = InitStr;
        for (i = 0; i < passStr.length() - 1; ++i) {
          passStr[i] = '*';
        }
        sViewDrawField(0, passStr, FIELD_DEFAULT, XF);
      } else {
        sViewDrawField(0, InitStr, FIELD_DEFAULT, XF);
      }
      showStr = false;
    }
    field = HwTestAppEvent();
    switch(field) {
    case NO_EVENT:
      break;
    case BS1:
    case BS2:
      if (InitStr.length() > 0) InitStr.remove(InitStr.length() - 1, 1);
      field = BS1;
      showStr = true;
      Dln("BS");
      flash = true;
      break;
    case RET1:
    case RET2:
      Dln("RET");
      return(InitStr);
    case ABC1:
    case ABC2:
      if (++cset > csetmax) cset = 1; // next character set
      redraw = true;
      field = ABC1;
      Dln(select);
      flash = true;
      break;
    // keys
    case HOME:
      return(InitStr);
    case SPECIFIC:
    case SCREENSHOT:
      redraw = true;
      break;
    default:
      if (field >= XF) {
        Dln(field - XF);
        InitStr += aset[field - XF];
        showStr = true;
        if (cset == 2) {
          cset = 1;
          redraw = true;
        }
        if (field >= SPC1) field = SPC1;
        flash = true;
        for (i = 0; i < RGB_LED_NO; ++i) sViewRGBset(i, 19 + 4 * (field - XF)); // OK, nice
      }
      break;
    }
  }
}



/**
 * Slideshow to screen
 * from source (SD/LFS)
 */
bool HwTestAppSlideshow(bool onSD)
{
  const uint8_t XF  = 5;
  const uint8_t YF  = 8;
  const uint8_t ESC = XF * YF - 1;
  const uint8_t FWD = ESC - 1;
  const uint8_t PAUSE = ESC - 2;

  File root, file;
  String strname;
  uint32_t dt, stime;
  bool wait;
  int16_t field;

  if (onSD && !SDpresent) { // SD wanted, but not present
    onSD = false;   // get from LFS
  }
  sViewSetFields(XF, YF);
  if (onSD) {
    root = SD.open("/", "r");       // Scan SD-Card
  } else {
    root = LittleFS.open("/", "r"); // Scan LittleFS
  }
  while (file = root.openNextFile()) {
    strname = file.name();
    strname = "/" + strname;
    tft.setTextFont(2);
    tft.setTextColor(TFT_BLUE);  // transparent
    tft.setCursor(5, 240 - 23);
    if (!file.isDirectory() && strname.endsWith(".png")) { // If it is not a directory and filename ends in .png then load it
      dt = sViewLoadPNG(onSD, 0, 0, strname);
      if (onSD) tft.print("SD: ");
      else tft.print("LFS: ");
      tft.print(file.name());
      tft.print(" - ");
      tft.print(dt);
      tft.print(" ms   ");
      wait = true;
    } else {
      if (onSD) tft.print("SD: ");
      else tft.print("LFS: ");
      tft.print(file.name());
      tft.print(" -> no png   ");
      file.close();
      wait = false;
    }
    sViewDrawField(ESC, "ESC");   // draw ESC button
    sViewDrawField(FWD, ">>");    // draw forward button
    sViewDrawField(PAUSE, "||");  // draw pause button
    sViewRGBsetRandom(); // random change
    stime = millis();
    while (wait) {
      if (millis() - stime > 5000) wait = false;  // repeat on time
      field = HwTestAppEvent();
      switch(field) {
      case PAUSE:
        dt = sViewLoadPNG(onSD, 0, 0, strname);
        while (HwTestAppEvent() == NO_EVENT) {};       // wait touched or key
        wait = false;
        break;
      case FWD:
      case SPECIFIC:
      case SCREENSHOT:
        sViewSetFields(XF, YF);
        wait = false;
        break;      
      case HOME:
      case ESC:
        return(false);
      }
    }
  }
  return(true);
}

/**
 * UART Terminal
 */
void HwTestAppTerminal(uint8_t channel)
{ 
  const uint8_t XF  = 5;
  const uint8_t YF  = 6;
  const uint8_t ESC  = XF * YF - 1;
  const uint8_t SEND = ESC - 1;
  const uint8_t BAUD = ESC - 2;
  const uint8_t ASC  = ESC - 3;
  const uint8_t TERM = ESC - 4;
  int16_t field;
  bool refresh = true;
  bool isASC   = true;
  bool isRS485 = false;       // if RS-485 port
  uint8_t RS485txen;  // TX enable pin
  uint32_t baudrate;
  uint32_t baudvalues [] = {2400, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
  const uint8_t maxselect = 9;
  uint8_t baudselect = 5;
  HardwareSerial* SerialPort;

  switch(channel) {
  case 0:
    SerialPort = &SERIAL0;
    break;
  case 1:
#ifdef SERIAL1
    SerialPort = &SERIAL1;
#else
    return;
#endif
#ifdef TXD1_EN_PIN
    isRS485 = true;
    RS485txen = TXD1_EN_PIN;
#endif
    break;
  case 2:
#ifdef SERIAL2
    SerialPort = &SERIAL2;
#else
    return;
#endif    
#ifdef TXD2_EN_PIN
    isRS485 = true;
    RS485txen = TXD2_EN_PIN;
#endif
    break;
  }

  baudrate = baudvalues[baudselect];
  sViewInitSerial(channel, baudrate);
  while (true) {
    if (refresh) {
      sViewSetFields(XF, YF);
      tft.fillScreen(TFT_BG_COL);
      sViewLoadPNG(false, 0, 0, "qEdgeWallpaperScreen.png");
      tft.setCursor(0, 0);
      sViewDrawField(ESC, "ESC");   // draw ESC button
      sViewDrawField(SEND, "Send");    // draw send button
      sViewDrawField(BAUD, String(baudrate));    // draw baudrate button
      if(isASC) sViewDrawField(ASC, "ASCII");
      else sViewDrawField(ASC, "HEX"); // draw ascii button
      if (isRS485) sViewDrawField(TERM, "RS485");
      else sViewDrawField(TERM, "UART");
      sViewRGBsetRandom(); // random change
      tft.setTextColor(TFT_GREEN);
      refresh = false;
    }

    field = HwTestAppEvent();
    switch(field) {
    case SEND:
      tft.setTextColor(TFT_GREEN);
      tft.print(" [send] ");
      if (isRS485) digitalWrite(RS485txen, HIGH); // set transmit mode
      SerialPort->println("TX Paket");
      SerialPort->flush();              // wait transmission done
      if (isRS485) {
        delay(1);                       // to be safe
        digitalWrite(RS485txen, LOW); // set receive mode
      }
      break;
    case SPECIFIC:
    case BAUD:
      if (++baudselect == maxselect) baudselect = 0; // restart from beginning
      baudrate = baudvalues[baudselect];
      sViewInitSerial(channel, baudrate);
      sViewDrawField(BAUD, String(baudrate));    // draw baudrate button
      break;
    case ASC:
      isASC = !isASC;
      refresh = true;
      break;
    case SCREENSHOT:
      refresh = true;
      sViewSetFields(XF, YF);
      break;    
    case HOME:
    case ESC:
      return;
      break;
    }
    if (SerialPort->available()) {
      if (isASC) tft.print((char) SerialPort->read());
      else tft.printf(" %.2X", SerialPort->read());
    }
  }
}

/*
 *  Settings for Backlite, LEDs
 */

void HwTestAppSettings()
{
  int16_t field;
  const uint8_t XF  = 4;
  const uint8_t YF  = 4;
  const uint8_t SND_PLUS   = 0;
  const uint8_t SND_DISP   = SND_PLUS + XF;
  const uint8_t SND_MINUS  = SND_DISP + XF;
  const uint8_t RGB_CPLUS  = 1; // color value
  const uint8_t RGB_CDISP  = RGB_CPLUS + XF;
  const uint8_t RGB_CMINUS = RGB_CDISP + XF;
  const uint8_t RGB_BPLUS  = 2; // brightness value
  const uint8_t RGB_BDISP  = RGB_BPLUS + XF;
  const uint8_t RGB_BMINUS = RGB_BDISP + XF;
  const uint8_t BL_BPLUS  = 3; // backlight value
  const uint8_t BL_BDISP  = BL_BPLUS + XF;
  const uint8_t BL_BMINUS = BL_BDISP + XF;
  
  const uint8_t ESC  = XF * YF - 1;
  const uint8_t SAVE  = ESC - 1;
  const uint8_t TCAL  = SAVE - 1;
  const uint8_t PLAY  = TCAL - 1;
  
  
  uint8_t backlite;
  uint8_t rgbbright;
  uint8_t rgbcolor  = 1;
  //uint8_t keySound = defaultKeySound;
  uint8_t saveKeySound = defaultKeySound;
  
  bool refresh = true;
  String result;
  uint8_t i;

  result = getConfigParameter("+BACKLITE", false);
  if (result != "") backlite = result.toInt();
  else backlite = TFT_BACKLITE_INIT;
  sViewBacklite(backlite);    // update backlite brightness
  
  result = getConfigParameter("+RGB", false);
  if (result != "") rgbbright = result.toInt();
  else rgbbright = RGB_BRIGHT;
  sViewRGBbright(rgbbright);  // update RGB brightness

  while (true) {
    if (refresh) {
      sViewSetFields(XF, YF);
      tft.fillScreen(TFT_BG_COL);
      sViewDrawField(RGB_CPLUS, "COL+");   // draw buttons
      sViewDrawField(RGB_CMINUS, "COL-");
      sViewDrawField(RGB_BPLUS, "RGB+");
      sViewDrawField(RGB_BMINUS, "RGB-");
      sViewDrawField(BL_BPLUS, "BL+");
      sViewDrawField(BL_BMINUS, "BL-");
      sViewDrawField(SND_PLUS, "SOUND+");
      sViewDrawField(SND_MINUS, "SOUND-");
      sViewDrawField(PLAY, "PLAY");
      sViewDrawField(TCAL, "CTOUCH");
      sViewDrawField(SAVE, "SAVE");
      sViewDrawField(ESC, "ESC");
      sViewDrawField(RGB_CDISP, String(rgbcolor), FIELD_REVERSE);    // draw values
      sViewDrawField(RGB_BDISP, String(rgbbright), FIELD_REVERSE);
      sViewDrawField(BL_BDISP, String(backlite), FIELD_REVERSE);
      sViewDrawField(SND_DISP, String(defaultKeySound), FIELD_REVERSE);
      for (i = 0; i < RGB_LED_NO; ++i) sViewRGBset(i, rgbcolor); // update RGB color
      sViewRGBbright(rgbbright);  // update RGB brightness
      sViewBacklite(backlite);    // update backlite brightness
      refresh = false;
    }

    field = HwTestAppEvent();
    switch(field) {
    case SND_PLUS:
      ++defaultKeySound;
      sViewDrawField(SND_DISP, String(defaultKeySound), FIELD_REVERSE);
      break;
    case SND_MINUS:
      --defaultKeySound;
      sViewDrawField(SND_DISP, String(defaultKeySound), FIELD_REVERSE);
      break;      
    case PLAY:
      break;
    case TCAL:
      delay(500);
      sViewTouchCal(true);
      break;
    case RGB_CPLUS:
      ++rgbcolor;
      for (i = 0; i < RGB_LED_NO; ++i) sViewRGBset(i, rgbcolor); // update RGB color
      sViewDrawField(RGB_CDISP, String(rgbcolor), FIELD_REVERSE);
      break;
    case RGB_CMINUS:
      --rgbcolor;
      for (i = 0; i < RGB_LED_NO; ++i) sViewRGBset(i, rgbcolor); // update RGB color
      sViewDrawField(RGB_CDISP, String(rgbcolor), FIELD_REVERSE);
      break;
    case RGB_BPLUS:
      rgbbright += 5;
      sViewRGBbright(rgbbright); // update RGB brightness
      sViewDrawField(RGB_BDISP, String(rgbbright), FIELD_REVERSE);
      sViewDrawField(SAVE, "SAVE");
      break;
    case RGB_BMINUS:
      rgbbright -= 4;
      sViewRGBbright(rgbbright); // update RGB brightness
      sViewDrawField(RGB_BDISP, String(rgbbright), FIELD_REVERSE);
      sViewDrawField(SAVE, "SAVE");
      break;
    case BL_BPLUS:
      backlite += 5;
      sViewBacklite(backlite); // update backlite brightness
      sViewDrawField(BL_BDISP, String(backlite), FIELD_REVERSE);
      sViewDrawField(SAVE, "SAVE");
      break;
    case BL_BMINUS:
      backlite -= 4;
      sViewBacklite(backlite); // update backlite brightness
      sViewDrawField(BL_BDISP, String(backlite), FIELD_REVERSE);
      sViewDrawField(SAVE, "SAVE");
      break;
    case SPECIFIC:
      break;
    case SAVE:
      setConfigParameter("+BACKLITE", String(backlite), false);
      setConfigParameter("+RGB", String(rgbbright), false);
      setConfigParameter("+SND", String(defaultKeySound), false);
      saveKeySound = defaultKeySound;
      sViewDrawField(SAVE, "SAVED");
      break;
    case SCREENSHOT:
      refresh = true;
      break;
    case HOME:
    case ESC:
      defaultKeySound = saveKeySound;
      return;
      break;
    }
  }
}

/*
 *  Temperature HwTestApp from TFT_eSPI example Analogue_meters
 */
void HwTestAppTemperature()
{

  MeterWidget   tmp  = MeterWidget(&tft);
  static uint32_t updateTime = 0;

  tft.fillScreen(TFT_BG_COL);
  tmp.setZones(80, 100, 60, 80, 40, 60, 0, 40); // Example here red starts at 75% and ends at 100% of full scale
  tmp.analogMeter(40, 40, 80.0, "T-CPU", "0", "20", "40", "60", "80");    // Draw analogue meter
  // loop
  while (HwTestAppEvent() == NO_EVENT) { // exit if touched or key
    if (millis() - updateTime >= 300) {
      updateTime = millis();
      tmp.updateNeedle(analogReadTemp(), 0);
    }

  }
}

/**
 *  scan WiFi networks
 */
void HwTestAppWiFiScan()
{

#if defined(PF_PICOW)
  const uint8_t XF  = 2;
  const uint8_t YF  = 8;
  const uint8_t ESC  = XF * YF - 1;  
  int16_t field;
  wifiTable scanresult;
  String ssidStr, passwd;
  bool loop, refresh;

  // scan networks
  tft.fillScreen(TFT_BLACK);
  sViewLoadPNG(false, 0, 0, "qEdgeWallpaperScreen.png");
  sViewSetFields(XF, YF);
  sViewDrawField(0, "scanning networks ...", FIELD_DEFAULT, 2);
  scanresult = wifiScan();
  if (scanresult.count == 0) {
    sViewDrawField(0, "no networks found", FIELD_ERROR, 2);
    delay(3000);
    return;
  }

  // select network
  loop = true;
  refresh = true;
  while (loop) {
    if (refresh) {
      sViewSetFields(XF, YF);
      tft.fillScreen(TFT_BG_COL);
      sViewDrawField(ESC, "ESC");
      for (field = 0; field < scanresult.count; field++) {
        ssidStr = scanresult.entry[field].ssid + String((scanresult.entry[field].encryption == CYW43_AUTH_OPEN) ? "" : "*");
        sViewDrawField(field, ssidStr);   // draw button
      }
      refresh = false;
    }
    field = HwTestAppEvent();
    switch(field) {
    case ESC:
    case HOME:
      return;
    case NO_EVENT:
    case SPECIFIC:
      break;
    case SCREENSHOT:
      refresh = true;
      sViewSetFields(XF, YF);
      break;
    default:
      if (field >= 0 and field < scanresult.count) loop = false;    
    }
  }

  // enter password and save cryted
  ssidStr = scanresult.entry[field].ssid;
  passwd = HwTestAppKeyboard("");
  setConfigParameter(ssidStr, passwd, true); // save credentials
  sViewSetFields(XF, YF);
  tft.fillScreen(TFT_BG_COL);

  // show wifi status
  sViewDrawField(0, "Connecting to " + ssidStr, FIELD_DEFAULT, 2);
  if (wifiConnect(ssidStr)) {
    sViewDrawField(0, ssidStr + " connected", FIELD_DEFAULT, 2);
    //delay(3000);
  } else {
    sViewDrawField(0, "no connection to " + ssidStr, FIELD_DEFAULT, 2);
    //delay(3000);
  }

  // show ntp status
  sViewDrawField(XF, "set time to NTP " + ssidStr, FIELD_DEFAULT, 2);
  if (ntpConnect()) {
    sViewDrawField(XF, "time set to NTP time", FIELD_DEFAULT, 2);
    delay(3000);
  } else {
    sViewDrawField(XF, "NTP not reachable" + ssidStr, FIELD_DEFAULT, 2);
    delay(3000);
  }

  
#endif
}

/**
 * LoRa menu
 */
void HwTestAppLoRa()
{
  const uint8_t XF  = 5;
  const uint8_t YF  = 6;
  const uint8_t ESC  = XF * YF - 1;
  const uint8_t SEND = ESC - 1;
  const uint8_t TERM = ESC - 4;
  int16_t field;  
  bool refresh = true;
  String LoRaString;

  tft.fillScreen(TFT_BLACK);
  sViewLoadPNG(false, 0, 0, "qEdgeWallpaperScreen.png");
  sViewSetFields(XF, YF);
  LoRa.setPins(LORA_CS_PIN, -1, -1);// set CS, reset, IRQ pin
  if (!LoRa.begin(868E6)) {             // initialize ratio at 868 MHz
    sViewDrawField(0, "LoRa init failed. Check your settings.", FIELD_ERROR, XF);
    delay(2000);
    return;
  }
 
  while (true) {
    if (refresh) {
      sViewSetFields(XF, YF);
      tft.fillScreen(TFT_BG_COL);
      sViewLoadPNG(false, 0, 0, "qEdgeWallpaperScreen.png");
      tft.setCursor(0, 0);
      sViewDrawField(ESC, "ESC");   // draw ESC button
      sViewDrawField(SEND, "Send");    // draw send button
      sViewDrawField(TERM, "LoRa");
      sViewRGBsetRandom(); // random change
      tft.setTextColor(TFT_GREEN);
      refresh = false;
    }
    field = HwTestAppEvent();
    switch(field) {
    case SEND:
    case SPECIFIC:
      tft.setTextColor(TFT_GREEN);
      tft.print(" [TX] ");
      LoRa.beginPacket(); // send packet
      LoRaString = HwTestAppKeyboard("qEdgeUI LoRa Testmessage");
      LoRa.print(LoRaString);
      LoRa.endPacket();
      sViewRGBsetRandom(); // random change
      refresh = true;
      break;
    case SCREENSHOT:
      sViewSetFields(XF, YF);
      refresh = true;
      break;
    case HOME:
    case ESC:
      return;
      break;
    }
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) { // received a packet
      tft.print("[RX]");
      // read packet
      while (LoRa.available()) {
        tft.print((char)LoRa.read());
      }
      // print RSSI of packet
      tft.print("-RSSI ");
      tft.println(LoRa.packetRssi());
      sViewRGBsetRandom(); // random change
    }
  }
}

/**
 *   collect keyboard and touch events 
 *   call special functions
 */
int16_t HwTestAppEvent() 
{
  int16_t event;
  
  event = sViewGetTouchField();    // touch key ?
  if (event != NO_EVENT) {
    qEdgeUiBuzzer();
    return(event);
  }
  event = (int16_t) sViewGetKey(); // physical key ?
  if (!event) return(NO_EVENT);
  qEdgeUiBuzzer();
  switch (event) {
    case PHYSICAL_KEY1:
      return(HOME);
      break;
    case PHYSICAL_KEY2:
      return(SPECIFIC);
      break;      
    case PHYSICAL_KEY3:
#ifdef ENABLE_SCREEN_SHOT_FCT
      sViewScreenshot();
      return(SCREENSHOT);
#else
      qEdgeUiBuzzer(SPACE_GUN);
#endif
      break;            
    case PHYSICAL_KEY4:    
      rp2040.reboot();  
  }
  return(0);
}
 


 
