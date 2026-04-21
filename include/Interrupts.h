#pragma once

#include "Common.h"

// Interrupt names
constexpr uint8_t   IT_VBLANK = 0x01;
constexpr uint8_t IT_LCD_STAT = 0x02;
constexpr uint8_t    IT_TIMER = 0x04;
constexpr uint8_t   IT_SERIAL = 0x08;
constexpr uint8_t   IT_JOYPAD = 0x10;

// Interrupt Vectors
constexpr uint16_t   IV_VBLANK = 0x0040;
constexpr uint16_t IV_LCD_STAT = 0x0048;
constexpr uint16_t    IV_TIMER = 0x0050;
constexpr uint16_t   IV_SERIAL = 0x0058;
constexpr uint16_t   IV_JOYPAD = 0x0060;