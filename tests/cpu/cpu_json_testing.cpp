#include <gtest/gtest.h>

#include <format>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "core/IBus.h"
#include "core/SM83.h"
using json = nlohmann::json;

#include <array>
namespace TestBus {
class Bus : public IBus {
 public:
  // Fake memory
  std::array<uint8_t, 64 * 1024> ram;

  Bus() {
    // Connect CPU to communication bus
    cpu.connect_to_bus(this);

    reset();
  }

  // IBus systems
  SM83 cpu;

  uint8_t read(uint16_t addr) override { return ram[addr]; }

  void write(uint16_t addr, uint8_t data) override { ram[addr] = data; }

  void reset() override {
    // Clear RAM contents, just in case :P
    for (auto& i : ram) i = 0x00;

    cpu.reset();
  }

  void clock() override { cpu.step(); }
};
}  // namespace TestBus

void load_initial_state(json data, TestBus::Bus* b) {
  b->cpu.regs.a = uint8_t(data["initial"]["a"]);
  b->cpu.regs.f = uint8_t(data["initial"]["f"]);
  b->cpu.regs.b = uint8_t(data["initial"]["b"]);
  b->cpu.regs.c = uint8_t(data["initial"]["c"]);
  b->cpu.regs.d = uint8_t(data["initial"]["d"]);
  b->cpu.regs.e = uint8_t(data["initial"]["e"]);
  b->cpu.regs.h = uint8_t(data["initial"]["h"]);
  b->cpu.regs.l = uint8_t(data["initial"]["l"]);
  b->cpu.regs.pc = uint16_t(data["initial"]["pc"]) - 1;
  b->cpu.regs.sp = uint16_t(data["initial"]["sp"]);

  for (auto& i : data["initial"]["ram"]) b->ram[uint16_t(i[0])] = uint8_t(i[1]);
}

bool test_passed(json data, TestBus::Bus* b, int test_number) {
  auto check8 = [&](const char* name, uint8_t actual,
                    uint8_t expected) -> bool {
    if (actual != expected) {
      std::cout << std::format(
          "\x1B[91mError in test {:d} [ {:s} ]: {:s} should be {:02X} but was "
          "{:02X}\033[0m\n",
          test_number, std::string(data["name"]), name, expected, actual);
      return false;
    }
    return true;
  };
  auto check16 = [&](const char* name, uint16_t actual,
                     uint16_t expected) -> bool {
    if (actual != expected) {
      std::cout << std::format(
          "\x1B[91mError in test {:d} [ {:s} ]: {:s} should be {:04X} but was "
          "{:04X}\033[0m\n",
          test_number, std::string(data["name"]), name, expected, actual);
      return false;
    }
    return true;
  };

  if (!check8("A", b->cpu.regs.a, uint8_t(data["final"]["a"]))) return false;
  if (!check8("F", b->cpu.regs.f, uint8_t(data["final"]["f"]))) return false;
  if (!check8("B", b->cpu.regs.b, uint8_t(data["final"]["b"]))) return false;
  if (!check8("C", b->cpu.regs.c, uint8_t(data["final"]["c"]))) return false;
  if (!check8("D", b->cpu.regs.d, uint8_t(data["final"]["d"]))) return false;
  if (!check8("E", b->cpu.regs.e, uint8_t(data["final"]["e"]))) return false;
  if (!check8("H", b->cpu.regs.h, uint8_t(data["final"]["h"]))) return false;
  if (!check8("L", b->cpu.regs.l, uint8_t(data["final"]["l"]))) return false;
  if (!check16("PC", b->cpu.regs.pc, uint16_t(data["final"]["pc"]) - 1))
    return false;
  if (!check16("SP", b->cpu.regs.sp, uint16_t(data["final"]["sp"])))
    return false;

  for (auto& i : data["final"]["ram"]) {
    uint16_t addr = uint16_t(i[0]);
    uint8_t expected = uint8_t(i[1]);
    if (b->ram[addr] != expected) {
      std::cout << std::format(
          "\x1B[91mError in test {:d} [ {:s} ]: Memory ${:04X} should be "
          "{:02X} but was {:02X}\033[0m\n",
          test_number, std::string(data["name"]), addr, expected, b->ram[addr]);
      return false;
    }
  }
  return true;
}

