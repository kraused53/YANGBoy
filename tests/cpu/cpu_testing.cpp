#include <gtest/gtest.h>

#include "Cpu/SM83.h"

TEST(CPU, Initialization) {
  SM83 cpu;

  EXPECT_EQ(cpu.regs.af, 0x01B0);
  EXPECT_EQ(cpu.regs.bc, 0x0013);
  EXPECT_EQ(cpu.regs.de, 0x00D8);
  EXPECT_EQ(cpu.regs.hl, 0x014D);
  EXPECT_EQ(cpu.regs.pc, 0x0100);
  EXPECT_EQ(cpu.regs.sp, 0xFFFE);
}

TEST(CPU, Reset) {
  SM83 cpu;

  cpu.regs.af = 0xFFFF;
  cpu.regs.bc = 0xFFFF;
  cpu.regs.de = 0xFFFF;
  cpu.regs.hl = 0xFFFF;
  cpu.regs.sp = 0xFFFF;
  cpu.regs.pc = 0xFFFF;

  cpu.reset();

  EXPECT_EQ(cpu.regs.af, 0x01B0);
  EXPECT_EQ(cpu.regs.bc, 0x0013);
  EXPECT_EQ(cpu.regs.de, 0x00D8);
  EXPECT_EQ(cpu.regs.hl, 0x014D);
  EXPECT_EQ(cpu.regs.pc, 0x0100);
  EXPECT_EQ(cpu.regs.sp, 0xFFFE);
}

TEST(CPU, RegisterAccess) {
  SM83 cpu;

  cpu.regs.af = 0x1234;
  cpu.regs.bc = 0x5678;
  cpu.regs.de = 0x9012;
  cpu.regs.hl = 0x3456;
  cpu.regs.sp = 0x7890;
  cpu.regs.pc = 0x1234;

  EXPECT_EQ(cpu.regs.af, 0x1234);
  EXPECT_EQ(cpu.regs.bc, 0x5678);
  EXPECT_EQ(cpu.regs.de, 0x9012);
  EXPECT_EQ(cpu.regs.hl, 0x3456);
  EXPECT_EQ(cpu.regs.sp, 0x7890);
  EXPECT_EQ(cpu.regs.pc, 0x1234);

  EXPECT_EQ(cpu.regs.a, 0x12);
  EXPECT_EQ(cpu.regs.f, 0x34);
  EXPECT_EQ(cpu.regs.b, 0x56);
  EXPECT_EQ(cpu.regs.c, 0x78);
  EXPECT_EQ(cpu.regs.d, 0x90);
  EXPECT_EQ(cpu.regs.e, 0x12);
  EXPECT_EQ(cpu.regs.h, 0x34);
  EXPECT_EQ(cpu.regs.l, 0x56);
}