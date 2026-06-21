#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>

#include "core/Bus.h"

int main() {
  auto logger = spdlog::stdout_color_mt("yangboy");
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);  // set via config/arg in practice

  // std::filesystem::path path = "test_roms/cpu/01-special.gb";
  std::filesystem::path path = "test_roms/cpu/02-interrupts.gb";
  // std::filesystem::path path = "test_roms/cpu/03-op sp,hl.gb";
  // std::filesystem::path path = "test_roms/cpu/04-op r,imm.gb";
  // std::filesystem::path path = "test_roms/cpu/05-op rp.gb";
  // std::filesystem::path path = "test_roms/cpu/06-ld r,r.gb";
  // std::filesystem::path path = "test_roms/cpu/07-jr,jp,call,ret,rst.gb";
  // std::filesystem::path path = "test_roms/cpu/08-misc instrs.gb";
  // std::filesystem::path path = "test_roms/cpu/09-op r,r.gb";
  // std::filesystem::path path = "test_roms/cpu/10-bit ops.gb";
  // std::filesystem::path path = "test_roms/cpu/11-op a,(hl).gb";

  Bus system;
  if (!system.cartridge.load_rom(path)) {
    return 1;
  }

  uint64_t cnt = 0;
  while (!system.test_done) {
    system.clock();
    if (cnt >= 10000000) {
      spdlog::error("Test finished before rom indicated a success...");
      break;
    }
    cnt++;
  }

  return 0;
}