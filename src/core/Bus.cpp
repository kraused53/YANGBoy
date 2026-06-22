#include "core/Bus.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "Addresses.h"
#include "IORegisters.h"

Bus::Bus() {
  // Connect CPU to communication bus
  cpu.connect_to_bus(this);
  timer.connect_to_bus(this);

  reset();
}

Bus::~Bus() {}

void Bus::write(uint16_t addr, uint8_t data) {
  if (addr == SC && data == 0x81) {
    char c = static_cast<char>(read(SB));
    if (c != '\0') {
      test_rom_log += c;
    } else {
      test_done = true;
    }

    if (!test_rom_log.empty() && test_rom_log.back() == '\n') {
      if (test_rom_log == "Passed\n") {
        test_done = true;
      }
      spdlog::info("[SERIAL] {}",
                   test_rom_log.substr(0, test_rom_log.size() - 1));
      test_rom_log.clear();
    }
    return;
  }

  if (is_between(addr, FIXED_ROM_START, FIXED_ROM_END)) {
    cartridge.rom_write(addr, data);
  } else if (is_between(addr, BANK_ROM_START, BANK_ROM_END)) {
    cartridge.rom_write(addr, data);
  } else if (is_between(addr, VRAM_START, VRAM_END)) {
    // handle_bus_error(true, addr, "VRAM not implemented!");
  } else if (is_between(addr, EXTERNAL_RAM_START, EXTERNAL_RAM_END)) {
    cartridge.ram_write(addr, data);
  } else if (is_between(addr, WRAM_START, WRAM_END)) {
    ram.wram_write(addr, data);
  } else if (is_between(addr, RAM_ECHO_START, RAM_ECHO_END)) {
    // handle_bus_error(true, addr, "RAM Echo not implemented!");
  } else if (is_between(addr, OAM_START, OAM_END)) {
    // handle_bus_error(true, addr, "PPU not implemented!");
  } else if (is_between(addr, IO_START, IO_END)) {
    if (addr == DIV) {
      timer.clear_div();
    } else if (addr == DMA) {
      dma_transfer(data);
    } else {
      io.io_write(addr, data);
    }
  } else if (is_between(addr, HRAM_START, HRAM_END)) {
    ram.hram_write(addr, data);
  } else if (addr == IE) {
    ram.ie_write(data);
  } else {
    handle_bus_error(true, addr, "Invalid Address!");
  }
}

uint8_t Bus::read(uint16_t addr) {
  uint8_t data = 0xFF;

  if (is_between(addr, FIXED_ROM_START, FIXED_ROM_END)) {
    data = cartridge.rom_read(addr);
  } else if (is_between(addr, BANK_ROM_START, BANK_ROM_END)) {
    data = cartridge.rom_read(addr);
  } else if (is_between(addr, VRAM_START, VRAM_END)) {
    // handle_bus_error(false, addr, "VRAM not implemented!");
  } else if (is_between(addr, EXTERNAL_RAM_START, EXTERNAL_RAM_END)) {
    data = cartridge.ram_read(addr);
  } else if (is_between(addr, WRAM_START, WRAM_END)) {
    data = ram.wram_read(addr);
  } else if (is_between(addr, RAM_ECHO_START, RAM_ECHO_END)) {
    // handle_bus_error(false, addr, "RAM Echo not implemented!");
  } else if (is_between(addr, OAM_START, OAM_END)) {
    // handle_bus_error(false, addr, "PPU not implemented!");
  } else if (is_between(addr, IO_START, IO_END)) {
    if (addr != DIV) {
      data = io.io_read(addr);
    } else {
      data = timer.get_div();
    }
  } else if (is_between(addr, HRAM_START, HRAM_END)) {
    data = ram.hram_read(addr);
  } else if (addr == IE) {
    data = ram.ie_read();
  } else {
    handle_bus_error(false, addr, "Invalid Address!");
  }

  return data;
}

void Bus::reset() {
  spdlog::info("Full system reset...");

  test_done = false;

  cpu.reset();
  ram.reset();
  cartridge.reset();
  timer.reset();
  io.reset();
}

void Bus::clock() {
  int cpu_cycles = cpu.step();

  timer.tick(cpu_cycles);
}

void Bus::handle_bus_error(bool write, uint16_t addr, const char* msg) {
  spdlog::error("Invalid {} address ${:04X}: {}",
                write ? "write to" : "read from", addr, msg);

  // Stop the CPU and indicate an error
  // cpu.error = true;
  // cpu.halted = true;
}

bool Bus::is_between(uint16_t addr, uint16_t range_start, uint16_t range_end) {
  return (addr >= range_start) && (addr <= range_end);
}

// Interrupts
uint8_t Bus::get_ie() { return ram.ie_read(); }

uint8_t Bus::get_if() { return io.get_if(); }

void Bus::set_if(uint8_t val) { io.set_if(val); }

void Bus::request_interrupt(uint8_t interrupt_flag) {
  uint8_t if_register = io.get_if();
  if_register |= interrupt_flag;
  io.set_if(if_register);

  switch (interrupt_flag) {
    case VBLANK_FLAG:
      spdlog::debug("VBLANK interrupt requested");
      break;
    case LCD_FLAG:
      spdlog::debug("LCD interrupt requested");
      break;
    case TIMER_FLAG:
      spdlog::debug("TIMER interrupt requested");
      break;
    case SERIAL_FLAG:
      spdlog::debug("SERIAL interrupt requested");
      break;
    case JOYPAD_FLAG:
      spdlog::debug("JOYPAD interrupt requested");
      break;

    default:
      spdlog::warn("Unknown interrupt requested...");
      break;
  }
}

void Bus::dma_transfer(uint8_t data) {
  uint16_t addr = data << 8;
  spdlog::info("dma transfer ${:02X} -> ${:04X}", data, addr);
  for (uint8_t i = 0; i < 0xA0; i++) {
    write(0xFE00 + i, read(addr + i));
  }
}