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

#ifndef HWTESTAPP_h
#define HWTESTAPP_h

// external libraries used
#include <time.h>
#include <sys/time.h>
#include <Wire.h>
#include "Game.h"

// key events for HwTestAppEvent
#define KEY_EVENT1  32761
#define KEY_EVENT2  32762
#define KEY_EVENT3  32763
#define KEY_EVENT4  32764

// special keys
#define HOME        KEY_EVENT1
#define SPECIFIC    KEY_EVENT2
#define SCREENSHOT  KEY_EVENT3
//#define REBOOT KEY_EVENT4

/**
 *  functions in this scope
 */

void HwTestAppMenue();
void HwTestAppIdle();
void HwTestAppFSmodify(bool onSD, String filename);
void HwTestAppFSinfo(bool onSD);
String HwTestAppKeyboard(String InitStr, bool password = false);
bool HwTestAppSlideshow(bool onSD);
void HwTestAppSysInfo();
void HwTestAppTerminal(uint8_t channel);
void HwTestAppSettings();
void HwTestAppTemperature();
void HwTestAppWiFiScan();
void HwTestAppLoRa();

int16_t HwTestAppEvent();


#endif
