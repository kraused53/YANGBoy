#include "core/Bus.h"

Bus::Bus( void ) {
    Logger::Log( "Initializing System Bus..." );

    // Connect CPU to bus and reset it
    cpu.connect_to_bus( this );
    reset();
}

Bus::~Bus( void ) {
    // TODO
}

uint8_t Bus::bus_read( uint16_t addr ) {
    uint8_t ret = 0xFF;
	if( addr == 0xFFFE ) {
		//printf( "FFFE Read\n" );
	}
    if ( ( addr >= CART_ROM_START ) && ( addr <= CART_ROM_END ) ) {
		// Cart ROM - 32 KB
		ret = cart.rom_read( addr );
	} else if ( ( addr >= VRAM_START ) && ( addr <= VRAM_END ) ) {
		// VRAM - 8 KB
		//ret = ppu.vram_read( addr );
	} else if ( ( addr >= CART_RAM_START ) && ( addr <= CART_RAM_END ) ) {
		// Cart RAM - 8 KB
		ret = cart.ram_read( addr );
	} else if ( ( addr >= WRAM_START ) && ( addr <= WRAM_END ) ) {
		// WRAM - 8 KB
		ret = wram.read( addr );
	} else if ( ( addr >= WRAM_MIRROR_START ) && ( addr <= WRAM_MIRROR_END ) ) {
		// WRAM Mirror - >8 KB
		ret = wram.read( addr - 0x2000 );
	} else if ( ( addr >= OAM_START ) && ( addr <= OAM_END ) ) {
		// OAM - 160 Bytes
		//ret = ppu.oam_read( addr );
	} else if ( ( addr >= IO_START ) && ( addr <= IO_END ) ) {
		// IO - 128 Bytes
		//ret = io.io_read( addr );
		if( addr == 0xFF44 ) {
			ret = 0x90;
		}
	} else if ( ( addr >= HRAM_START ) && ( addr <= HRAM_END ) ) {
		// HRAM - 127 Bytes
		ret = hram.read( addr );

	} else if ( addr == IE_ADDRESS ) {
		// Interrupt Enable Register - 1 Byte
		// TODO IE regiseter
		ret = IE;
	}
    return ret;
}

void Bus::bus_write( uint16_t addr, uint8_t data ) {
	if( addr == 0xFF01 ) {
		//printf( "%c", data );
	}
    if ( ( addr >= CART_ROM_START ) && ( addr <= CART_ROM_END ) ) {
		// Cart ROM - 32 KB
		cart.rom_write( addr, data );
	} else if ( ( addr >= VRAM_START ) && ( addr <= VRAM_END ) ) {
		// VRAM - 8 KB
		//ppu.vram_write( addr, data );
	} else if ( ( addr >= CART_RAM_START ) && ( addr <= CART_RAM_END ) ) {
		// Cart RAM - 8 KB
		cart.ram_write( addr, data );
	} else if ( ( addr >= WRAM_START ) && ( addr <= WRAM_END ) ) {
		// WRAM - 8 KB
		wram.write( addr, data );
	} else if ( ( addr >= WRAM_MIRROR_START ) && ( addr <= WRAM_MIRROR_END ) ) {
		// WRAM Mirror - >8 KB
		wram.write( addr-0x2000, data );
	} else if ( ( addr >= OAM_START ) && ( addr <= OAM_END ) ) {
		// OAM - 160 Bytes
		//ppu.oam_write( addr, data );
	} else if ( ( addr >= IO_START ) && ( addr <= IO_END ) ) {
		// IO - 128 Bytes
		//io.io_write( addr, data );
	} else if ( ( addr >= HRAM_START ) && ( addr <= HRAM_END ) ) {
		// HRAM - 127 Bytes
		hram.write( addr, data );

	} else if ( addr == IE_ADDRESS ) {
		// Interrupt Enable Register - 1 Byte
		// TODO IE regiseter
		IE = data;
	}
}

void Bus::clock( void ) {
    // Clock CPU
    cpu.clock();

    // Clock APU

    // Clock PPU

    // Clock DMA
    
}

void Bus::reset( void ) {
    // Reset System Objects
    cpu.reset();

	IE = 0x00;
}

bool Bus::system_error( void ) {
    // Check if any bus objects have encountered an error
    if( cpu.get_error() ) {
        return true;
    }

    // No errors found, return false
    return false;
}

/* Interrupts */
void Bus::clear_interrupt( uint8_t it_name ) {
	uint8_t IF_Value = bus_read( IF );
	bus_write( IF, IF_Value & ~it_name );
}

void Bus::request_interrupt( uint8_t it_name ) {
	uint8_t IF_Value = bus_read( IF );
	bus_write( IF, IF_Value | it_name );
}