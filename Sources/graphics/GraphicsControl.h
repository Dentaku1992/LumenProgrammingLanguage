#ifndef GRAPHICSCONTROL_H
#define GRAPHICSCONTROL_H

//--------------------------------------------------
// Drawing on VRAM
//--------------------------------------------------

/// <summary>
///	Draws a pixel with a specified color at a specified location on the screen
/// </summary>
/// <param name="xpos">
/// Unsigned char denoting the x position on the screen
/// </param>
/// <param name="ypos">
/// Unsigned char denoting the y position on the screen
/// </param>
/// <param name="color">
/// Unsigned char denoting the color of the pixel
/// </param>
void gc_drawPixel(unsigned char xpos, unsigned char ypos, unsigned char color);

/// <summary>
///	Draws a pixel with color = 1 at a specified location on the screen
/// </summary>
/// <param name="xpos">
/// Unsigned char denoting the x position on the screen
/// </param>
/// <param name="ypos">
/// Unsigned char denoting the y position on the screen
/// </param>
void gc_setPixel(unsigned char xpos, unsigned char ypos);

/// <summary>
///	Draws a pixel with color = 0 at a specified location on the screen
/// </summary>
/// <param name="xpos">
/// Unsigned char denoting the x position on the screen
/// </param>
/// <param name="ypos">
/// Unsigned char denoting the y position on the screen
/// </param>
void gc_clearPixel(unsigned char xpos, unsigned char ypos);

/// <summary>
///	Draws 8 subsequent pixels starting at a specified location on the screen
/// The folowing pixels are each drawn below the previous drawn pixel (ie. advancing in y position)
/// If the end of the screen is reached, the rest of the pixels is not drawn
/// </summary>
/// <param name="xpos">
/// Unsigned char denoting the starting x position on the screen
/// </param>
/// <param name="ypos">
/// Unsigned char denoting the starting y position on the screen
/// </param>
/// <param name="data">
/// Unsigned char containing the pixel data, is used lsb first
/// </param>
void gc_drawByte(unsigned char xpos, unsigned char ypos, unsigned char data);

/// <summary>
///	Test the number bit of a certain byte data
/// </summary>
/// <param name="data">
/// An unsigned char denoting the data byte we want a to test a bit from
/// </param>
/// <param name="number">
/// The nth bit from the data byte we want to test, with nth being equal to number
/// </param>
/// <returns>
/// Unsigned char, in fact return is restricted to 0 or 1
/// </returns>
unsigned char gc_testbit(unsigned char data, unsigned char number);

//--------------------------------------------------
// Sprite
//--------------------------------------------------

/// <summary>
///	This method gets the pixel data for a sprite from the SPI receive buffer and puts it in the correct memory slot. The memory slot is given by the spritenumber.
/// </summary>
/// <param name="refNumber">
/// The number of the sprite in memory.
/// </param>
/// <param name="data">
/// Takes a pointer to the start of the pixel data for the sprite, by definition a sprite has 8 bytes of pixeldata, they are found subsequent at the pointer location.
/// </param>
void gc_loadSprite(unsigned char refNumber, unsigned char *data);

/// <summary>
///	Enables one to draw a sprite on the screen by taking a reference from the sprite in memory and mapping it to a sprite on the screen. It can be set at any location on the screen, if the sprite position is out of screen boundries nothing will be drawn.
/// Correct sprite numbers are from 1 upto 32, which means a maximum of 32 sprites can be drawn on the screen at once. One sprite in memory can have multiple entries in the list with sprites to draw on the screen.
/// </summary>
/// <param name="refNumber">
/// This is the number of the sprite in memory. This is also the reference to the actual sprite pixel data.
/// </param>
/// <param name="listNumber">
/// This is the number of the sprite on the screen. Can be different from the number of the sprite in memory.
/// </param>
/// <param name="xpos">
/// An unsigned char denoting the X postion on the screen. This means the left edge.
/// </param>
/// <param name="ypos">
/// An unsigned char denoting the Y postion on the screen. This means the top edge.
/// </param>
void gc_setSprite(unsigned char refNumber, unsigned char listNumber, unsigned char xpos, unsigned char ypos);

/// <summary>
///	Removes a sprite from the list with sprites to be drawn on the screen.
/// </summary>
/// <param name="listNumber">
/// The number of the sprite on the screen to be removed.
/// </param>
void gc_clearSprite(unsigned char listNumber);

/// <summary>
///	Changes a sprite to a new postion on the screen.
/// </summary>
/// <param name="listNumber">
/// The number of the sprite on the screen from which the position will be changed.
/// </param>
/// <param name="xpos">
/// An unsigned char denoting the new X postion on the screen. This means the left edge.
/// </param>
/// <param name="ypos">
/// An unsigned char denoting the new Y postion on the screen. This means the top edge.
/// </param>
void gc_updateSprite(unsigned char listNumber, unsigned char xpos, unsigned char ypos);

