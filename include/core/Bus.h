#pragma once

#include "Common.h"

#include <vector>

#include "core/SM83.h"
#include "core/Cart.h"
#include "core/Ram/WRAM.h"
#include "core/Ram/HRAM.h"
#include "core/IO/Timer.h"

class Bus{
private:

public:
    Bus( void );
    ~Bus( void );

    uint8_t bus_read( uint16_t addr );
    void   bus_write( uint16_t addr, uint8_t data );

    void clock( void );
    void reset( void );
    bool system_error( void );

    // Gameboy CPU
    SM83 cpu;

    // Cartridge
    Cart cart;

    // System Ram
    WRAM wram;
    HRAM hram;

    // IO
    Timer timer;

    // Interrupt Register
    uint8_t IE;
    
    // Interrupts
    void clear_interrupt( uint8_t it_name );
    void request_interrupt( uint8_t it_name );
};