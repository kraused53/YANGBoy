#pragma once

#include <cstdint>

// Interrupt flags
constexpr uint8_t VBLANK_FLAG = (0x01 << 0);
constexpr uint8_t LCD_FLAG = (0x01 << 1);
constexpr uint8_t TIMER_FLAG = (0x01 << 2);
constexpr uint8_t SERIAL_FLAG = (0x01 << 3);
constexpr uint8_t JOYPAD_FLAG = (0x01 << 4);