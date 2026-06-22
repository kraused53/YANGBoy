#include "Cartridge.h"

#include <fstream>
#include <iostream>

#include "Addresses.h"

Cartridge::Cartridge(void) { reset(); }

Cartridge::~Cartridge() {}

void Cartridge::reset() {
  rom_bank_count = 0;
  ram_bank_count = 0;
  current_rom_bank = 0;
  current_ram_bank = 0;
  ram_active = false;
}

/* Memory Access */
uint8_t Cartridge::ram_read(uint16_t addr) {
  if (!ram_active) {
    return 0x00;
  }
  // Make address relative to RAM BANK start
  addr -= EXTERNAL_RAM_START;
  return RAM[get_ram_offset() + addr];
}

void Cartridge::ram_write(uint16_t addr, uint8_t data) {
  if (!ram_active) {
    return;
  }
  // Make address relative to RAM BANK start
  addr -= EXTERNAL_RAM_START;
  RAM[get_ram_offset() + addr] = data;
}

uint8_t Cartridge::rom_read(uint16_t addr) {
  if (addr > BANK_ROM_END) {
    return 0x00;
  }

  // ROM bank zero
  if (addr < BANK_ROM_START) {
    return ROM[addr];
  }

  // Make address relative to BANK 1 start
  addr -= BANK_ROM_START;

  // Mappable rom bank
  return ROM[get_rom_offset() + addr];
}

void Cartridge::rom_write(uint16_t addr, uint8_t data) {
  // THIS MAPPER IS ONLY VALID FOR MBC1 - NOT COMPLETE
  if (addr < 0x2000) {
    if ((data & 0x0F) == 0x0A) {
      ram_active = true;
    } else {
      ram_active = false;
    }
  } else if (addr < 0x4000) {
    data = data & 0x1F;
    current_rom_bank = data;

    if (current_rom_bank < 1) {
      current_rom_bank = 1;
    }

    if (current_rom_bank >= rom_bank_count) {
      current_rom_bank = rom_bank_count - 1;
    }

  } else if (addr < 0x6000) {
    data = data & 0x03;
    current_ram_bank = data;

    if (current_ram_bank >= ram_bank_count) {
      current_ram_bank = ram_bank_count - 1;
    }
  }
}

/* Utility*/
size_t Cartridge::get_rom_offset() { return ROM_BANK_SIZE * current_rom_bank; }

size_t Cartridge::get_ram_offset() { return RAM_BANK_SIZE * current_ram_bank; }

/* Load a cartridge */
bool Cartridge::load_rom(const std::filesystem::path& rom_path) {
  // Attempt to open rom at given path
  std::ifstream file(rom_path, std::ios::binary | std::ios::ate);
  if (!file) {
    return false;
  }

  const std::streamsize rom_size = file.tellg();
  file.seekg(0);

  // Allocate temp buffer for rom file
  std::vector<uint8_t> buffer(rom_size);
  if (!file.read(reinterpret_cast<char*>(buffer.data()), rom_size)) {
    return false;
  }

  switch (buffer[0x0147]) {
    case 0x00:
      std::cout << ("ROM uses no mappers") << std::endl;
      break;
    case 0x01:
      std::cout << ("ROM uses MBC1") << std::endl;
      break;
    case 0x02:
      std::cout << ("ROM uses MBC1 + RAM") << std::endl;
      break;
    case 0x03:
      std::cout << ("ROM uses MBC1 + RAM + Battery") << std::endl;
      break;
    default:
      return false;
  }

  // Fetch ROM Bank Count
  switch (buffer[0x0148]) {
    case 0x00:
      rom_bank_count = 2;
      break;
    case 0x01:
      rom_bank_count = 4;
      break;
    case 0x02:
      rom_bank_count = 8;
      break;
    case 0x03:
      rom_bank_count = 16;
      break;
    case 0x04:
      rom_bank_count = 32;
      break;
    case 0x05:
      rom_bank_count = 64;
      break;
    case 0x06:
      rom_bank_count = 128;
      break;
    case 0x07:
      rom_bank_count = 256;
      break;
    case 0x08:
      rom_bank_count = 512;
      break;
    case 0x52:
      rom_bank_count = 72;
      break;
    case 0x53:
      rom_bank_count = 80;
      break;
    case 0x54:
      rom_bank_count = 96;
      break;
    default:
      rom_bank_count = 0;
      break;
  }

  current_rom_bank = 1;

  if (rom_bank_count == 0) {
    std::cout << ("ERROR: ROM file reports 0 ROM banks...") << std::endl;
    return false;
  }

  // Make sure ROM file size matches the number of rom banks stated
  if (rom_size > (ROM_BANK_SIZE * rom_bank_count)) {
    std::cout << ("ERROR: ROM file is larger than its reported size...")
              << std::endl;
    return false;
  }

  // Allocate Cartridge ROM
  ROM.resize(ROM_BANK_SIZE * rom_bank_count);
  if (ROM.size() != ROM_BANK_SIZE * rom_bank_count) {
    std::cout << ("ERROR: Could not allocate temporary ROM buffer")
              << std::endl;
    return false;
  }

  // Load ROM file into cartridge ROM
  for (int i = 0; i < rom_size; i++) {
    ROM[i] = buffer[i];
  }

  // Fetch RAM Bank Count
  switch (buffer[0x0149]) {
    case 0x02:
      ram_bank_count = 1;
      break;  //   8 KB
    case 0x03:
      ram_bank_count = 4;
      break;  //  32 KB
    case 0x04:
      ram_bank_count = 16;
      break;  // 128 KB
    case 0x05:
      ram_bank_count = 8;
      break;  //  64 KB
    default:
      ram_bank_count = 0;
      break;  // No RAM
  }

  current_ram_bank = 0;
  ram_active = false;

  if (ram_bank_count != 0) {
    RAM.resize(RAM_BANK_SIZE * ram_bank_count);
    if (RAM.size() != RAM_BANK_SIZE * ram_bank_count) {
      std::cout << ("ERROR: Could not allocate temporary ROM buffer")
                << std::endl;
      return false;
    }
  }

  std::cout << ("ROM loaded without errors") << std::endl;
  return true;
}