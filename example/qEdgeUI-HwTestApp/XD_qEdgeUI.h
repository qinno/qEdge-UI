/**
  Extended Hardware platform definition for qEdgeUI

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

#ifndef XD_QEDGEUI_h
#define XD_QEDGEUI_h

/*
 *  board: qEdge-UI with RPi PICO - RP2040 CPU
 */

// to decide board specifica
//#define PF_PICO       // RP204 RPi PICO board
#define PF_PICOW     // RP204 RPi PICO "W" board with WiFi

#ifdef PF_PICOW
#define PF_NAME "qEdge-WiFi" // name plattform
#include <WiFi.h>
#include <LoRa.h>
#else
#define PF_NAME "qEdge"
#endif

/*
 * interfaces/addons comment if not present
 */
#define SDCARD                    // SD-Card interface present
#define SYSSERIAL Serial          // always needed
#define SERIAL0   Serial
#define SERIAL1   Serial1         // additional serial Interfaces present
#define SERIAL2   Serial2


#define RGB_LEDS                  // RGB Leds WS28xx
#define BUZZER                    // Buzzer/Speaker
#define KEYBUTT                   // Hardware button
#define I2CBUS                    // I²C Bus

#define SER0BAUD  115200          // default baudrates
#define SER1BAUD  115200          // 
#define SER2BAUD  115200          // 

#define TFT_ROTATION     3        // 0 or 2 portrait; 1 or 3 landscape

// TFT and touch, always
#define TFT_BACKLITE_PIN   6      // 
#define TFT_CS_PIN        17      // TFT and touch need also definition 
#define TFT_DC_PIN        20      // in library TFT_eSPI/User_Setup.h !!!
#define TOUCH_CS_PIN      15      // 
#define SD_CS_PIN          7      // SD-Card SPI CS
#define SD_CLK_FREQ       12      // SD clock frequency in MHz 
#define SPI_CLK_PIN       18      // 
#define SPI_MOSI_PIN      19      // 
#define SPI_MISO_PIN      16      // 
#define SPI_CTRL          SPI     // controller for TFT, touch, SD

#define LORA_CS_PIN       13      // LoRa SPI CS
#define LORA_CLK_FREQ      8      // LoRa clock frequency in MHz 
#define SPI1_CLK_PIN      10      // 
#define SPI1_MOSI_PIN     11      // 
#define SPI1_MISO_PIN     12      // 
#define SPI1_CTRL         SPI1    // controller for LoRa

#define RGB_LED_PIN        2      // RGB LEDs pin
#define RGB_LED_NO         3      // and number

#define BUZZER_PIN        14      // Buzzer

// additional serials
#define TXD1_PIN           0      // to RPi 4B
#define RXD1_PIN           1      // 
//#define TXD1_EN_PIN     -1      // driver enable for RS-485 (High)
#define TXD2_PIN           8      // RS-485
#define RXD2_PIN           9      //
#define TXD2_EN_PIN       28      // driver enable for RS-485 (High)

// I2C
#define SDA_PIN            4      // #
#define SCL_PIN            5      // #  

// special hardware features
#define QEDGE_BCTRL        3      // qEdge Bus control
#define ADC_KEY           A0      // keys via ADC
#define ADC_KEYVAL0      389      // > no key pressed
#define ADC_KEYVAL4      345      // key limits
#define ADC_KEYVAL3      229
#define ADC_KEYVAL2       83
#define PHYSICAL_KEY1      1      // leftmost key
#define PHYSICAL_KEY2      2
#define PHYSICAL_KEY3      3
#define PHYSICAL_KEY4      4      // rightmost key

#define PICO_VSYS         A3      // Pico module internal VSYS ADC
#define PICO_LED          25      // LED
#define PICO_USB          24      // USB present if HIGH
#define PICO_SMPS         23      // SMPS control HIGH = PWM mode else PFM (low power)

#endif
