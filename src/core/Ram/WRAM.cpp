#include "core/Ram/WRAM.h"

WRAM::WRAM() {
    reset();
}


WRAM::~WRAM() {
    
}

void WRAM::reset() {
    memory.fill( 0x00 );
}

uint8_t WRAM::read( uint16_t addr ) {
    return memory[addr - WRAM_START];
}

void WRAM::write( uint16_t addr, uint8_t data ) {
    memory[addr - WRAM_START] = data;
}