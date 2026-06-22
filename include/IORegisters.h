#pragma once

#include <cstdint>

// Joypad Input
constexpr uint16_t P1JOYP = 0xFF00;

// Serial Transfer
constexpr uint16_t SB = 0xFF01;
constexpr uint16_t SC = 0xFF02;

// Timer
constexpr uint16_t DIV = 0xFF04;
constexpr uint16_t TIMA = 0xFF05;
constexpr uint16_t TMA = 0xFF06;
constexpr uint16_t TAC = 0xFF07;

// Interrupts
constexpr uint16_t IF = 0xFF0F;

// Audio
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

// Wave Pattern
constexpr uint16_t WR0 = 0xFF30;
constexpr uint16_t WR1 = 0xFF31;
constexpr uint16_t WR2 = 0xFF32;
constexpr uint16_t WR3 = 0xFF33;
constexpr uint16_t WR4 = 0xFF34;
constexpr uint16_t WR5 = 0xFF35;
constexpr uint16_t WR6 = 0xFF36;
constexpr uint16_t WR7 = 0xFF37;
constexpr uint16_t WR8 = 0xFF38;
constexpr uint16_t WR9 = 0xFF39;
constexpr uint16_t WRA = 0xFF3A;
constexpr uint16_t WRB = 0xFF3B;
constexpr uint16_t WRC = 0xFF3C;
constexpr uint16_t WRD = 0xFF3D;
constexpr uint16_t WRE = 0xFF3E;
constexpr uint16_t WRF = 0xFF3F;

// LCD
constexpr uint16_t LCDC = 0xFF40;
constexpr uint16_t STAT = 0xFF41;
constexpr uint16_t SCY = 0xFF42;
constexpr uint16_t SCX = 0xFF43;
constexpr uint16_t LY = 0xFF44;
constexpr uint16_t LYC = 0xFF45;
constexpr uint16_t BGP = 0xFF47;
constexpr uint16_t OBP0 = 0xFF48;
constexpr uint16_t OBP1 = 0xFF49;
constexpr uint16_t WY = 0xFF4A;
constexpr uint16_t WX = 0xFF4B;

// OAM DMA
constexpr uint16_t DMA = 0xFF46;

// KEY0 - KEY1 <CGB>
constexpr uint16_t KEY0 = 0xFF4C;
constexpr uint16_t KEY1 = 0xFF4D;

// VRAM Bank Select <CGB>
constexpr uint16_t VBK = 0xFF4F;

// Boot ROM Mapping
constexpr uint16_t BANK = 0xFF50;

// VRAM DMA <CGB>
constexpr uint16_t HDMA1 = 0xFF51;
constexpr uint16_t HDMA2 = 0xFF52;
constexpr uint16_t HDMA3 = 0xFF53;
constexpr uint16_t HDMA4 = 0xFF54;
constexpr uint16_t HDMA5 = 0xFF55;

// IR Port <CGB>
constexpr uint16_t RP = 0xFF56;

// BG - OBJ Palettes <CGB>
constexpr uint16_t BCPS = 0xFF68;
constexpr uint16_t BCPD = 0xFF69;
constexpr uint16_t OCPS = 0xFF6A;
constexpr uint16_t OCPD = 0xFF6B;

// Object Priority Mode <CGB>
constexpr uint16_t OPRI = 0xFF6C;

// WRAM Bank Select <CGB>
constexpr uint16_t SVBK = 0xFF70;

// PCM
constexpr uint16_t PCM12 = 0xFF77;
constexpr uint16_t PCM34 = 0xFF76;