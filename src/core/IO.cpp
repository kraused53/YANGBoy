#include "core/IO.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "IORegisters.h"

IO::IO() {
  spdlog::info("Initializing IO...");
  reset();
}

IO::~IO() {}

void IO::reset() {
  spdlog::info("IO Reset...");
  // Reset registers
  iSB = 0;
  iSC = 0;
  iIF = 0xE1;
  iTIMA = 0x00;
  iTMA = 0x00;
  iTAC = 0xF8;
  iP1JOYP = 0x00;
}

uint8_t IO::io_read(uint16_t addr) {
  uint8_t data = 0x00;
  switch (addr) {
    // Joypad
    case P1JOYP:
      data = iP1JOYP & 0x30;
      if ((iP1JOYP & 0x30) == 0x10) {
        data |= (control_buttons & 0x0F);
      } else if ((iP1JOYP & 0x30) == 0x20) {
        data |= (dpad_buttons & 0x0F);
      } else {
        data |= 0x0F;
      }
      break;

    // Serial data transfer
    case SB:
      data = iSB;
      break;
    case SC:
      data = iSC;
      break;

    // Interrupt flags
    case IF:
      data = iIF | 0xE0;
      break;

      // Timer
    case TIMA:
      data = iTIMA;
      break;
    case TMA:
      data = iTMA;
      break;
    case TAC:
      data = iTAC | 0xF8;
      break;

    // PPU
    case LCDC:
      data = iLCDC;
      break;
    case STAT:
      data = iSTAT;
      break;
    case SCY:
      data = iSCY;
      break;
    case SCX:
      data = iSCX;
      break;
    case LY:
      data = iLY;
      break;
    case LYC:
      data = iLYC;
      break;
    case BGP:
      data = iBGP;
      break;
    case OBP0:
      data = iOBP0;
      break;
    case OBP1:
      data = iOBP1;
      break;
    case WY:
      data = iWY;
      break;
    case WX:
      data = iWX;
      break;

    // Not implemented yet / error
    default:
      // spdlog::warn("IO register ${:04x} has not been implemented yet... ",
      //              addr);
      break;
  }
  return data;
}

/*
  uint8_t BGP;
  uint8_t OBP0;
  uint8_t OBP1;
  uint8_t WY;
  uint8_t WX;
*/

void IO::io_write(uint16_t addr, uint8_t data) {
  switch (addr) {
    // Joypad
    case P1JOYP:
      data &= 0x30;
      iP1JOYP &= ~0x30;
      iP1JOYP |= data;
      break;

    // Serial data transfer
    case SB:
      iSB = data;
      break;
    case SC:
      iSC = data;
      break;

      // Interrupt flags
    case IF:
      iIF = data | 0xE0;
      break;

      // Timer
    case TIMA:
      iTIMA = data;
      break;
    case TMA:
      iTMA = data;
      break;
    case TAC:
      iTAC = data & 0x07;
      break;

    // PPU
    case LCDC:
      iLCDC = data;
      break;
    case STAT:
      iSTAT = data;
      break;
    case SCY:
      iSCY = data;
      break;
    case SCX:
      iSCX = data;
      break;
    case LY:
      iLY = data;
      break;
    case LYC:
      iLYC = data;
      break;
    case BGP:
      iBGP = data;
      break;
    case OBP0:
      iOBP0 = data;
      break;
    case OBP1:
      iOBP1 = data;
      break;
    case WY:
      iWY = data;
      break;
    case WX:
      iWX = data;
      break;

    // Not implemented yet / error
    default:
      // spdlog::warn("IO register ${:04x} has not been implemented yet... ",
      //              addr);
      break;
  }
}