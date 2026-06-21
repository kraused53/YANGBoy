#include "core/Bus.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "Addresses.h"

Bus::Bus() {
  // Connect CPU to communication bus
  cpu.connect_to_bus(this);

  reset();
}

Bus::~Bus() {}

void Bus::write(uint16_t addr, uint8_t data) {
  if (addr == 0xFF02 && data == 0x81) {
    char c = static_cast<char>(read(0xFF01));
    if (c != '\0') {
      test_rom_log += c;
    } else {
      test_done = true;
    }

    if (test_rom_log.back() == '\n') {
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
    io.io_write(addr, data);
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
    data = io.io_read(addr);
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
  io.reset();
}

void Bus::clock() { cpu.step(); }

void Bus::handle_bus_error(bool write, uint16_t addr, const char* msg) {
  spdlog::error("Invalid {} address ${:04X}: {}",
                write ? "write to" : "read from", addr, msg);

  // Stop the CPU and indicate an error
  cpu.error = true;
  cpu.halted = true;
}

bool Bus::is_between(uint16_t addr, uint16_t range_start, uint16_t range_end) {
  return (addr >= range_start) && (addr <= range_end);
}