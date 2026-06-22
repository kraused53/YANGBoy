#pragma once

#include <cstdint>

// Cart ROM - Fixed Bank
constexpr uint16_t FIXED_ROM_START = 0x0000;
constexpr uint16_t FIXED_ROM_END = 0x3FFF;

// Cart ROM - Switched Bank
constexpr uint16_t BANK_ROM_START = 0x4000;
constexpr uint16_t BANK_ROM_END = 0x7FFF;

// Video RAM
constexpr uint16_t VRAM_START = 0x8000;
constexpr uint16_t VRAM_END = 0x9FFF;

// Cart RAM
constexpr uint16_t EXTERNAL_RAM_START = 0xA000;
constexpr uint16_t EXTERNAL_RAM_END = 0xBFFF;

// Cart RAM
constexpr uint16_t WRAM_START = 0xC000;
constexpr uint16_t WRAM_END = 0xDFFF;

// Cart RAM
constexpr uint16_t RAM_ECHO_START = 0xE000;
constexpr uint16_t RAM_ECHO_END = 0xFDFF;

// OAM
constexpr uint16_t OAM_START = 0xFE00;
constexpr uint16_t OAM_END = 0xFE9F;

// Unusable
constexpr uint16_t ERROR_START = 0xFEA0;
constexpr uint16_t ERROR_END = 0xFEFF;

// IO Registers
constexpr uint16_t IO_START = 0xFF00;
constexpr uint16_t IO_END = 0xFF7F;

// HRAM
constexpr uint16_t HRAM_START = 0xFF80;
constexpr uint16_t HRAM_END = 0xFFFE;

// IE
constexpr uint16_t IE = 0xFFFF;