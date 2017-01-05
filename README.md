#LUMEN 

LUMEN stands for Language For Ultraportable And Microcontroller-based Embedded eNvironments. It is an ultra lightweight programming language intended to design arcade games on embedded devices.

The core features of LUMEN are:
	- Easy to understand
	- Small memory footprint on any platform
	- Minimal number of opcodes

##Overview of the project

The idea of the project is to build a basic game console. This console interprets
the LUMEN programming language which is also part of the project. The project is
divided in two separate parts.

The first part is developing an interpreted programming language, with an accompagnying IDE and compiler with sprite editor and map designer. The tools willbe considered as already existing, since these do not contribute to embedded programming.The entire flow of the program, using inputs and providing output, is written in the interpreted code. The preprogrammed code in the PIC is merely a framework to enable the interpreter to execute the full range of functions available to the programmer.

In the second part, a game console is constructed. This console is built upon a PIC18F4620 microcontroller which runs an interpreter. A monochrome display is provided to give the game graphical output. Inputs are possible by two separate handheld controllers and thus multiplayer with up to 2 players is supported. The Games are stored in replaceable game cartridges to give a maximum flexibility. The goal of the project is to make a new programming language and gaming console with the following features:

- An interpreter running on the PIC18F4620
- Graphical output to a monochrome display
- Input from two handheld controllers
- Multiplayer support with a maximum of two controllers
- Replaceable ROM game cards (cartridge system)

##The game console

The console has some specific features that will be described here. All of the aspects of hardware requirements, memory usage, input and output... will be explained. Based on the image below, all requirements and specifications will be defined, from the controller to the interpreter to the graphics control. Any of these blocks is a stand-alone configuration, but the interpreter and the graphics control compose the actual console.

![Game console overview]()

### Controller
A total of 8 push buttons are present on the controller, each with a pressed and a released state. When the user presses a button the state changes form released to pressed and vice versa. The controller uses an SPI interface to allow communication with the game. Multiplayer is possible when connecting 2 controllers at once.

####Buttons

#####D-Pad
Four of the buttons at the left side of the controller are formed as a D-Pad, a flat thumb-operated four-way directional control with one push button on each point, providing intuitive direction and steering capabilities, and should be used as such.

#####Select and start
In the center, the select and start button are located. These should be used to start the game, pause it, allow menu navigation etc. The start button should allow pausing the game at any point, pressing start again returns to the game.

#####A and B
Next to the select and start buttons, A and B are located. These two buttons are to be used in-game, providing interaction with objects on the screen. They should not be used to pause or unpause the game at any time.

####Interface
The controller connects through a three-wire interface similar to SPI. A maximum of two controllers can be connected, addressed as 0 and 1. The timing can be seen in this view (clockperiod of 6µs, from top to bottom: Latch, Clock, Data):

![Timing Diagram]()


###Interpreter

The interpreter is the main unit of the console. It executes the code and sends commands to the graphics controller to ensure the data displayed on screen is correct. Since the code is stored in external cartridge memory in the form of bytecode, specific requirement should be met to obtain accurate behaviour:
- MCU: PIC18F4620 (8-bit, 8 MHz)
- RAM: 3968 bytes - 256 bytes can be used by one game
- Input: NES controller (max 2)

####Peripherals

#####Registers
Data transfer and manipulation is done via registers. These are memory locations that can be used to perform arithmetics on, used as delay counter. . . They can be used as data storage, but the RAM is a better place for this. In total, 16 8-bit wide registers can be used, yet one should be used with caution since it will be overwritten by some specific instructions. Registers 0 to 14 are free to use, and will only be overwritten when the code tells the interpreter to do so. The 15th register, called the HL register, is used by the interpreter as data buffer for e.g. reading a controller or timer.

#####Stack
The stack is used to temporarly store data when the interpreter executes a subroutine or a register should be stored for later use. It is a LIFO structure of 256 items long, each item being 8 bit wide. The stack pointer is 8 bit too, but cannot be manipulated by the user directly: push and pop instructions

#####Program counter
The program counter is a pointer to where the interpreter is in the code. It is a 16 bit unsigned short, starting at 0 for the first instruction. Each instruction or operand

increments this program counter. The program counter cannot be manipulated by the user directly, but should go through the jump, call or return instructions of the interpreter. A jump instruction replaces the program counter by a new value, while the call instruction pushes the current program counter to the stack and then replaces the program counter by the new one. Return pops the program counter from the stack at the end of a subroutine, and replaces the current program counter by this value, which results in a return to the point of calling the subroutine.