//--------------------------------------------------
// Map
//--------------------------------------------------

/// <summary>
///	Method to receive the data for a map. A map exist of tiles who are in fact sprites. A map is 16 tiles wide and 8 tiles high, which correspond to 128 x 64 pixels. A map is tiled from left to right and then from top to bottom. This method expects that 128 bytes with a reference to a sprite are send over SPI, after entering the method.
/// </summary>
/// <param name="mapNumber">
/// Denotes where to write the map data on receiving. The possible maps are from 0 upto 3.
/// </param>
void gc_loadMap(unsigned char mapNumber);

/// <summary>
///	This method send the visible part of the map to the VRAM. Because the screen is only 84 x 48 pixels and the map is 128 x 64 pixels one needs to specify the visible part. The visible part is selected with the X and Y offsets.
/// </summary>
/// <param name="mapNumber">
/// Tells the method from which map the part should be draw.
/// </param>
/// <param name="xoffset">
/// An unsigned char denoting the starting X postion on the map. This means the left edge of the screen.
/// </param>
/// <param name="yoffset">
/// An unsigned char denoting the starting Y postion on the map. This means the top edge of the screen.
/// </param>
void gc_drawMap(unsigned char mapNumber, unsigned char xoffset, unsigned char yoffset); // offset in pixels

//--------------------------------------------------
// String
//--------------------------------------------------

/// <summary>
///	This method loads a string into the string memory. The stringnumber is the reference for future usage of the string. A string ends with a NULL character or when the string memory is full, which is 254 bytes.
/// </summary>
/// <param name="stringNumber">
/// The stringnumber reference.
/// </param>
void gc_loadString(unsigned char stringNumber);

/// <summary>
///	This method draws a string onto the screen. It does so by writing the TRAM, so text can't conflict with graphics.
/// </summary>
/// <param name="xpos">
/// An unsigned char denoting the X postion on the screen. The text starts at this position an continues to the right.
/// </param>
/// <param name="ypos">
/// An unsigned char denoting the Y postion on the screen. This means the top line of the text.
/// </param>
/// <param name="stringNumber">
/// Specifies the string from memory to be draw on the screen.
/// </param>
void gc_drawString(unsigned char xpos, unsigned char ypos, unsigned char stringNumber);

//--------------------------------------------------
// LCD commands
//--------------------------------------------------

/// <summary>
///	Initializes the LCD module.
/// This means sending commands to the LCD for setting backplane voltage, contrast ...
/// It also initializes the software SPI internally.
/// </summary>
void gc_initLCD();

/// <summary>
///	Sets X and Y address of VRAM to zero in the LCD module.
/// </summary>
void gc_resetCursor();

/// <summary>
///	Selects the datamode of the LCD, in which pixeldata can be sent to it.
/// </summary>
void gc_dataMode();

/// <summary>
///	Selects the commandmode of the LCD, in which commands can be sent to it.
/// </summary>
void gc_commandMode();

/// <summary>
///	Sends a commandbyte to the lcd.
/// </summary>
/// <param name="command">
/// An unsigned char denoting the command byte to send.
/// </param>
void gc_sendCommand(unsigned char command);

/// <summary>
///	Sends a byte containing data for 8 pixels.
/// </summary>
/// <param name="data">
/// An unsigned char denoting the pixel data to send.
/// </param>
void gc_sendData(unsigned char data);

/// <summary>
///	Sends the whole VRAM and TRAM from the controller to the LCD module.
/// </summary>
void gc_redraw();

/// <summary>
///	Sends a command to turn on all the pixels of the LCD.
/// Can be used to test the LCD for defects.
/// </summary>
void gc_allOn();

/// <summary>
///	Sends a command to turn off all the pixels of the LCD.
/// </summary>
void gc_allOff();

/// <summary>
///	Sends a command to enable the inverted pixelmode of the LCD.
/// This inverts each and every pixel on the screen.
/// </summary>
void gc_invertedMode();

/// <summary>
///	Sends a command to disable the inverted pixelmode.
/// </summary>
void gc_normalMode();

/// <summary>
///	Clears both the VRAM and TRAM by filling them with zeros.
/// </summary>
void gc_clearAll();

/// <summary>
///	Clears the VRAM by filling it with zeros.
/// </summary>
void gc_clearGraphics();

/// <summary>
///	Clears the TRAM by filling it with zeros.
/// </summary>
void gc_clearStrings();

#endif