/**
  Hardware Test Application for qEdgeUI

  Copyright (c) 2022-2025 Ralf Wolters

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

/*
  Settings: PICO W
  --> Tools -> Flash size  -> Sketch: 1MB, FS: 1MB
  --> Tools -> CPU speed   -> 125 MHz
  --> Tools -> Optimize    -> -O3
  --> Tools -> WiFi Region -> - select your country

  Settings: PICO2 W
  --> Tools -> CPU Architecture -> ARM
  --> Tools -> Flash size  -> Sketch: 2MB, FS: 2MB
  --> Tools -> CPU speed   -> 150 MHz
  --> Tools -> Optimize    -> -O3
*/

#include "SmartView.h"
#include "HwTestApp.h"

//Uncomment define for enabling "screenshot functionality" by pressing a button to make and store screenshots to FLASH / SD-card
// #define ENABLE_SCREEN_SHOT_FCT
/**
    init
*/
void setup() {
  sViewInit();
  Dln("Init done");
  qEdgeUiBuzzer();
  Dln("Buzz done");
}

/**
    main loop
*/
void loop() {
  Dln("Starting Hardware Test Application Main Menue");
  HwTestAppMenue();
}