bool json_test(std::string pth, TestBus::Bus* b) {
  std::cout << std::format("Testing file: {:s}\n", pth);
  std::cout << "--------------------------------------------------\n";

  std::ifstream f(pth);
  if (!f.is_open()) {
    std::cerr << "Failed to open: " << pth << "\n";
    return false;
  }
  json data = json::parse(f);
  f.close();

  int i = 0;
  for (auto& test : data) {
    load_initial_state(test, b);
    b->clock();
    if (!test_passed(test, b, i)) {
      std::cout << "**************************************************\n";
      return false;
    }
    i++;
  }

  std::cout << std::format("\x1B[92mPassed all {:d} tests!\033[0m\n", i);
  std::cout << "**************************************************\n";
  return true;
}

// ============================================================
//  Helper macro — keeps each TEST() body to one line
// ============================================================
#ifndef JSON_TEST_DIR
#define JSON_TEST_DIR "JSON"
#endif
#define JSON_TEST(suite, name, file)                                        \
  TEST(suite, name) {                                                       \
    TestBus::Bus system;                                                    \
    EXPECT_TRUE(                                                            \
        json_test(std::string(JSON_TEST_DIR) + "/" file ".json", &system)); \
  }

// ============================================================
//  Block 1 — 0x00–0x3F
// ============================================================
JSON_TEST(JSON, NOP, "00")
JSON_TEST(JSON, LD_BC_u16, "01")
JSON_TEST(JSON, LD_iBC_A, "02")
JSON_TEST(JSON, INC_BC, "03")
JSON_TEST(JSON, INC_B, "04")
JSON_TEST(JSON, DEC_B, "05")
JSON_TEST(JSON, LD_B_u8, "06")
JSON_TEST(JSON, RLCA, "07")
JSON_TEST(JSON, LD_iu16_SP, "08")
JSON_TEST(JSON, ADD_HL_BC, "09")
JSON_TEST(JSON, LD_A_iBC, "0a")
JSON_TEST(JSON, DEC_BC, "0b")
JSON_TEST(JSON, INC_C, "0c")
JSON_TEST(JSON, DEC_C, "0d")
JSON_TEST(JSON, LD_C_u8, "0e")
JSON_TEST(JSON, RRCA, "0f")

// 0x10 STOP omitted
JSON_TEST(JSON, LD_DE_u16, "11")
JSON_TEST(JSON, LD_iDE_A, "12")
JSON_TEST(JSON, INC_DE, "13")
JSON_TEST(JSON, INC_D, "14")
JSON_TEST(JSON, DEC_D, "15")
JSON_TEST(JSON, LD_D_u8, "16")
JSON_TEST(JSON, RLA, "17")
JSON_TEST(JSON, JR_i8, "18")
JSON_TEST(JSON, ADD_HL_DE, "19")
JSON_TEST(JSON, LD_A_iDE, "1a")
JSON_TEST(JSON, DEC_DE, "1b")
JSON_TEST(JSON, INC_E, "1c")
JSON_TEST(JSON, DEC_E, "1d")
JSON_TEST(JSON, LD_E_u8, "1e")
JSON_TEST(JSON, RRA, "1f")

JSON_TEST(JSON, JR_NZ_i8, "20")
JSON_TEST(JSON, LD_HL_u16, "21")
JSON_TEST(JSON, LD_iHLi_A, "22")
JSON_TEST(JSON, INC_HL, "23")
JSON_TEST(JSON, INC_H, "24")
JSON_TEST(JSON, DEC_H, "25")
JSON_TEST(JSON, LD_H_u8, "26")
JSON_TEST(JSON, DAA, "27")
JSON_TEST(JSON, JR_Z_i8, "28")
JSON_TEST(JSON, ADD_HL_HL, "29")
JSON_TEST(JSON, LD_A_iHLi, "2a")
JSON_TEST(JSON, DEC_HL, "2b")
JSON_TEST(JSON, INC_L, "2c")
JSON_TEST(JSON, DEC_L, "2d")
JSON_TEST(JSON, LD_L_u8, "2e")
JSON_TEST(JSON, CPL, "2f")

