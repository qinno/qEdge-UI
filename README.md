# qEdgeUI

![Preview](./qEdgeUI-1_800.png)
---

# About

This project is an example of use of what can be achieved with free/libre and open source software and free/libre and open source tools in a professional proceeding. 
In contrast to the generally accepted approach of commercial companies, the development results of this project (hardware and mechanical design, as well as software source code) are also available under an open source licence.

Based on the Raspberry pico, we have created a design that can be used as a starting point for further development. In order to keep things as simple as possible we use the Arduino platform.

Key hardware features of **qEdgeUI**:

- Raspberry pico RP2040
- Dual-Core ARM Cortex M0+ CPU @133 MHz
- 264KB SRAM and 2MB Flash
- WiFi + Bluetooth (pico-W)
- USB interface and power supply
- additional 12/24 VDC supply
- 3,2" TFT-Display 320x240 with resistive Touch
- 4x1 keypad
- expansion connector for UART, I²C, GPIO, ADC
- RS-485 Interface
- SD-Card slot
- LoRa module with stable SMA connector
- RTC with Ultracap buffer
- Buzzer
- Raspberry Pi 3b/4b connectable (optional, not tested)

# Built With

- [KiCad EDA](https://www.kicad.org/)
- [Arduino® IDE](https://www.arduino.cc/)
- [Raspberry Pi Pico Arduino core](https://github.com/earlephilhower/arduino-pico)
- [TFT_eSPI graphics library](https://github.com/Bodmer/TFT_eSPI)
- and other libraries

# Getting Started

## Prerequisites (baseboard & housing)

### HW-Environment

To get started you will need the qEdgeUI hardware.
You have two options to acquire the hardware:
* You can build the baseboard and housing by your own by using the electronics documentation in [baseboard in KiCAD format] and 
the [STL files for 3D-printing a housing and Buttons].
* Or you can order a qEdgeUI (baseboard and housing)[here].

### SW-Environment
To get started with the software development you need a PC with:
* preinstalled Windows 7 or newer (64Bit), Linux (32/64Bit) or MacOS 10.10 newer
* An Arduino IDE 1.8.x including USB Driver support (tested with 1.8.19 - 541MB) [download here](https://www.arduino.cc/en/software). Arduino IDE 2.x is not recommended, because it has no support for Littlefs (extension), Compilation with IDE 2.x should work)
* The Raspberry Pi Pico / RP2040 Board package by Earle F. Philhower, III. Please follow the [installation guide](https://github.com/earlephilhower/arduino-pico#installation) on Github from Earle F. Philhower, III in the arduino-pico repository. Only the installtion steps under the section "Installation" are needed.

### Libraries

Install the following libraries, from the Arduino IDE, by selecting the "Sketch" menu and then selecting "Include Library" and then clicking on "Manage Libraries":
* TFT_eSPI (by Bodmer - version 2.5.0)
* TFT_eWidget (by Bodmer - version 0.0.5)
* PNGdec (by Larry Bank - version 1.0.1)
* Adafruit_NeoPixel (by Adafruit - version 1.11.0)
* LoRa (by Sandeep Mistry - version 0.8.0)

#### Configure LoRA libary
* Open with an editor the file LoRa.h in Arduino/libraries/LoRa/src/
* Change in Line 23 from:

```
#define LORA_DEFAULT_SPI           SPI
```

to:

```
#define LORA_DEFAULT_SPI           SPI1
```


## Installing qEdgeUI HwTestApp

### Clone the qEdgeUI repository e.g. with git locally

* git clone https://github.com/qinno/qEdgeUI-Software

### Copy & Configure the HwTestApp

* Copy the whole folder **apps/qEdgeUI-HwTestApp** of the repository to your Arduino sources folder.

** Located in the default installation path for Windows:
***C:\Users\{username}\Documents\Arduino**

** Located in the default installtion path for Linux:   ***/home/{username}/Arduino**

* Copy the file **User_Setup.h** of the repository folder libaries\TFT_eSPI\ to overwrite the file with the same name in:

**Windows: **C:\Users\{username}\Documents\Arduino\libraries\TFT_eSPI**

**Linux: **/home/{username}/Arduino\libraries\TFT_eSPI**


### Download & Integrate LittleFS plugin

* Download from Github the latest ZIP archive of the LittleFS plugin (e.g. PicoLittleFS-0.2.0.zip) [Release folder of the repository of arduino-pico-littlefs-plugin from Earle F. Philhower, III](https://github.com/earlephilhower/arduino-pico-littlefs-plugin/releases). 

* Create a folder tools if it not already exists:
** Windows: **C:\Users\{username}\Documents\Arduino\tools**
** Linux: **/home/{username}/Arduino/tools**
* Unzip the LittleFS plugin archiv and copy the folder PicoLittleFS folder with its subfolder into the folder tools 
* Close the Ardunio IDE

![Preview](./qEdgeUI-2_800.png)

## Running qEdgeUI HwTestApp on qEdgeUI

### Initial setup Arduino IDE

* Start the Arduino IDE and select: Tools -> Board ->Raspberry Pi RP2040 Boards(3.3.0) -> **Raspberry Pi Pico W**
* Set LittleFS size by Tools -> Flash Size -> **2MB (Sketch: 1536KB, FS: 512KB**
* Connect your qEdgeUI with an USB cable 
* Select the corresponding COM Port X for the USB communication by Tools -> Port -> COM X (Raspberry Pi Pico W)

### Loading HwTestApp sketch into the IDE and compile

* In the Arduino IDE load the sketch **qEdgeUI-HwTestApp** by File -> Open -> and selecting in the qEdgeUI-HwTestApp folder the arduino file qEdgeUI-HwTestApp.ino
* Compile the sketch by selecting: Sketch -> Compile

### Upload sketch & LittleFS files to target

* Upload the sketch by selecting: Sketch -> Upload 
* Upload LittleFS files by selecting: Tools -> Pico LittleFS Data Upload (this uploads the data located in qEdgeUI-HwTestApp/data/)

### Calibration of the qEdgeUI 

The first time when the qEdgeUI unit is used after upload, it starts with a touch calibration screen. Please follow the instructions on the touchscreen. After calibration press the save button on the touchscreen and wait up to 5 seconds (if micro sd memory card is not accessible).

If you want to do the calibration again, you have to power off the system (unplug the USB cable) and press any front panel key of the qEdgeUI up to 5 seconds while appling power.


### qEdgeUI Demo functionality

The main objective of the qEdgeUI Demo software is to check the functionality of the hardware.

* Touch - check precision of touch screen
* Slideshow - show PNG files on SD-Card (micro)
* UART - Test UART1 (RX/TX)
* RS-485 - Test UART2 connected to RS-485 driver (RX/TX)
* SD-Card - check SD-Card presence and list files on SD-CARD
* LittleFS - list, copy, rename and delete files in the filesystems
* LoRa - send/receive LoRa messages
* WiFi - scan and log in a network for receiving NTP time information
* GAME - space shooters clone to show performanceei
* Info - show system infos
* Keyboard - keyboard emulation
* Settings - set RGB preferences and Backlight, test Buzzer

## behind the scenes

PNG files on SD-Card or LittleFS needs to be 320x240 (4:3) to fill the whole screen. Use ImageMagick to convert existing files.

# Roadmap

See the [open issues](https://github.com/dec0dOS/amazing-github-template/issues) for a list of proposed features (and known issues).

# Contributing

First off, thanks for taking the time to contribute! Contributions are what makes the open-source community such an amazing place to learn, inspire, and create. Any contributions you make will benefit everybody else and are **greatly appreciated**.

Please try to create bug reports that are:

- _Reproducible._ Include steps to reproduce the problem.
- _Specific._ Include as much detail as possible: which version, what environment, etc.
- _Unique._ Do not duplicate existing opened issues.
- _Scoped to a Single Bug._ One bug per report.

# Use and Regulations

Please note that this is an engineering example and is not certified to European regulations such as CE and RED, so use it at your own risk. If you find this platform helpful, we can assist you in creating a production-ready and certified product.

# known Limitations

* main menu starts slowly if compiled with SD card and no SD card inserted

* RTC not yet supported in software

# Support

Reach out to the maintainer at one of the following places:

- The email which is located at [qinno GmbH · GitHub](https://github.com/qinno)

# License

This project is licensed under the **LGPL license**. 

See LICENSE for more information.

# Acknowledgements

Thanks to our partners supporting this project:

Axel Isele for the mechanics design

[KT Kunststofftechnik for their CNC milling/bending/welding housing](https://www.kt-technik.com/en/home/)

[Würth Elektronik for free samples for the very first prototypes](https://www.we-online.com/de)

All qinno-colleagues for their commitment even in their free time

Thanks also for these awesome resources that were used during the development of the **qEdgeUI**:

* [the KiCAD team for their great CAE tool](https://www.kicad.org/)
* [Earle F. Philhower, III for his Arduino library for the RP2040](https://github.com/earlephilhower/arduino-pico)
* [Bodmer for his versatile TFT-Library](https://github.com/Bodmer/TFT_eSPI)
* [Larry Bank for his PNG extension](https://github.com/bitbank2/PNGdec)
* [The Adafruit Industries Team for their RGB-LED library](https://github.com/adafruit/Adafruit_NeoPixel)
* [and the Arduino team for their easy to use (now legacy) IDE](https://www.arduino.cc/en/software)
* [Sandeep Mistry for his handy LoRa library](https://github.com/sandeepmistry/arduino-LoRa)
