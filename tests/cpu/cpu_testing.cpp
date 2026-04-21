#include <gtest/gtest.h>
#include "core/SM83.h"
#include "core/Bus.h"

TEST( CPU, Initialization ) {
    SM83 cpu;

    // System registers
    EXPECT_EQ( cpu.get_A(), 0x01 );
    EXPECT_EQ( cpu.get_F(), 0xB0 );
    EXPECT_EQ( cpu.get_B(), 0x00 );
    EXPECT_EQ( cpu.get_C(), 0x13 );
    EXPECT_EQ( cpu.get_D(), 0x00 );
    EXPECT_EQ( cpu.get_E(), 0xD8 );
    EXPECT_EQ( cpu.get_H(), 0x01 );
    EXPECT_EQ( cpu.get_L(), 0x4D );
    EXPECT_EQ( cpu.get_PC(), 0x0100 );
    EXPECT_EQ( cpu.get_SP(), 0xFFFE );

    // Emulation values
    EXPECT_EQ( cpu.get_halted(), false );
    EXPECT_EQ( cpu.get_error(), false );
    EXPECT_EQ( cpu.get_ie(), false );
    EXPECT_EQ( cpu.get_ied(), false );
    EXPECT_EQ( cpu.get_OC(), 0x00 );
    EXPECT_EQ( cpu.get_cycles(), 0 );
}

TEST( CPU, NOP ) {
    Bus system;

    system.bus_write( 0x0100, 0x00 );

    system.clock();

    // System registers
    EXPECT_EQ( system.cpu.get_A(), 0x01 );
    EXPECT_EQ( system.cpu.get_F(), 0xB0 );
    EXPECT_EQ( system.cpu.get_B(), 0x00 );
    EXPECT_EQ( system.cpu.get_C(), 0x13 );
    EXPECT_EQ( system.cpu.get_D(), 0x00 );
    EXPECT_EQ( system.cpu.get_E(), 0xD8 );
    EXPECT_EQ( system.cpu.get_H(), 0x01 );
    EXPECT_EQ( system.cpu.get_L(), 0x4D );
    EXPECT_EQ( system.cpu.get_PC(), 0x0101 );
    EXPECT_EQ( system.cpu.get_SP(), 0xFFFE );

    // Emulation values
    EXPECT_EQ( system.cpu.get_halted(), false );
    EXPECT_EQ( system.cpu.get_error(), false );
    EXPECT_EQ( system.cpu.get_ie(), false );
    EXPECT_EQ( system.cpu.get_ied(), false );
    EXPECT_EQ( system.cpu.get_OC(), 0x00 );
    EXPECT_EQ( system.cpu.get_cycles(), 4 );
}