JSON_TEST(JSON, JR_NC_i8, "30")
JSON_TEST(JSON, LD_SP_u16, "31")
JSON_TEST(JSON, LD_iHLd_A, "32")
JSON_TEST(JSON, INC_SP, "33")
JSON_TEST(JSON, INC_iHL, "34")
JSON_TEST(JSON, DEC_iHL, "35")
JSON_TEST(JSON, LD_iHL_u8, "36")
JSON_TEST(JSON, SCF, "37")
JSON_TEST(JSON, JR_C_i8, "38")
JSON_TEST(JSON, ADD_HL_SP, "39")
JSON_TEST(JSON, LD_A_iHLd, "3a")
JSON_TEST(JSON, DEC_SP, "3b")
JSON_TEST(JSON, INC_A, "3c")
JSON_TEST(JSON, DEC_A, "3d")
JSON_TEST(JSON, LD_A_u8, "3e")
JSON_TEST(JSON, CCF, "3f")

// ============================================================
//  Block 2 — 0x40–0x7F  (LD r,r  +  HALT at 0x76 omitted)
// ============================================================
JSON_TEST(JSON, LD_B_B, "40")
JSON_TEST(JSON, LD_B_C, "41")
JSON_TEST(JSON, LD_B_D, "42")
JSON_TEST(JSON, LD_B_E, "43")
JSON_TEST(JSON, LD_B_H, "44")
JSON_TEST(JSON, LD_B_L, "45")
JSON_TEST(JSON, LD_B_iHL, "46")
JSON_TEST(JSON, LD_B_A, "47")
JSON_TEST(JSON, LD_C_B, "48")
JSON_TEST(JSON, LD_C_C, "49")
JSON_TEST(JSON, LD_C_D, "4a")
JSON_TEST(JSON, LD_C_E, "4b")
JSON_TEST(JSON, LD_C_H, "4c")
JSON_TEST(JSON, LD_C_L, "4d")
JSON_TEST(JSON, LD_C_iHL, "4e")
JSON_TEST(JSON, LD_C_A, "4f")

JSON_TEST(JSON, LD_D_B, "50")
JSON_TEST(JSON, LD_D_C, "51")
JSON_TEST(JSON, LD_D_D, "52")
JSON_TEST(JSON, LD_D_E, "53")
JSON_TEST(JSON, LD_D_H, "54")
JSON_TEST(JSON, LD_D_L, "55")
JSON_TEST(JSON, LD_D_iHL, "56")
JSON_TEST(JSON, LD_D_A, "57")
JSON_TEST(JSON, LD_E_B, "58")
JSON_TEST(JSON, LD_E_C, "59")
JSON_TEST(JSON, LD_E_D, "5a")
JSON_TEST(JSON, LD_E_E, "5b")
JSON_TEST(JSON, LD_E_H, "5c")
JSON_TEST(JSON, LD_E_L, "5d")
JSON_TEST(JSON, LD_E_iHL, "5e")
JSON_TEST(JSON, LD_E_A, "5f")

JSON_TEST(JSON, LD_H_B, "60")
JSON_TEST(JSON, LD_H_C, "61")
JSON_TEST(JSON, LD_H_D, "62")
JSON_TEST(JSON, LD_H_E, "63")
JSON_TEST(JSON, LD_H_H, "64")
JSON_TEST(JSON, LD_H_L, "65")
JSON_TEST(JSON, LD_H_iHL, "66")
JSON_TEST(JSON, LD_H_A, "67")
JSON_TEST(JSON, LD_L_B, "68")
JSON_TEST(JSON, LD_L_C, "69")
JSON_TEST(JSON, LD_L_D, "6a")
JSON_TEST(JSON, LD_L_E, "6b")
JSON_TEST(JSON, LD_L_H, "6c")
JSON_TEST(JSON, LD_L_L, "6d")
JSON_TEST(JSON, LD_L_iHL, "6e")
JSON_TEST(JSON, LD_L_A, "6f")

