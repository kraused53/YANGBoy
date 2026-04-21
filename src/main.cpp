#include "core/Bus.h"

#include <iostream>

int main( void ) {
	Bus system;

	//system.cart.load_rom( "./roms/test/01-special.gb" );
	system.cart.load_rom( "./roms/test/02-interrupts.gb" );
	//system.cart.load_rom( "./roms/test/03-op sp,hl.gb" );
	//system.cart.load_rom( "./roms/test/04-op r,imm.gb" );
	//system.cart.load_rom( "./roms/test/05-op rp.gb" );
	//system.cart.load_rom( "./roms/test/06-ld r,r.gb" );
	//system.cart.load_rom( "./roms/test/07-jr,jp,call,ret,rst.gb" );
	//system.cart.load_rom( "./roms/test/08-misc instrs.gb" );
	//system.cart.load_rom( "./roms/test/09-op r,r.gb" );
	//system.cart.load_rom( "./roms/test/10-bit ops.gb" );
	//system.cart.load_rom( "./roms/test/11-op a,(hl).gb" );

	for( int i = 0; i < 10000000; i++ ) {
		system.clock();
	}

    return 0;
}
