# qEdge-UI software



![qEdge-UI Logo](https://github.com/qinno/qEdge-UI/blob/master/Images/qEdge.png)
---

# Getting Started

To get started with the software development you need a PC with:

## SW-Environment Prerequisites

### OS
* Windows 7 or newer (64Bit) or
* Linux (32/64Bit) or 
* MacOS 10.10 or newer

### IDE
* An Arduino IDE 1.8.x including USB Driver support (tested with 1.8.19 - 541MB) [download here](https://www.arduino.cc/en/software). Arduino IDE 2.x is not recommended, because it has no support for Littlefs (extension), Compilation with IDE 2.x should work)

### Board package
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

## Installing qEdge-UI HwTestApp

### Clone the qEdge-UI repository locally into the Arduino sources folder
* Located in the default installation path for Windows:
```
cd C:\Users\{username}\Documents\Arduino
```
  
* Located in the default installation path for Linux:
```  
cd /home/{username}/Arduino
```

* clone the repo
```
git clone https://github.com/qinno/qEdge-UI
```

### Copy & Configure the qEdge-UI HwTestApp

* Copy the file **User_Setup.h** of the repository folder Software\libaries\TFT_eSPI\ to overwrite the file with the same name in:
  
  Windows: `C:\Users\{username}\Documents\Arduino\libraries\TFT_eSPI`
  
  Linux: `/home/{username}/Arduino\libraries\TFT_eSPI`

### Download & Integrate LittleFS plugin

* Download from Github the latest ZIP archive of the LittleFS plugin (e.g. PicoLittleFS-0.2.0.zip) [Release folder of the repository of arduino-pico-littlefs-plugin from Earle F. Philhower, III](https://github.com/earlephilhower/arduino-pico-littlefs-plugin/releases). 

* Create a folder tools if it not already exists:
  
  Windows: `C:\Users\{username}\Documents\Arduino\tools`
  
  Linux: `/home/{username}/Arduino/tools`

* Unzip the LittleFS plugin archiv into the folder tools 

* Close the Ardunio IDE

![qEdge-UI with HwTestApp](https://github.com/qinno/qEdge-UI/blob/master/Images/qEdge-UI-1_800.png)

## Running HwTestApp on qEdge-UI

### Initial setup Arduino IDE

* Start the Arduino IDE and select: Tools -> Board ->Raspberry Pi RP2040 Boards(3.3.0) -> **Raspberry Pi Pico W**
* Set LittleFS size by Tools -> Flash Size -> **2MB (Sketch: 1536KB, FS: 512KB**
* Connect your qEdgeUI with an USB cable 
* Select the corresponding COM Port X for the USB communication by Tools -> Port -> COM X (Raspberry Pi Pico W)

### Loading HwTestApp sketch into the IDE and compile

* In the Arduino IDE load the sketch **HwTestApp** by File -> Open -> and selecting in the qEdgeUI-demo folder the arduino file qEdgeUI-demo.ino
* Compile the sketch by selecting: Sketch -> Compile

### Upload sketch & LittleFS files to target

* Upload the sketch by selecting: Sketch -> Upload 
* Upload LittleFS files by selecting: Tools -> Pico LittleFS Data Upload (located in HwTestApp/data)

### Calibration of the qEdge-UI

The first time when the qEdge-UI unit is used after upload, it starts with a touch calibration screen. Please follow the instructions on the touchscreen. After calibration press the save button on the touchscreen and wait up to 5 seconds (if micro sd memory card is not accessible).

If you wish to re-calibrate, power down the system (unplug the USB cable) and press any front panel key on the qEdgeUI for up to 5 seconds while power is applied.

## Behind the scenes

PNG files on Micro-SD-card or LittleFS needs to be 320x240 (4:3) to fill the whole screen. Use ImageMagick to convert existing files.

# Roadmap

See the [open issues](https://github.com/dec0dOS/amazing-github-template/issues) for a list of proposed features (and known issues).

# Contributing

First of all, thank you for taking the time to contribute! Contributions are what make the open source community such an amazing place to learn, inspire and create. Every contribution you make benefits everyone else and is **greatly appreciated**.

Please try to create bug reports that are:

- _Reproducible._ Include steps to reproduce the problem.
- _Specific._ Include as much detail as possible: which version, what environment, etc.
- _Unique._ Do not duplicate existing opened issues.
- _Scoped to a Single Bug._ One bug per report.

# Known limitations

* Main menu starts slowly if compiled with SD option and no Micro-SD-card inserted

* RTC not yet supported in software

# Support

Reach out to the maintainer at one of the following places:

- By email which is located at [qinno GmbH · GitHub](https://github.com/qinno)

# License
This project is licensed under the following open source licenses:
* Software Examples are under [**GPLv3**](https://www.gnu.org/licenses/gpl-3.0.en.html)

See the licence header in each file for more information.
