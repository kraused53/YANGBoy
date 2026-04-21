#pragma once

#include <cstdint>
#include <format>
#include "Logger.h"

/* Bus Memory Map */
// Cart ROM - 32 KB
constexpr uint16_t    CART_ROM_START = 0x0000;
constexpr uint16_t      CART_ROM_END = 0x7FFF;

// Cart ROM 0 - 16 KB
constexpr uint16_t  ROM_BANK_0_START = 0x0000;
constexpr uint16_t    ROM_BANK_0_END = 0x3FFF;

// Cart ROM 1 - 16 KB
constexpr uint16_t  ROM_BANK_1_START = 0x4000;
constexpr uint16_t    ROM_BANK_1_END = 0x7FFF;

// VRAM - 8 KB
constexpr uint16_t        VRAM_START = 0x8000;
constexpr uint16_t          VRAM_END = 0x9FFF;

// Cart RAM - 8 KB
constexpr uint16_t    CART_RAM_START = 0xA000;
constexpr uint16_t      CART_RAM_END = 0xBFFF;

// WRAM - 8 KB
constexpr uint16_t        WRAM_START = 0xC000;
constexpr uint16_t          WRAM_END = 0xDFFF;

// WRAM Mirror - >8 KB
constexpr uint16_t WRAM_MIRROR_START = 0xE000;
constexpr uint16_t   WRAM_MIRROR_END = 0xFDFF;

// OAM - 160 Bytes
constexpr uint16_t         OAM_START = 0xFE00;
constexpr uint16_t           OAM_END = 0xFE9F;

// IO - 128 Bytes
constexpr uint16_t          IO_START = 0xFF00;
constexpr uint16_t            IO_END = 0xFF7F;

// HRAM - 127 Bytes
constexpr uint16_t        HRAM_START = 0xF800;
constexpr uint16_t          HRAM_END = 0xFFFE;

// Interrupt Enable Register - 1 Byte
constexpr uint16_t IE_ADDRESS = 0xFFFF;

/* IO Addresses */
// Joypad
constexpr uint16_t P1 = 0xFF00;
// Serial
constexpr uint16_t SB = 0xFF01;
constexpr uint16_t SC = 0xFF02;
// Timer A
constexpr uint16_t DIV = 0xFF04;
constexpr uint16_t TIMA = 0xFF05;
constexpr uint16_t TMA = 0xFF06;
constexpr uint16_t TAC = 0xFF07;
// Interrupt Flags
constexpr uint16_t IF = 0xFF0F;
// Sound
constexpr uint16_t NR10 = 0xFF10;
constexpr uint16_t NR11 = 0xFF11;
constexpr uint16_t NR12 = 0xFF12;
constexpr uint16_t NR13 = 0xFF13;
constexpr uint16_t NR14 = 0xFF14;
constexpr uint16_t NR21 = 0xFF16;
constexpr uint16_t NR22 = 0xFF17;
constexpr uint16_t NR23 = 0xFF18;
constexpr uint16_t NR24 = 0xFF19;
constexpr uint16_t NR30 = 0xFF1A;
constexpr uint16_t NR31 = 0xFF1B;
constexpr uint16_t NR32 = 0xFF1C;
constexpr uint16_t NR33 = 0xFF1D;
constexpr uint16_t NR34 = 0xFF1E;
constexpr uint16_t NR41 = 0xFF20;
constexpr uint16_t NR42 = 0xFF21;
constexpr uint16_t NR43 = 0xFF22;
constexpr uint16_t NR44 = 0xFF23;
constexpr uint16_t NR50 = 0xFF24;
constexpr uint16_t NR51 = 0xFF25;
constexpr uint16_t NR52 = 0xFF26;
// Wave Pattern RAM
constexpr uint16_t WAVE_PATTERN_RAM_START = 0xFF30;
constexpr uint16_t WAVE_PATTERN_RAM_END = 0xFF3F;
// LCD
constexpr uint16_t LCDC = 0xFF40;
constexpr uint16_t STAT = 0xFF41;
constexpr uint16_t SCY = 0xFF42;
constexpr uint16_t SCX = 0xFF43;
constexpr uint16_t LY = 0xFF44;
constexpr uint16_t LYC = 0xFF45;
constexpr uint16_t DMA = 0xFF46;
constexpr uint16_t BGP = 0xFF47;
constexpr uint16_t OBP0 = 0xFF48;
constexpr uint16_t OBP1 = 0xFF49;
constexpr uint16_t WY = 0xFF4A;
constexpr uint16_t WX = 0xFF4B;
constexpr uint16_t KEY1 = 0xFF4D;
constexpr uint16_t VBK = 0xFF4F;

constexpr uint16_t HDMA1 = 0xFF51;
constexpr uint16_t HDMA2 = 0xFF52;
constexpr uint16_t HDMA3 = 0xFF53;
constexpr uint16_t HDMA4 = 0xFF54;
constexpr uint16_t HDMA5 = 0xFF55;
constexpr uint16_t RP = 0xFF56;

constexpr uint16_t BCPS = 0xFF68;
constexpr uint16_t BCPD = 0xFF69;
constexpr uint16_t OCPS = 0xFF6A;
constexpr uint16_t OCPD = 0xFF6B;
constexpr uint16_t OPRI = 0xFF6C;

constexpr uint16_t SVBK = 0xFF70;
constexpr uint16_t PCM12 = 0xFF76;
constexpr uint16_t PCM34 = 0xFF77;