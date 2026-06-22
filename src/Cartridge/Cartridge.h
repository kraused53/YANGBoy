#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

constexpr unsigned int ROM_BANK_SIZE = 16 * 1024;
constexpr unsigned int RAM_BANK_SIZE = 8 * 1024;

class Cartridge {
 private:
  unsigned int rom_bank_count;
  unsigned int ram_bank_count;
  unsigned int current_rom_bank;
  unsigned int current_ram_bank;
  bool ram_active;

  std::vector<uint8_t> ROM;
  std::vector<uint8_t> RAM;

  size_t get_rom_offset();
  size_t get_ram_offset();

  enum MAPPERS { MBCNONE, MBC1 };
  MAPPERS mapper;

 public:
  Cartridge();
  ~Cartridge();

  void reset();

  uint8_t rom_read(uint16_t addr);
  void rom_write(uint16_t addr, uint8_t data);

  uint8_t ram_read(uint16_t addr);
  void ram_write(uint16_t addr, uint8_t data);

  bool load_rom(const std::filesystem::path& rom_path);
};