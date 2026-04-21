#pragma once

#include "Common.h"
#include <vector>

constexpr int ROM_BANK_SIZE = 16 * 1024; // 16 KB
constexpr int RAM_BANK_SIZE =  8 * 1024; //  8 KB

class Cart {
private:
    int rom_bank_count;
    int ram_bank_count;
    int current_rom_bank;
    int current_ram_bank;
    bool ram_active;

    uint8_t* ROM;
    uint8_t* RAM;

    uint16_t get_rom_offset( void );
    uint16_t get_ram_offset( void );

    enum MAPPERS {
        MBCNONE,
        MBC1,
    };
    MAPPERS mapper;

public:
    Cart( void );
    ~Cart( void );

    uint8_t ram_read( uint16_t addr );
    void ram_write( uint16_t addr, uint8_t data );

    uint8_t rom_read( uint16_t addr );
    void rom_write( uint16_t addr, uint8_t data );

    bool load_rom( const char* path );

};