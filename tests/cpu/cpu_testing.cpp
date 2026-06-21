#include <gtest/gtest.h>

#include "core/SM83.h"

TEST(CPU, Initialization) {
  SM83 cpu;

  EXPECT_EQ(cpu.regs.af, 0x0000);
  EXPECT_EQ(cpu.regs.bc, 0x0000);
  EXPECT_EQ(cpu.regs.de, 0x0000);
  EXPECT_EQ(cpu.regs.hl, 0x0000);
  EXPECT_EQ(cpu.regs.pc, 0x0000);
  EXPECT_EQ(cpu.regs.sp, 0x0000);
}