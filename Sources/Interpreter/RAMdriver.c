#include "RAMdriver.h"
#include <system.h>

//--------------------------------------------------
//DEFINES
//--------------------------------------------------
#define PROGMEM 0
#define DATAMEM 1

//--------------------------------------------------
//VARIABLES
//--------------------------------------------------

unsigned char selected_bank; // This parameter contains the selected bank. PROGMEM = 0, DATAMEM = 1

//--------------------------------------------------
//RAM OPERATIONS
//--------------------------------------------------

void rd_setBank(unsigned char bank)
{
	selected_bank = bank;
}

unsigned char rd_getBank()
{
	//returns the selected bank
	return selected_bank;
}

void rd_initRAM()
{
	unsigned char selected_bank = PROGMEM;
	/*
	trisb = 0; //ADDRESS MSB (BIT 9 TO 16)
	trisc = 0; //ADDRESS LSB (BIT 1 TO 8)
	trisd = 0xff; //DATA (8 BIT) IN/OUT --> SETUP = IN;
	trise.0 = 0; //BANK SELECTION - BIT 17
	*/
	trisb = 0x00; //ADDRESS
	trisc = 0xFF; //DATA
}

void rd_writeToRAM(unsigned short address, unsigned char value)
{
	trisd = 0; //DATA OUT
	if(selected_bank == PROGMEM)
	{
		porte.0 = PROGMEM;
	}
	else //DATAMEM
	{
		porte.0 = DATAMEM;
	} 
	portb = address >> 8; 
	portc = address & 255;
	portd = value; 
	trisd = 0xff;
}

unsigned char rd_readFromRAM(unsigned short address)
{
/*
	if(selected_bank == PROGMEM)
	{
		porte.0 = PROGMEM;
	}
	else //DATAMEM
	{
		porte.0 = DATAMEM;
	}
	portb = address >> 8; 
	portc = address & 255;
	return (char)portd;
	*/
	//delay_ms(200);
	//portb = (unsigned char)(address/*&0x00FF*/);
	return (unsigned char)portc;
}