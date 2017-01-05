#include <system.h>
#include "GraphicsControl.h"
#include "CommandControl.h"

// config registers left on default values, except:
// config registers left on default values, except:
#pragma DATA _CONFIG1H, _OSC_HS_1H
//HS OSCILLATOR
#pragma DATA _CONFIG2L, _PWRT_OFF_2L & _BOREN_OFF_2L
//POWER-UP TIMER DISABLED; BROWN OUT RESET DISABLED
#pragma DATA _CONFIG2H, _WDT_OFF_2H
//WATCHDOG TIMER DISABLED
#pragma DATA _CONFIG3H, _MCLRE_ON_3H & _PBADEN_OFF_3H
//MASTER CLEAR PIN ENABLED RE3 IS MASTER CLEAR; PORTB PINS DIGITAL
#pragma DATA _CONFIG4L, _DEBUG_OFF_4L & _LVP_OFF_4L & _XINST_OFF_4L
//BACKGROUND DEBUGGER OFF; LOW VOLT. PROG. OFF;
//EXTENDED INSTRUCTIONSET DISABLED (for lcd)
#pragma CLOCK_FREQ	8000000

void main()
{
	gc_initLCD();
	cc_initCommandControl();
	
	while(1)
	{
		cc_control();
	}
}