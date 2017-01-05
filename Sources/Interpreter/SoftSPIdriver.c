#include "SoftSPIdriver.h"

#define SOFT_SPI_CLK 			portd.0
#define SOFT_SPI_SDI 			portd.2
#define SOFT_SPI_SDO 			portd.1

//--------------------------------------------------
// Software SPI driver
//--------------------------------------------------

void ss_initSPI()
{
	SOFT_SPI_SDO = 0;
	SOFT_SPI_CLK = 0;
}

void ss_sendByte(unsigned char data)
{
	portc = data;
	//delay_ms(200);
	for(unsigned char i = 0; i < 8; i++)
	{
		SOFT_SPI_SDO = data.7;
		//delay_ms(1);
		SOFT_SPI_CLK = 1;
		delay_us(1);
		data <<= 1;
		SOFT_SPI_CLK = 0;
		SOFT_SPI_SDO = data.7;
		delay_us(1);
	}
}

unsigned char ss_readByte()
{
	unsigned char data = 0;
	for(unsigned char i = 0; i < 8; i++)
	{
		SOFT_SPI_CLK = 1;
		data += ((SOFT_SPI_SDI && 1) << (7-i));
		SOFT_SPI_CLK = 0;
	}
	return data;
}