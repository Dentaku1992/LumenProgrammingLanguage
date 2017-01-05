#include "CommandControl.h"
#include "GraphicsControl.h"
#include "HardSPIdriver.h"
#include "Eventhandler.h"

unsigned char buffer[10] = {0};
unsigned char bufferPtr = 0;

void cc_initCommandControl()
{
	hs_initSPI();
}

void cc_interrupt(void)
{
	eventhandler();
}

void cc_spiCallback()
{
	buffer[bufferPtr++] = hs_readByte();
	portd = buffer[bufferPtr-1];
}

void cc_control()
{
	if (bufferPtr >= 1)
	{
		switch(buffer[0])
		{
			case 0x10:
				if(bufferPtr == 10)
				{
					gc_loadSprite(buffer[1], &(buffer[2]));
					bufferPtr = 0;
				}
				break;
			case 0x12:
				if(bufferPtr == 2)
				{
					gc_loadMap(buffer[1]);
					bufferPtr = 0;
				}
				break;
			case 0x14:
				if(bufferPtr == 2)
				{
					gc_loadString(buffer[1]);
					bufferPtr = 0;
				}
				break;
			case 0x16:
				if(bufferPtr == 5)
				{
					gc_setSprite(buffer[1], buffer[2], buffer[3], buffer[4]);
					bufferPtr = 0;
				}
				break;
			case 0x18:
				if(bufferPtr == 4)
				{
					gc_updateSprite(buffer[1], buffer[2], buffer[3]);
					bufferPtr = 0;
				}
				break;
			case 0x1A:
				if(bufferPtr == 2)
				{
					gc_clearSprite(buffer[1]);
					bufferPtr = 0;
				}
				break;
			case 0x1C:
				if(bufferPtr == 4)
				{
					gc_drawMap(buffer[1], buffer[2], buffer[3]);
					bufferPtr = 0;
				}
				break;
			case 0x1E:
				if(bufferPtr == 4)
				{
					gc_drawString(buffer[1], buffer[2], buffer[3]);
					bufferPtr = 0;
				}
				break;
			case 0x20:
				gc_clearAll();
				bufferPtr = 0;
				break;
			case 0x22:
				gc_clearGraphics();
				bufferPtr = 0;
				break;
			case 0x24:
				gc_clearStrings();
				bufferPtr = 0;
				break;
			case 0x26:
				gc_redraw();
				bufferPtr = 0;
				break;
			case 0x28:
				gc_allOn();
				bufferPtr = 0;
				break;
			case 0x2A:
				gc_allOff();
				bufferPtr = 0;
				break;
			case 0x2C:
				gc_invertedMode();
				bufferPtr = 0;
				break;
			case 0x2E:
				gc_normalMode();
				bufferPtr = 0;
				break;
			default:
				break;
		}
	}
}
