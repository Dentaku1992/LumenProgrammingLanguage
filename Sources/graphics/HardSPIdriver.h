#ifndef HARDSPIDRIVER_H
#define HARDSPIDRIVER_H

//--------------------------------------------------
// Hardware SPI driver
//--------------------------------------------------

/// <summary>
/// Initialisation of the hardware SPI pins.
/// Sets all the registers needed for a correct working SPI peripheral
/// Also sets the registers needed for interrupt callback
/// </summary>
void hs_initSPI();

/// <summary>
/// Sends a byte over SPI to the selected slave.
/// </summary>
/// <param name="data">
/// The data argument is an unsigned char, the byte we want to send.
/// </param>
/// <returns>
/// The method returns a boolean, this denotes the if the operation was succesfull.
/// </returns>
bool hs_sendByte(unsigned char data);

/// <summary>
/// Reads a byte over SPI from the selected slave.
/// </summary>
/// <returns>
/// The data argument is an unsigned char, the byte we want to send.
/// </returns>
unsigned char hs_readByte();

/// <summary>
/// Check whether there is new data available since the last read or write operation.
/// </summary>
/// <returns>
/// Returns a boolean, denoting the availability of new data.
/// </returns>
bool hs_dataAvailable();

/// <summary>
/// Method for resetting the SPI module after a collision or overflow has happened.
/// Does not clear the buffer!
/// </summary>
void hs_reset();

#endif