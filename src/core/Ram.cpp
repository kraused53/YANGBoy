#include "core/Ram.h"

Ram::Ram( void ) {
    for( int i = 0; i < 4 * 1024; i++ ) {
        WRAM[0] = 0x00;
    }

    for( int i = 0; i < 127; i++ ) {
        HRAM[0] = 0x00;
    }
}

void Ram::wram_write( uint16_t addr, uint8_t data ) {
	// Make address relative to wram start
	addr -= WRAM_START;
	WRAM[addr] = data;
}

uint8_t Ram::wram_read( uint16_t addr ) {
	// Make address relative to wram start
	addr -= WRAM_START;
	return WRAM[addr];
}

void Ram::hram_write( uint16_t addr, uint8_t data ) {
	// Make address relative to hram start
	addr -= HRAM_START;
	HRAM[addr] = data;
}

uint8_t Ram::hram_read( uint16_t addr ) {
	// Make address relative to hram start
	addr -= HRAM_START;
	return HRAM[ addr ];
}