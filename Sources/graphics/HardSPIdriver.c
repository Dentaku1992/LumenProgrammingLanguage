#include "HardSPIdriver.h"
#include <system.h>

//--------------------------------------------------
// Hardware SPI driver
//--------------------------------------------------

void hs_initSPI()
{
	trisc = 0b00011000; // Setting correct I/O's
	trisa = 0b00100000;

	intcon.7 = 1;
	intcon.6 = 1;
	pir1.3 = 0;
	pie1.3 = 1;
	ipr1.3 = 1;

	portc.0 = 1;

	sspcon1.5 = 1; // Enables MSSP module

	sspcon1.3 = 0; // Sets SPI slave mode, with slave slave select (slave select = RA5)
	sspcon1.2 = 1;
	sspcon1.1 = 0;
	sspcon1.0 = 0;
	sspcon1.4 = 0; // Sets clock polarity to idle low
	sspcon1.6 = 1; // Receive new byte while old is still in buffer
	sspcon1.7 = 0; // Disables collision detection,
				   // only needed in master mode but done for safety
	sspstat.7 = 0; // Datasheet says the SMP bit must be cleared in slave mode
	sspstat.6 = 0; // Data is latched when CLK goes from idle to active
	sspstat.0 = 0; // No data received yet, poll SSPSTAT.0 for received data

	pie1.3 = 1; //Enables MSSP interrupt
}

bool hs_sendByte(unsigned char data)
{
	sspbuf = data;
	_asm nop
	return sspcon1.7;
}

unsigned char hs_readByte()
{
	unsigned char data;
	data = sspbuf;
	sspbuf = 0x00; // Write dummy data to avoid setting of overflow flag
	sspstat.0 = 0;
	pir1.3 = 0;
	return data;
}

bool hs_dataAvailable()
{

	return sspstat.0;
}

void hs_reset()
{
	//resets collision detect and overflow flags
	sspcon1.7 = 0;
	sspcon1.6 = 0;
}