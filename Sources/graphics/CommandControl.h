#ifndef COMMANDCONTROL_H
#define COMMANDCONTROL_H

/// <summary>
///	This initializes the command control, calls the needed methods to set some registers
/// </summary>
void cc_initCommandControl();

/// <summary>
///	Contains the code that must be executed on SPI interrupt
/// This interrupt is triggered on receiving a full byte
/// </summary>
void cc_spiCallback();

/// <summary>
///	This method does all the command handling
/// It checks the receive buffer and calls the correct middleware methods
/// </summary>
void cc_control();

#endif