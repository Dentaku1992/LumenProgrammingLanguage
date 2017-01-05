#include <system.h>
#include "SoftSPIdriver.h"
#include "HardSPIdriver.h"
#include "graphicsControl.h"

#define LCD_RESETPIN 			portb.3
#define LCD_DATACOMMANDPIN 		portb.5
#define LCD_SLAVESELECT 		portb.4


//--------------------------------------------------
// Memory for different sprites and maps
//--------------------------------------------------

//--------------------------------------------------
// Memory for sprites
//--------------------------------------------------
unsigned char spritesInMemory[256] = {0};
unsigned char spritesOnScreen[3 * 32] = {0};

//--------------------------------------------------
// Memory for maps
//--------------------------------------------------
unsigned char map_0[128] = {0};
unsigned char map_1[128] = {0};
unsigned char map_2[128] = {0};
unsigned char map_3[128] = {0};
unsigned char *maps[] = {map_0, map_1, map_2, map_3};

//--------------------------------------------------
// Memory for strings
//--------------------------------------------------
unsigned char strings[32] = {0};
unsigned char stringData[256] = {0};
unsigned char stringDataPtr = 0;

//--------------------------------------------------
// VRAM
//--------------------------------------------------
unsigned char vram_0[252] = {0}; // Left half screen
unsigned char vram_1[252] = {0}; // Right half screen
unsigned char *vram[] = {vram_0, vram_1};

//--------------------------------------------------
// TRAM
//--------------------------------------------------
unsigned char tram_0[252] = {0}; // Left half screen
unsigned char tram_1[252] = {0}; // Right half screen
unsigned char *tram[] = {tram_0, tram_1};

//--------------------------------------------------
// FONT
//--------------------------------------------------
#include "Font5x7.h"

//--------------------------------------------------
// Drawing on VRAM
//--------------------------------------------------

void gc_drawPixel(unsigned char xpos, unsigned char ypos, unsigned char color)
{
	if(color.0 == 1)
	{
		gc_setPixel(xpos, ypos);
	}
	else
	{
		gc_clearPixel(xpos, ypos);
	}
}

void gc_drawByte(unsigned char xpos, unsigned char ypos, unsigned char data)
{
	for (unsigned char i = 0; i < 8; i++)
	{
		if (ypos+i <= 47 && xpos <= 83)
		{
			gc_drawPixel(xpos, ypos + i, gc_testbit(data,i));
		}
	}
}

unsigned char gc_testbit(unsigned char data, unsigned char number)
{
	return 0 < (data & 1<<number)? 1 : 0;
}

#pragma OPTIMIZE "0"
void gc_setPixel(unsigned char xpos, unsigned char ypos)
{

	set_bit(vram[xpos / 42][(xpos % 42) * 6 + ypos / 8], ypos%8);
}

#pragma OPTIMIZE "0"
void gc_clearPixel(unsigned char xpos, unsigned char ypos)
{

	clear_bit(vram[xpos / 42][(xpos % 42) * 6 + ypos / 8], ypos%8);
}

//--------------------------------------------------
// Sprite
//--------------------------------------------------

void gc_loadSprite(unsigned char refNumber, unsigned char data[])
{
	for (unsigned char i = 0; i < 8; i++)
	{
		spritesInMemory[(refNumber - 1) * 8 + i] = data[i];
	}
}

void gc_setSprite(unsigned char refNumber, unsigned char listNumber, unsigned char xpos, unsigned char ypos)
{
	spritesOnScreen[listNumber * 3 + 0] = refNumber;
	spritesOnScreen[listNumber * 3 + 1] = xpos;
	spritesOnScreen[listNumber * 3 + 2] = ypos;
}

void gc_clearSprite(unsigned char listNumber)
{
	spritesOnScreen[listNumber * 3 + 0] = 0;
	spritesOnScreen[listNumber * 3 + 1] = 0;
	spritesOnScreen[listNumber * 3 + 2] = 0;
}

void gc_updateSprite(unsigned char listNumber, unsigned char xpos, unsigned char ypos)
{
	unsigned char spriteNumber = spritesOnScreen[listNumber * 3] -1;
	spritesOnScreen[listNumber * 3 + 1] = xpos;
	spritesOnScreen[listNumber * 3 + 2] = ypos;
	for (unsigned char i = 0; i < 8; i++)
	{
		gc_drawByte(xpos + i, ypos, spritesInMemory[8 * spriteNumber + i]);
	}
}

