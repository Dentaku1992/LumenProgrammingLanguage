#ifndef IOCONTROL_H
#define IOCONTROL_H

#include <system.h>
#include "RAMdriver.h" 
#include "SoftSPIdriver.h"

//DEBUG
void io_initSprites(unsigned short startAddress, unsigned short amount);

//--------------------------------------------------
//HEADERS
//--------------------------------------------------

/// <summary> 
/// 
/// </summary> 
void io_initSlaveSelect();

/// <summary> 
/// This function reads 1 byte of data from the external program RAM chip at a given address. The program RAM contains the 
/// program code that is executed by the interpreter.
/// </summary> 
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// The program address range is from 0x00000 to 0x0FFFF.
/// </param> 
/// <returns> 
/// This method returns a unsigned char containing the data that was stored at the given address.
/// </returns> 
unsigned char io_readProgramMemory(unsigned short address);

/// <summary> 
/// This function reads 1 byte of data from the external data RAM chip at a given address. The data RAM contains the 
/// sprites, maps, strings, settings and savebanks. 
/// </summary> 
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// The data address range is from 0xF0000 to 0xFFFFF.
/// </param> 
/// <returns> 
/// This method returns a unsigned char containing the data that was stored at the given address.
/// </returns> 
unsigned char io_readDataMemory(unsigned short address);

/// <summary> 
/// This function writes 1 byte of data to the external data RAM at a given address.
/// </summary> 
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// The data address range is from 0xF0000 to 0xFFFFF.
/// </param> 
void io_writeToDataMemory(unsigned short address, unsigned char data);

/// <summary> 
/// This function reads the game controller buttons state for a given controller number.
/// </summary> 
/// <param name="ControllerNumber">
/// This parameter contains the controller number:
/// Controller1 = 0x00
/// Controller2 = 0xff
/// </param> 
/// <returns> 
/// The method returns a unsigned char containing the controller button state. The meaning of each bit:
/// BIT 0: A
/// BIT 1: B
/// BIT 2: SELECT
/// BIT 3: START
/// BIT 4: DPAD UP
/// BIT 5: DPAD DOWN
/// BIT 6: DPAD LEFT
/// BIT 7: DPAD RIGHT
/// </returns> 
unsigned char io_readController(unsigned char controllerNumber);

/// <summary> 
/// This function sets up the IOcontrol interface. It performs the following actions:
/// - RAM interface setup
/// - SoftSPI interface setup and slave select initialisation
/// - Loads the sprites into the video memory
/// - Loads the strings into the video memory
/// - Loads the maps into the video memory
/// </summary> 
/// <note>
/// This function has a long execution time.
/// </note>
void io_initIO();

/// <summary> 
/// This function lights up all the pixels of the LCD screen without overwriting the video memory.
/// </summary> 
void io_allOn();

/// <summary> 
/// This function disables all the pixels of the LCD screen without overwriting the video memory.
/// </summary> 
void io_allOff();

/// <summary> 
/// This function inverts all the pixels of the LCD screen overwriting the video memory. This means that a black pixel turns white and a white pixel 
/// turns into black. 
/// </summary> 
void io_invert();

/// <summary> 
/// This function sets the coordinates of the LCD display in the videobuffer. TOTO: EXPLAIN
/// The coordinate (0,0) is located top left.
/// </summary> 
/// <param name="x">
/// This parameter of type unsigned char contains the x-coordinate of the cursor. 
/// </param> 
/// <param name="y">
/// This parameter of type unsigned char contains the y-coordinate of the cursor. 
/// </param> 
void io_setCursor(unsigned char x , unsigned char y);

/// <summary> 
/// This function updates the position of a sprite in the video memory.
/// </summary> 
/// <param name="indexVram">
/// This parameter of type unsigned char container the index of the sprite in the video RAM.
/// </param> 
/// <param name="x">
/// This parameter of type unsigned char contains the x-coordinate of the cursor. 
/// </param> 
/// <param name="y">
/// This parameter of type unsigned char contains the y-coordinate of the cursor. 
/// </param> 
void io_updateSprite(unsigned char indexVram,unsigned char x, unsigned char y);

/// <summary> 
/// This function initialize a sprite in the video memory at a given position.
/// </summary> 
/// <param name="index">
/// This parameter of type unsigned char contains the index of the sprite in ROM memory.
/// </param> 
/// <param name="indexVram">
/// This parameter of type unsigned char container the index of the sprite in the video RAM.
/// </param> 
/// <param name="x">
/// This parameter of type unsigned char contains the x-coordinate of the cursor. 
/// </param> 
/// <param name="y">
/// This parameter of type unsigned char contains the y-coordinate of the cursor. 
/// </param> 
void io_setSprite(unsigned char index, unsigned char indexVram);

/// <summary> 
/// This function clears a sprite given by the index parameter in the video memory.
/// </summary> 
/// <param name="index">
/// This parameter of type unsigned char contains the index of the sprite in ROM memory.
/// </param> 
void io_clearSprite(unsigned char index);

/// <summary> 
/// This function draws a string given by the index parameter on the screen at a given position.
/// </summary> 
/// <param name="index">
/// This parameter of type unsigned char contains the index of the string in ROM memory.
/// </param>
/// <param name="x">
/// This parameter of type unsigned char contains the x-coordinate of the cursor. 
/// </param> 
/// <param name="y">
/// This parameter of type unsigned char contains the y-coordinate of the cursor. 
/// </param> 
void io_drawString(unsigned char index, unsigned char x, unsigned char y);

/// <summary> 
/// This function draws a map with a given index on the LCD screen.
/// </summary> 
/// <param name="index">
/// This parameter of type unsigned char contains the index of the map in ROM memory.
/// </param>
void io_drawMap(unsigned char index);

/// <summary> 
/// This function clears al the elements on the LCD screen. 
/// </summary> 
void io_clearAll();

/// <summary> 
/// This function clears the string buffer.
/// </summary> 
void io_clearStrings();

/// <summary> 
/// This function clears the display buffer. 
/// </summary> 
void io_clearGraphics();

/// <summary> 
/// This function redraws the displaybuffer to the LCD screen.
/// </summary> 
void io_redraw();

/// <summary> 
/// This function sets the LCD screen back to normal mode.
/// </summary> 
void io_normalMode();


#endif