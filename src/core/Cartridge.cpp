#include "core/Cartridge.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <fstream>

#include "Addresses.h"

Cartridge::Cartridge(void) {
  spdlog::info("Initializing Cartridge...");
  reset();
}

Cartridge::~Cartridge() {}

void Cartridge::reset() {
  spdlog::info("Cartridge Reset");
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
  spdlog::trace("Reading the value {:02X} from RAM Bank {:02d} ${:04X}",
                RAM[get_ram_offset() + addr], current_ram_bank,
                get_ram_offset() + addr);
  return RAM[get_ram_offset() + addr];
}

void Cartridge::ram_write(uint16_t addr, uint8_t data) {
  if (!ram_active) {
    return;
  }
  // Make address relative to RAM BANK start
  addr -= EXTERNAL_RAM_START;
  RAM[get_ram_offset() + addr] = data;
  spdlog::trace("Writing the value {:02X} to RAM Bank {:02d} ${:04X}", data,
                current_ram_bank, get_ram_offset() + addr);
}

uint8_t Cartridge::rom_read(uint16_t addr) {
  if (addr > BANK_ROM_END) {
    spdlog::error("Invalid cartridge rom read: ${:04X}", addr);
    return 0x00;
  }

  // ROM bank zero
  if (addr < BANK_ROM_START) {
    spdlog::trace("Reading the value {:02X} from ROM Bank 00 ${:04X}",
                  ROM[addr], addr);
    return ROM[addr];
  }

  // Make address relative to BANK 1 start
  addr -= BANK_ROM_START;

  spdlog::trace("Reading the value {:02X} from ROM Bank {:02d} ${:04X}",
                ROM[get_rom_offset() + addr], current_rom_bank,
                get_rom_offset() + addr);

  // Mappable rom bank
  return ROM[get_rom_offset() + addr];
}

void Cartridge::rom_write(uint16_t addr, uint8_t data) {
  // THIS MAPPER IS ONLY VALID FOR MBC1 - NOT COMPLETE
  if (addr < 0x2000) {
    if ((data & 0x0F) == 0x0A) {
      spdlog::debug("Enabling RAM...");
      ram_active = true;
    } else {
      spdlog::debug("Disabling RAM...");
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

    spdlog::debug("Switching ROM to bank {:02d}...", current_rom_bank);
  } else if (addr < 0x6000) {
    data = data & 0x03;
    current_ram_bank = data;

    if (current_ram_bank >= ram_bank_count) {
      current_ram_bank = ram_bank_count - 1;
    }

    spdlog::debug("Switching RAM to bank {:02d}...", current_ram_bank);
  }
}

/* Utility*/
size_t Cartridge::get_rom_offset() { return ROM_BANK_SIZE * current_rom_bank; }

size_t Cartridge::get_ram_offset() { return RAM_BANK_SIZE * current_ram_bank; }

/* Load a cartridge */
bool Cartridge::load_rom(const std::filesystem::path& rom_path) {
  spdlog::info("Loading ROM file: {}", rom_path.string());

  // Attempt to open rom at given path
  std::ifstream file(rom_path, std::ios::binary | std::ios::ate);
  if (!file) {
    spdlog::error("Error opening the file: {}", rom_path.string());
    return false;
  }
  spdlog::trace("File opened without error...");

  const std::streamsize rom_size = file.tellg();
  file.seekg(0);
  spdlog::trace("ROM size: {} bytes", rom_size);

  // Allocate temp buffer for rom file
  std::vector<uint8_t> buffer(rom_size);
  if (!file.read(reinterpret_cast<char*>(buffer.data()), rom_size)) {
    spdlog::error("Failed to read ROM file into buffer");
    return false;
  }

  switch (buffer[0x0147]) {
    case 0x00:
      spdlog::info("ROM uses no mappers");
      break;
    case 0x01:
      spdlog::info("ROM uses MBC1");
      break;
    case 0x02:
      spdlog::info("ROM uses MBC1 + RAM");
      break;
    case 0x03:
      spdlog::info("ROM uses MBC1 + RAM + Battery");
      break;
    default:
      spdlog::error("ERROR: ROM uses un supported mapper: {:02X}",
                    buffer[0x0147]);
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

  spdlog::info("ROM has {:d} banks", rom_bank_count);

  current_rom_bank = 1;

  if (rom_bank_count == 0) {
    spdlog::error("ERROR: ROM file reports 0 ROM banks...");
    return false;
  }

  // Make sure ROM file size matches the number of rom banks stated
  if (rom_size > (ROM_BANK_SIZE * rom_bank_count)) {
    spdlog::error("ERROR: ROM file is larger than its reported size...");
    return false;
  }

  // Allocate Cartridge ROM
  ROM.resize(ROM_BANK_SIZE * rom_bank_count);
  if (ROM.size() != ROM_BANK_SIZE * rom_bank_count) {
    spdlog::error("ERROR: Could not allocate temporary ROM buffer");
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
  spdlog::info("RAM has {:d} banks", ram_bank_count);

  current_ram_bank = 0;
  ram_active = false;

  if (ram_bank_count != 0) {
    RAM.resize(RAM_BANK_SIZE * ram_bank_count);
    if (RAM.size() != RAM_BANK_SIZE * ram_bank_count) {
      spdlog::error("ERROR: Could not allocate temporary ROM buffer");
      return false;
    }
  }

  spdlog::info("ROM loaded without errors");
  return true;
}