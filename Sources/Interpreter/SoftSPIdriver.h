#ifndef SOFTSPIDRIVER_H
#define SOFTSPIDRIVER_H

#include <system.h>

//--------------------------------------------------
// Software SPI driver
//--------------------------------------------------

/// <summary>
/// Initialisation of the software SPI pins.
/// Sets all the output pins low.
/// Mind the tris register, this has to be set manually!
/// The standard in this case is trisa = 0b00000100;
/// </summary>
void ss_initSPI();

/// <summary>
/// Sends a byte over SPI to the selected slave.
/// </summary>
/// <param name="data">
/// The data argument is an unsigned char, the byte we want to send.
/// </param>
void ss_sendByte(unsigned char data);

/// <summary>
/// Reads a byte over SPI from the selected slave.
/// </summary>
/// <returns>
/// The method returns an unsigned char, the data received over SPI.
/// </returns>
unsigned char ss_readByte();

#endif