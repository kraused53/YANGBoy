#include "core/Cart.h"

#include <format>
#include <fstream>

/* Constructors */
Cart::Cart( void ) {
    rom_bank_count = 0;
    ram_bank_count = 0;
    current_rom_bank = 0;
    current_ram_bank = 0;
    ram_active = false;

	ROM = nullptr;
	RAM = nullptr;
}

Cart::~Cart( void ) {
	free(ROM);
	free(RAM);
}

/* Memory Access */
uint8_t Cart::ram_read( uint16_t addr ) {
	if ( !ram_active ) {
		return 0x00;
	}
	// Make address relative to RAM BANK start
	addr -= CART_RAM_START;
	return RAM[get_ram_offset() + addr];
}

void Cart::ram_write( uint16_t addr, uint8_t data ) {
	if ( !ram_active ) {
		return;
	}
	// Make address relative to RAM BANK start
	addr -= CART_RAM_START;
	RAM[get_ram_offset() + addr] = data;
}

uint8_t Cart::rom_read( uint16_t addr ) { 
	if ( addr > ROM_BANK_1_END ) {
		Logger::Err( std::format( "Invalid cartridge rom read: ${:04X}", addr ));
		return 0x00;
	}
	
	// ROM bank zero
	if ( addr < ROM_BANK_1_START ) {
		return ROM[addr];
	}

	// Make address relative to BANK 1 start
	addr -= ROM_BANK_1_START;

	// Mappable rom bank
	return ROM[get_rom_offset() + addr];
}

void Cart::rom_write( uint16_t addr, uint8_t data ) {
	// THIS MAPPER IS ONLY VALID FOR MBC1 - NOT COMPLETE
	if ( addr < 0x2000 ) {
		if ( ( data & 0x0F ) == 0x0A ) {
			ram_active = true;
		} else {
			ram_active = false;
		}
	} else if ( addr < 0x4000 ) {
		data = data & 0x1F;
		current_rom_bank = data;

		if ( current_rom_bank < 1 ) {
			current_rom_bank = 1;
		}

		if ( current_rom_bank >= rom_bank_count ) {
			current_rom_bank = rom_bank_count - 1;
		}
	} else if ( addr < 0x6000 ) {
		data = data & 0x03;
		current_ram_bank = data;

		if ( current_ram_bank >= ram_bank_count ) {
			current_ram_bank = ram_bank_count - 1;
		}
	}
}

uint16_t Cart::get_rom_offset() {
	return ROM_BANK_SIZE * current_rom_bank;
}

uint16_t Cart::get_ram_offset() {
	return RAM_BANK_SIZE * current_ram_bank;
}

/* Loading */
bool Cart::load_rom( const char* path ) {
    Logger::Log( std::format( "Loading ROM file: {:s}", path ) );

	// Attempt to open rom at given path
	FILE* f = fopen( path, "rb" );
	if ( !f ) {
		Logger::Err( "ERROR: Could not open the given rom..." );
		return false;
	}

	fseek( f, 0, SEEK_END );
	int rom_size = ftell( f );
	rewind( f );

	// Allocate temp buffer for rom file
	uint8_t* buffer = ( uint8_t* ) calloc( rom_size, sizeof( uint8_t ) );
	if ( !buffer ) {
		fclose( f );
		Logger::Err( "ERROR: Could not allocate temporary ROM buffer" );
		return false;
	}

	// Load rom into buffer
	fread( buffer, sizeof( uint8_t ), rom_size, f );
	fclose( f );

	switch ( buffer[0x0147] ) {
		case 0x00: Logger::Log( "ROM uses no mappers" ); break;
		case 0x01: Logger::Log( "ROM uses MBC1" ); break;
		case 0x02: Logger::Log( "ROM uses MBC1 + RAM" ); break;
		case 0x03: Logger::Log( "ROM uses MBC1 + RAM + Battery" ); break;
		default: Logger::Err( std::format( "ERROR: ROM uses un supported mapper: {:02X}", buffer[0x0147] ) ); return false;
	}

	// Fetch ROM Bank Count
	switch ( buffer[0x0148] ) {
		case 0x00: rom_bank_count = 2; break;
		case 0x01: rom_bank_count = 4; break;
		case 0x02: rom_bank_count = 8; break;
		case 0x03: rom_bank_count = 16; break;
		case 0x04: rom_bank_count = 32; break;
		case 0x05: rom_bank_count = 64; break;
		case 0x06: rom_bank_count = 128; break;
		case 0x07: rom_bank_count = 256; break;
		case 0x08: rom_bank_count = 512; break;
		case 0x52: rom_bank_count = 72; break;
		case 0x53: rom_bank_count = 80; break;
		case 0x54: rom_bank_count = 96; break;
		default:   rom_bank_count = 0; break;
	}

	Logger::Log( std::format( "Rom file has {:d} rom banks", rom_bank_count ) );

	current_rom_bank = 1;

	if ( rom_bank_count == 0 ) {
		free( buffer );
		Logger::Err( "ERROR: ROM file reports 0 ROM banks..." );
		return false;
	}

	// Make sure ROM file size matches the number of rom banks stated
	if ( rom_size > ( ROM_BANK_SIZE * rom_bank_count ) ) {
		free( buffer );
		Logger::Err( "ERROR: ROM file is larger than its reported size..." );
		return false;
	}

	// Allocate Cartridge ROM
	ROM = ( uint8_t* ) calloc( ROM_BANK_SIZE * rom_bank_count, sizeof( uint8_t ) );
	if ( !ROM ) {
		free( buffer );
		Logger::Err( "ERROR: Could not allocate temporary ROM buffer" );
		return false;
	}
	

	// Load ROM file into cartridge ROM
	for ( int i = 0; i < rom_size; i++ ) {
		ROM[i] = buffer[i];
	}

	// Fetch RAM Bank Count
	switch ( buffer[0x0149] ) {
		case 0x02: ram_bank_count =  1; break;	//   8 KB
		case 0x03: ram_bank_count =  4; break;	//  32 KB
		case 0x04: ram_bank_count = 16; break;	// 128 KB
		case 0x05: ram_bank_count =  8; break;	//  64 KB
		default:   ram_bank_count =  0; break;	// No RAM
	}
	Logger::Log( std::format( "Rom file has {:d} ram banks", ram_bank_count ) );

	current_ram_bank = 0;
	ram_active = false;

	if ( ram_bank_count != 0 ) {
		RAM = ( uint8_t* ) calloc( RAM_BANK_SIZE * ram_bank_count, sizeof( uint8_t ) );
		if ( !RAM ) {
			free( buffer );
			Logger::Err( "ERROR: Could not allocate temporary ROM buffer" );
			return false;
		}
	}

	free( buffer );
	Logger::Log( "ROM loaded without errors" );
	return true;
}