#####RAM
RAM can be used to store data during the execution of a program, it is 256 items long, each item being 8 bits wide. It will never be overwritten by the interpreter automatically, yet overwriting can happen when the code tells the interpreter to do so. It is possible to exchange data from a register to a RAM address and the other way around. Data that should be stored over a longer period of time can be placed in RAM, and copied back to a register for later manipulation.

####Cartridge memory mapping
An important aspect of every game is the memory mapping on the cartridge. Every item (code, sprites, strings and maps) has its own specific start- and endaddress to establish a solid and correct performance. The mapping is as follows:

![Cartridge memory mapping]()

#####Code
This is the actual bytecode that will be interpreted and executed. An in-depth look on the instructionset can be found in chapter 2.1. The maximum codesize is 64kB, or 65536 bytes. If no jump is performed at byte 65535, the program counter will overflow, resulting in a reset of the program counter to zero. Code examples can be found in the appendix on page 67.

#####Sprites
A sprite is an 8 by 8 pixels bitmap in black and white. Each bit represents one pixel, resulting in a size of 8 bytes per sprite. The addressing starts at the top left, and goes down per colum, this happens to be 8 bits, and can thus be packed in one byte. The first colum is the first byte, the second colum the second byte. . .

#####Maps
A map is a combination of sprites that can fill the background of the screen. Placing a map on the screen can be functional, e.g. in a platformer game, or aesthetically. It is in fact a tiled field containing the indexes of sprites in the onscreen memory, each tile being 8 by 8 pixels (equal to 1 sprite).

#####Strings
Strings can be used to be drawn on screen as guidelines or instructions to the user. They are to be stored in the cartrigde memory as null-terminated ascii encoded strings, and will be read as such. If a string is not null-terminated, the interpreter will read until the first null is encountered, which can result in unexpected behaviour. A maximun total size of 10kB of strings can be loaded into the cartridge, yet that amount can never be displayed at once on the screen, since the string buffer is limited to 256 bytes. 

#####Savebanks
The savebanks are 10 address locations of each 1kB that can be used freely by the game. If playernames, highscores, achievements. . . should be stored, this is the location to do so. The game will not check if the data is legitimate or not, this is up to the programmer. Settings should not be stored in savebanks, for this has its own address location.

#####Splash screen
A splash screen is the screen that should be shown while all data is loaded and transfered between the interpreter and the graphics control. It has to be a black and white bitmap addressed from the top left down each colum. Since the screen is 84x48, a fullscreen bitmap is 4032 bits, or 504 bytes.

#####Settings
Settings can be stored in this address location. No restrictions are applied, and the content is up to the programmer.

###Graphics control
The graphics control is located entirely in a separate microcontroller. Its tasks in a nutshell are to drive the actual LCD display and to provide a simple way of using graphics in a game. To do so it provides a set of commands to the programmer that can be used to initialize, draw, move and remove spites on the screen. It also has support for text on screen and for maps that can be scrolled over the screen.

####The LCD display
The LCD display used in this setup has a resolution of 84x48 pixels. It has a PCD8544 chipset and is interfaced with SPI. The chipset does all the timing and scanning of the LCD. All we need to provide is a correct initialisation, fill the VRAM and send valid commands. This is where graphics control comes in. The graphics control translates SPI commands and data from the interpreter to SPI commands and data the LCD understands.

####Memories of the graphics control
The graphics control has several sort of memories. Although most of it is directly in ram, their purposes are completely different:

#####Sprites in memory
This part of the memory is used to store the actual data of a sprite, or the data for the visual representation of that particular sprite. It can contain up to 32 different sprites and a sprite is 8 by 8 pixels or 8 bytes in size. For an in-depth look on the format of sprites, please refer to chapter 1.3.3. Every sprite in memory can be referenced by its number in memory, as there are 32 possible sprites valid numbers are from 1 up to
32.

#####Sprites on screen
This part of the memory tell the graphics control what to draw where concerning sprites. It can contain up to 32 record. A record is 3 bytes big and represents a sprite (first byte), a X position on the screen (second byte) and a Y position on the screen (third byte). From now on a sprite can be drawn on the screen. And a sprite on the screen is referenced by a list number, that number is the number of the record containing information of the sprite data and position. Note that multiple records can use the same sprite.

#####Maps
This part of the memory is used for background maps. Up to four maps can be used. A map exists of tiles and these tiles are in fact just sprites. One must use the the number of the spite in memory to use it in a map. A map measures 16 by 8 tiles, or 128 by 64 pixels. For tiling the map a sequence of references to sprites is needed. A map is tiled from left to right and than from top to bottom. When drawing a map on the screen, one can give a X and Y offset, resulting in a scrollable background.

