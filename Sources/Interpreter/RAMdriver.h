#include <system.h> 

//--------------------------------------------------
//HEADERS
//--------------------------------------------------

/// <summary> 
/// 
/// </summary> 
void rd_initRAM();

/// <summary> 
/// This function selects a memorybank. 
/// </summary> 
/// <param name="bank">
/// The bank parameter contains the selected bank: PROGMEM = 0, DATAMEM = 1
/// </param> 
void rd_setBank(unsigned char bank);

/// <summary> 
/// Function to get the selected bank
/// </summary> 
/// <returns> 
/// The method returns a unsigned char containing the selected bank
/// </returns> 
unsigned char rd_getBank();

/// <summary> 
/// Function to write data (1 byte) to the external RAM in a given address
/// </summary> 
/// <param name="address">
/// The address parameter requires an 16 bit address (unsigned short)
/// </param> 
/// <param name="value">
/// Parameter value is helds the value that needs to be stored in the ROM 
/// </param> 
void rd_writeToRAM(unsigned short address, unsigned char value); 

/// <summary> 
/// Function the read data (1 byte) from a given address
/// </summary> 
/// <param name="address">
/// The address parameter requires an 16 bit address (unsigned short). 
/// </param> 
/// <returns> 
/// The method returns a unsigned char containing the data in the address
/// </returns> 
unsigned char rd_readFromRAM(unsigned short address);