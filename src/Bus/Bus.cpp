#include "Bus.h"

#include <iostream>

#include "Addresses.h"
#include "IORegisters.h"

Bus::Bus() {
  // Connect CPU to communication bus
  cpu.connect_to_bus(this);
  timer.connect_to_bus(this);
  ppu.connect_to_bus(this, framebuffer);

  reset();
}

Bus::~Bus() {}

void Bus::write(uint16_t addr, uint8_t data) {
  if (addr == SC && data == 0x81) {
    return;
  }

  if (is_between(addr, FIXED_ROM_START, FIXED_ROM_END)) {
    cartridge.rom_write(addr, data);
  } else if (is_between(addr, BANK_ROM_START, BANK_ROM_END)) {
    cartridge.rom_write(addr, data);
  } else if (is_between(addr, VRAM_START, VRAM_END)) {
    ram.vram_write(addr, data);
  } else if (is_between(addr, EXTERNAL_RAM_START, EXTERNAL_RAM_END)) {
    cartridge.ram_write(addr, data);
  } else if (is_between(addr, WRAM_START, WRAM_END)) {
    ram.wram_write(addr, data);
  } else if (is_between(addr, RAM_ECHO_START, RAM_ECHO_END)) {
    // handle_bus_error(true, addr, "RAM Echo not implemented!");
  } else if (is_between(addr, OAM_START, OAM_END)) {
    ram.oam_write(addr, data);
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
  }
}

uint8_t Bus::read(uint16_t addr) {
  uint8_t data = 0xFF;

  if (is_between(addr, FIXED_ROM_START, FIXED_ROM_END)) {
    data = cartridge.rom_read(addr);
  } else if (is_between(addr, BANK_ROM_START, BANK_ROM_END)) {
    data = cartridge.rom_read(addr);
  } else if (is_between(addr, VRAM_START, VRAM_END)) {
    data = ram.vram_read(addr);
  } else if (is_between(addr, EXTERNAL_RAM_START, EXTERNAL_RAM_END)) {
    data = cartridge.ram_read(addr);
  } else if (is_between(addr, WRAM_START, WRAM_END)) {
    data = ram.wram_read(addr);
  } else if (is_between(addr, RAM_ECHO_START, RAM_ECHO_END)) {
    // handle_bus_error(false, addr, "RAM Echo not implemented!");
  } else if (is_between(addr, OAM_START, OAM_END)) {
    data = ram.oam_read(addr);
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
  }

  return data;
}

void Bus::reset() {
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
  ppu.tick(cpu_cycles);
}

bool Bus::is_between(uint16_t addr, uint16_t range_start, uint16_t range_end) {
  return (addr >= range_start) && (addr <= range_end);
}

bool Bus::load_rom(const std::filesystem::path& rom_path) {
  return cartridge.load_rom(rom_path);
}

// Interrupts
uint8_t Bus::get_ie() { return ram.ie_read(); }

uint8_t Bus::get_if() { return io.get_if(); }

void Bus::set_if(uint8_t val) { io.set_if(val); }

void Bus::request_interrupt(uint8_t interrupt_flag) {
  uint8_t if_register = io.get_if();
  if_register |= interrupt_flag;
  io.set_if(if_register);
}

void Bus::dma_transfer(uint8_t data) {
  uint16_t addr = data << 8;
  for (uint8_t i = 0; i < 0xA0; i++) {
    write(0xFE00 + i, read(addr + i));
  }
}