//--------------------------------------------------
// Map
//--------------------------------------------------

void gc_loadMap(unsigned char mapNumber)
{
	unsigned char count = 0;
	while(count < 128)
	{
		if(hs_dataAvailable())
		{
			maps[mapNumber - 1][count] = 0;
			hs_readByte();
			count++;
		}
	}
}

void gc_drawMap(unsigned char mapNumber, unsigned char xoffset, unsigned char yoffset)
{
	unsigned char data = 0;
	unsigned char currentSprite;

	for (int x = 0; x < 84; x++)
	{
		for (int y = 0; y < 48; y++)
		{
			currentSprite = maps[mapNumber][((xoffset + x) / 8) * 8 + ((yoffset + y) / 8)];
			data = gc_testbit(spritesInMemory[currentSprite * 8 + (xoffset + x) % 8],((yoffset + y) % 8));
			gc_drawPixel(x, y, data);
		}
	}
}

//--------------------------------------------------
// String
//--------------------------------------------------

void gc_loadString(unsigned char stringNumber)
{
	strings[stringNumber] = stringDataPtr;

	unsigned char received = 1;
	while(received != "\0" && stringDataPtr < 255)
	{
		if(hs_dataAvailable())
		{
			received = hs_readByte();
			stringData[stringDataPtr] = received;
			stringDataPtr++;
		}
	}
}

void gc_drawString(unsigned char xpos, unsigned char ypos, unsigned char stringNumber)
{
	unsigned char textData = stringData[strings[stringNumber]];
	unsigned char ptr = 0;
	while(textData != '\0')
	{
		unsigned char fontBank = textData / 32;
		unsigned char i;
		switch(fontBank)
		{
			case 1:
				for (i = 0; i < 5; i++)
				{
					gc_drawByte(xpos + i, ypos, font_1[(textData % 32) * 5 + i]);
				}
				break;
			case 2:
				for (i = 0; i < 5; i++)
				{
					gc_drawByte(xpos + i, ypos, font_2[(textData % 32) * 5 + i]);
				}
				break;
			case 3:
				for (i = 0; i < 5; i++)
				{
					gc_drawByte(xpos + i, ypos, font_3[(textData % 32) * 5 + i]);
				}
				break;
			default:
				break;
		}
		xpos += 6;
		ptr++;
		textData = stringData[strings[stringNumber]+ptr];
	}
}

//--------------------------------------------------
// LCD commands
//--------------------------------------------------

void gc_initLCD()
{
	trisb = 0x00;
	LCD_SLAVESELECT = 0;
	ss_initSPI();
	LCD_RESETPIN = 0;
	delay_ms(250);
	delay_ms(250);
	LCD_RESETPIN = 1;

	gc_sendCommand(0x21);
	gc_sendCommand(0xE0);
	gc_sendCommand(0x04);
	gc_sendCommand(0x14);
	gc_sendCommand(0x22);
	gc_sendCommand(0x0C);
}

void gc_resetCursor()
{
	gc_sendCommand(0x40);
	gc_sendCommand(0x80);
}

void gc_dataMode()
{

	LCD_DATACOMMANDPIN = 1;
}

void gc_commandMode()
{

	LCD_DATACOMMANDPIN = 0;
}
void gc_sendCommand(unsigned char command)
{
	gc_commandMode();
	ss_sendByte(command);
}

void gc_sendData(unsigned char data)
{
	gc_dataMode();
	ss_sendByte(data);
}

void gc_redraw()
{
	gc_resetCursor();
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 252; j++)
		{
			gc_sendData((vram[i][j])^(tram[i][j]));
		}
	}
}
void gc_allOn()
{

	gc_sendCommand(0x09);
}

void gc_allOff()
{

	gc_sendCommand(0x08);
}

void gc_invertedMode()
{

	gc_sendCommand(0x0D);
}

void gc_normalMode()
{

	gc_sendCommand(0x0C);
}

void gc_clearAll()
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 252; j++)
		{
			vram[i][j] = 0;
			tram[i][j] = 0;
		}
	}
	gc_redraw();
}

void gc_clearGraphics()
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 252; j++)
		{
			vram[i][j] = 0;
		}
	}
}

void gc_clearStrings()
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 252; j++)
		{
			tram[i][j] = 0;
		}
	}
}