JSON_TEST(JSON, LD_iHL_B, "70")
JSON_TEST(JSON, LD_iHL_C, "71")
JSON_TEST(JSON, LD_iHL_D, "72")
JSON_TEST(JSON, LD_iHL_E, "73")
JSON_TEST(JSON, LD_iHL_H, "74")
JSON_TEST(JSON, LD_iHL_L, "75")
// 0x76 HALT omitted
JSON_TEST(JSON, LD_iHL_A, "77")
JSON_TEST(JSON, LD_A_B, "78")
JSON_TEST(JSON, LD_A_C, "79")
JSON_TEST(JSON, LD_A_D, "7a")
JSON_TEST(JSON, LD_A_E, "7b")
JSON_TEST(JSON, LD_A_H, "7c")
JSON_TEST(JSON, LD_A_L, "7d")
JSON_TEST(JSON, LD_A_iHL, "7e")
JSON_TEST(JSON, LD_A_A, "7f")

// ============================================================
//  Block 3 — 0x80–0xBF  (ALU A, r)
// ============================================================
JSON_TEST(JSON, ADD_A_B, "80")
JSON_TEST(JSON, ADD_A_C, "81")
JSON_TEST(JSON, ADD_A_D, "82")
JSON_TEST(JSON, ADD_A_E, "83")
JSON_TEST(JSON, ADD_A_H, "84")
JSON_TEST(JSON, ADD_A_L, "85")
JSON_TEST(JSON, ADD_A_iHL, "86")
JSON_TEST(JSON, ADD_A_A, "87")
JSON_TEST(JSON, ADC_A_B, "88")
JSON_TEST(JSON, ADC_A_C, "89")
JSON_TEST(JSON, ADC_A_D, "8a")
JSON_TEST(JSON, ADC_A_E, "8b")
JSON_TEST(JSON, ADC_A_H, "8c")
JSON_TEST(JSON, ADC_A_L, "8d")
JSON_TEST(JSON, ADC_A_iHL, "8e")
JSON_TEST(JSON, ADC_A_A, "8f")

JSON_TEST(JSON, SUB_A_B, "90")
JSON_TEST(JSON, SUB_A_C, "91")
JSON_TEST(JSON, SUB_A_D, "92")
JSON_TEST(JSON, SUB_A_E, "93")
JSON_TEST(JSON, SUB_A_H, "94")
JSON_TEST(JSON, SUB_A_L, "95")
JSON_TEST(JSON, SUB_A_iHL, "96")
JSON_TEST(JSON, SUB_A_A, "97")
JSON_TEST(JSON, SBC_A_B, "98")
JSON_TEST(JSON, SBC_A_C, "99")
JSON_TEST(JSON, SBC_A_D, "9a")
JSON_TEST(JSON, SBC_A_E, "9b")
JSON_TEST(JSON, SBC_A_H, "9c")
JSON_TEST(JSON, SBC_A_L, "9d")
JSON_TEST(JSON, SBC_A_iHL, "9e")
JSON_TEST(JSON, SBC_A_A, "9f")

JSON_TEST(JSON, AND_A_B, "a0")
JSON_TEST(JSON, AND_A_C, "a1")
JSON_TEST(JSON, AND_A_D, "a2")
JSON_TEST(JSON, AND_A_E, "a3")
JSON_TEST(JSON, AND_A_H, "a4")
JSON_TEST(JSON, AND_A_L, "a5")
JSON_TEST(JSON, AND_A_iHL, "a6")
JSON_TEST(JSON, AND_A_A, "a7")
JSON_TEST(JSON, XOR_A_B, "a8")
JSON_TEST(JSON, XOR_A_C, "a9")
JSON_TEST(JSON, XOR_A_D, "aa")
JSON_TEST(JSON, XOR_A_E, "ab")
JSON_TEST(JSON, XOR_A_H, "ac")
JSON_TEST(JSON, XOR_A_L, "ad")
JSON_TEST(JSON, XOR_A_iHL, "ae")
JSON_TEST(JSON, XOR_A_A, "af")

JSON_TEST(JSON, OR_A_B, "b0")
JSON_TEST(JSON, OR_A_C, "b1")
JSON_TEST(JSON, OR_A_D, "b2")
JSON_TEST(JSON, OR_A_E, "b3")
JSON_TEST(JSON, OR_A_H, "b4")
JSON_TEST(JSON, OR_A_L, "b5")
JSON_TEST(JSON, OR_A_iHL, "b6")
JSON_TEST(JSON, OR_A_A, "b7")
JSON_TEST(JSON, CP_A_B, "b8")
JSON_TEST(JSON, CP_A_C, "b9")
JSON_TEST(JSON, CP_A_D, "ba")
JSON_TEST(JSON, CP_A_E, "bb")
JSON_TEST(JSON, CP_A_H, "bc")
JSON_TEST(JSON, CP_A_L, "bd")
JSON_TEST(JSON, CP_A_iHL, "be")
JSON_TEST(JSON, CP_A_A, "bf")