#####Strings
This part of the memory is used to store pointers to string data. A string starts at the pointer location and ends when a null character is encountered. A maximum of 32 strings is allowed. The number of the string is used for referencing when drawing it to the screen.

#####String data
This part of the memory is used to store the actual data for the strings. It can contain up to 256 characters. Therefore all the strings (including null characters) together can have a maximum size of 256 bytes. A new string starts where the old string ended. Therefore it is important to initialize the strings at the beginning. Otherwise conflicts of overwriting strings is a ever present danger.

#####Font
This font memory contains the data for the actual graphical representation of an ascii character on screen. A character is made out of five bytes, and is mapped onto a 5x7 font. This part is also the only part not in RAM, it is hard coded in the flash region of the microcontroller.

#####VRAM
The VRAM is the graphical buffer for the screen. If needed, the map is drawn first, after which sprites are drawn on top. Every graphical element except for strings are drawn in this buffer, which is then sent to the display where it resides in the display’s RAM until overwritten.

#####TRAM
The TRAM is the graphical buffer for the text to be drawn on screen. Through the font memory, characters are converted to bytes and draw in the TRAM. When the image is to be drawn to the display, the VRAM and TRAM buffers are XORed to ensure that twice a black pixel becomes white so that the image or character can still be seen.

####The normal operation
To obtain a correct functionality, the graphics control is initialized using the provided intializer. Once this is completed, commands and data can be sent to it. This is done though a hardware SPI interface and will be regarded as a command, optionally followed by some arguments. Once a command is received the routine waits for the
needed arguments, then calls the corresponding function. Correct commands are:

- __Load sprite:__ 0x10 Load a sprite in sprite memory
- __Load map:__ 0x12 Load a map in map memory
- __Load string:__ 0x14 Load a string in string memory
- __Set sprite:__ 0x16 Initialize a sprite in sprites on screen memory
- __Update sprite:__ 0x18 Draw a sprite on the screen at a certain position
- __Clear sprite:__ 0x1A Remove a sprite from the screen
- __Draw map:__ 0x1C Draw a map on the screen with a certain offset
- __Draw string:__ 0x1E Draw a string on the screen at a certain position
- __Clear all:__ 0x20 Clear both VRAM and TRAM
- __Clear graphics:__ 0x22 Clear VRAM
- __Clear strings:__ 0x24 Clear TRAM
- __Redraw:__ 0x26 Send the VRAM and TRAM to the LCD
- __All on:__ 0x28 Turn all the pixels on, draw them all black
- __All off:__ 0x2A Turn all the pixels of, draw them white
- __Invert:__ 0x2C Enable the inverted mode, black is white and vice versa
- __Normal:__ 0x2E Enable normal mode, to restore from inverted mode

##Instruction set

With this instructionset, the interpreter can be told to perform an action or operation. Each instruction performs a single action, but are not bound to an instruction or cycle speed. This means that it is hard to tell how long e.g. a subroutine will take, because there is no way to tell how long each instruction last. For this matter, a
timer is implemented, so that can be checked how many milliseconds have passed. All instructions have the same format: a header, followed by its arguments. The header is a specific code, called opcode or operation code, and each instructions has its own. This means that, to call a function, all you need to do is write down the
opcode, followed by it arguments. It looks like this: Most arguments are 8 bit wide, 

Opcode Operand 1 Operand 2 . . .

but some functions require a 16 bit argument, e.g. jumps. To correctly format these 16 bit arguments, the most significant byte (bit 8 to 15) should be first, followed by least significant byte (bit 0 to 7). This means that its hexadecimal representation can be ´copied´to the operands. To reduce size, and thus speed, instructions have an arbitrary length, and not a fixed one of e.g. 32 bits. This complicates jumps since not instructions should be counted, but bytes.

The instructions can be divided into seperate groups, depending on their function:

__Variables__ Loading of values, register manipulations . . . [page 10]
__Mathematics__ Adding, subtracting, multiplying . . . [page 15]
__Logic__ Logic AND, OR, XOR and NOT operations. [page 23]
__Jumps__ and calls Program counter manipulations. [page 27]
__Stack__ Push and pop values from and to the stack. [page 30]
__Graphics__ Draw sprites, strings . . . [page 33]
__Timer and delays__ Read and reset the timer and perform delays. [page 40]
__Data read/writes__ Read or write from the cartridge data memory. [page 39]

See the pdf for more information about every instruction.

#Authors

Gert-Jan Andries - info@gertjanandries.com - www.gertjanandries.com
Nick Steen
Xavier Dejager