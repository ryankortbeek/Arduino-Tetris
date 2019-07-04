#ifndef __NUMS_H
#define __NUMS_H

namespace pins {
  // Display Pins
  const uint8_t TFT_DC = 9;
  const uint8_t TFT_CS = 10;
  const uint8_t SD_CS = 6;

  // Touch Screen Pins
  const uint8_t YP = A2;
  const uint8_t XM = A3;
  const uint8_t YM = 5;
  const uint8_t XP = 4;

  // Joystick Pins
  const uint8_t JOY_X  = A1; // should connect A1 to pin VRx
  const uint8_t JOY_Y = A0;  // should connect A0 to pin VRy
  const uint8_t JOY_BUTTON = 8;
}

namespace ard_consts {
  // TFT Display Constants
  const uint16_t tft_height = 320;
  const uint16_t tft_width = 240;

  // Touch Screen Constants
  const uint16_t TS_MINX = 150;
  const uint16_t TS_MINY = 120;
  const uint16_t TS_MAXX = 920;
  const uint16_t TS_MAXY = 940;

  // Jostick Constants
  const uint16_t JOY_CENTER = 512;
  const uint16_t JOY_DEADZONE = 64;

}

namespace colours {
  // Colour codes
  const uint16_t BLACK = 0x0000;
  const uint16_t WHITE = 0xFFFF;
  const uint16_t GRAY = 0x7BEF;
  const uint16_t LIGHT_GRAY = 0xC618;
  const uint16_t GREEN = 0x07E0;
  const uint16_t LIME = 0x87E0;
  const uint16_t BLUE = 0x001F;
  const uint16_t RED = 0xF800;
  const uint16_t AQUA = 0x5D1C;
  const uint16_t YELLOW = 0xFFE0;
  const uint16_t MAGENTA = 0xF81F;
  const uint16_t CYAN = 0x07FF;
  const uint16_t DARK_CYAN = 0x03EF;
  const uint16_t ORANGE = 0xFCA0;
  const uint16_t PINKK = 0xF97F;
  const uint16_t BROWN = 0x8200;
  const uint16_t VIOLET = 0x9199;
  const uint16_t SILVER = 0xA510;
  const uint16_t GOLD = 0xA508;
  const uint16_t NAVY = 0x000F;
  const uint16_t MAROON = 0x7800;
  const uint16_t PURPLE = 0x780F;
  const uint16_t OLIVE = 0x7BE0;
}

#endif
