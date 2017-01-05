/***************************************************
This file contains functions that handle events.

The Global interrupt enable is switched off in hardware!
The interrupt flags are checked.

When a flag is on,
the correct driver eventhandler
must be called.

The correct flag is cleared.
The global interrupt is set back automatically by the hardware.

Creation date: 24 november 2009
Author: JJ Vandenbussche KHBO Dept. IW&T
Revision history:
08 dec 2009: removed switching the GIE and checking PEIE
		PIC switches GIE(H)(L) off when it handles the interrupt
		and switches it back on afterwards
		Since PEIE is shared with GIEL, it can also be switched off

***************************************************/

/**************************************************
KEEP ALL INTERRUPT RELATED STUFF IN THE EVENTHANDLER.
ALL INTERRUPT RELATED BITS MUST BE CHANGED HERE AND
NOWHERE ELSE
**************************************************/

#include "Eventhandler.h"

void eventhandler(void)
{
	//check which source generated the interrupt
	//The peripheral interrupts-----------------------------
	/*
	if (pir1.PSPIF && pie1.PSPIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir1,PSPIF);
	}
	else if (pir1.ADIF && pie1.ADIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir1,ADIF);
	}
	else if (pir1.RCIF && pie1.RCIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir1,RCIF);
	}
	else if (pir1.TXIF && pie1.TXIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir1,TXIF);
	}
	else */if (pir1.SSPIF && pie1.SSPIE)
	{
		//call the correct driver eventhandler
		portc.0 = 0;
		cc_spiCallback();
		clear_bit(pir1,SSPIF);
	}/*
	else if (pir1.CCP1IF && pie1.CCP1IE)
	{
		//call the correct driver eventhandler

		clear_bit(pir1,CCP1IF);
	}
	else if (pir1.TMR2IF && pie1.TMR2IE)
	{
		//call the correct driver eventhandler

		clear_bit(pir1,TMR2IF);
	}
	else if (pir1.TMR1IF && pie1.TMR1IE)
	{
		//call the correct driver eventhandler

		clear_bit(pir1,TMR1IF);
	}
	else if (pir2.OSCFIF && pie2.OSCFIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir2,OSCFIF);
	}
	else if (pir2.CMIF && pie2.CMIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir2,CMIF);
	}
	else if (pir2.EEIF && pie2.EEIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir2,EEIF);
	}
	else if (pir2.BCLIF && pie2.BCLIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir2,BCLIF);
	}
	else if (pir2.HLVDIF && pie2.HLVDIE)
	{
		//call the correct driver eventhandler

		clear_bit(pir2,HLVDIF);
	}
	else if (pir2.TMR3IF && pie2.TMR3IE)
	{
		//call the correct driver eventhandler

		clear_bit(pir2,TMR3IF);
	}
	else if (pir2.CCP2IF && pie2.CCP2IE)
	{
		//call the correct driver eventhandler

		clear_bit(pir2,CCP2IF);
	}
	//END OF the peripheral interrupts----------------------

	//Internal interrupts
	if (intcon.TMR0IE && intcon.TMR0IF)
	{
		//call the correct driver eventhandler

		clear_bit(intcon,TMR0IF);
	}
	else if (intcon.INT0IE && intcon.INT0IF)
	{
		//call the correct driver eventhandler

		clear_bit(intcon,INT0IF);
	}
	else if (intcon.RBIE && intcon.RBIF)
	{
		//call the correct driver eventhandler

		clear_bit(intcon,RBIF);
	}
	else if (intcon3.INT2IE && intcon3.INT2IF)
	{
		//call the correct driver eventhandler

		clear_bit(intcon3,INT2IF);
	}
	else if (intcon3.INT1IE && intcon3.INT1IF)
	{
		//call the correct driver eventhandler

		clear_bit(intcon3,INT1IF);
	}*/
	return;
}
