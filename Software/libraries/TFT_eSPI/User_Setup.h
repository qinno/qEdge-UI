/*
 *   uncomment right definition for display
 */

// controller select
#define ILI9341_DRIVER      // 320x240 2,4", 2,8" some 3,2"
//#define ILI9341_2_DRIVER  // 480x320 some 3,2"
//#define ILI9488_DRIVER


// board select only one !
#define QEDGE_PICO   // qEdge environment PICO

// board pinouts
#if defined(QEDGE_PICO)
#define TFT_MISO 16
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS   17  // Chip select control pin
#define TFT_DC   20  // Data Command control pin
#define TFT_RST  -1  // Reset pin (could connect to RST pin)
#define TOUCH_CS 15  // Chip select pin (T_CS) of touch screen
#endif

// Fonts loaded
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
//#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
//#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define SMOOTH_FONT

// SPI speed settings
#if defined(QEDGE_PICO)
#define SPI_FREQUENCY       22000000
#define SPI_READ_FREQUENCY  22000000
#define SPI_TOUCH_FREQUENCY  2500000
#endif