// ============================================================
//  Block 4 — 0xC0–0xFF  (control, stack, misc)
//  Invalid opcodes omitted: D3 DB DD E3 E4 EB EC ED F4 FC FD
//  EI (0xFB) and DI (0xF3) omitted by test suite
// ============================================================
JSON_TEST(JSON, RET_NZ, "c0")
JSON_TEST(JSON, POP_BC, "c1")
JSON_TEST(JSON, JP_NZ_u16, "c2")
JSON_TEST(JSON, JP_u16, "c3")
JSON_TEST(JSON, CALL_NZ_u16, "c4")
JSON_TEST(JSON, PUSH_BC, "c5")
JSON_TEST(JSON, ADD_A_u8, "c6")
JSON_TEST(JSON, RST_00, "c7")
JSON_TEST(JSON, RET_Z, "c8")
JSON_TEST(JSON, RET, "c9")
JSON_TEST(JSON, JP_Z_u16, "ca")
JSON_TEST(JSON, PREFIX_CB, "cb")
JSON_TEST(JSON, CALL_Z_u16, "cc")
JSON_TEST(JSON, CALL_u16, "cd")
JSON_TEST(JSON, ADC_A_u8, "ce")
JSON_TEST(JSON, RST_08, "cf")

JSON_TEST(JSON, RET_NC, "d0")
JSON_TEST(JSON, POP_DE, "d1")
JSON_TEST(JSON, JP_NC_u16, "d2")
// 0xD3 invalid
JSON_TEST(JSON, CALL_NC_u16, "d4")
JSON_TEST(JSON, PUSH_DE, "d5")
JSON_TEST(JSON, SUB_A_u8, "d6")
JSON_TEST(JSON, RST_10, "d7")
JSON_TEST(JSON, RET_C, "d8")
JSON_TEST(JSON, RETI, "d9")
JSON_TEST(JSON, JP_C_u16, "da")
// 0xDB invalid
JSON_TEST(JSON, CALL_C_u16, "dc")
// 0xDD invalid
JSON_TEST(JSON, SBC_A_u8, "de")
JSON_TEST(JSON, RST_18, "df")

JSON_TEST(JSON, LDH_iu8_A, "e0")
JSON_TEST(JSON, POP_HL, "e1")
JSON_TEST(JSON, LDH_iC_A, "e2")
// 0xE3 invalid
// 0xE4 invalid
JSON_TEST(JSON, PUSH_HL, "e5")
JSON_TEST(JSON, AND_A_u8, "e6")
JSON_TEST(JSON, RST_20, "e7")
JSON_TEST(JSON, ADD_SP_i8, "e8")
JSON_TEST(JSON, JP_HL, "e9")
JSON_TEST(JSON, LD_iu16_A, "ea")
// 0xEB invalid
// 0xEC invalid
// 0xED invalid
JSON_TEST(JSON, XOR_A_u8, "ee")
JSON_TEST(JSON, RST_28, "ef")

JSON_TEST(JSON, LDH_A_iu8, "f0")
JSON_TEST(JSON, POP_AF, "f1")
JSON_TEST(JSON, LDH_A_iC, "f2")
// 0xF3 DI omitted
// 0xF4 invalid
JSON_TEST(JSON, PUSH_AF, "f5")
JSON_TEST(JSON, OR_A_u8, "f6")
JSON_TEST(JSON, RST_30, "f7")
JSON_TEST(JSON, LD_HL_SP_i8, "f8")
JSON_TEST(JSON, LD_SP_HL, "f9")
JSON_TEST(JSON, LD_A_iu16, "fa")
// 0xFB EI omitted
// 0xFC invalid
// 0xFD invalid
JSON_TEST(JSON, CP_A_u8, "fe")
JSON_TEST(JSON, RST_38, "ff")
