#include <system.h>
#include "RAMdriver.h"
#include "IOcontrol.h"

//--------------------------------------------------
//DEFINES
//--------------------------------------------------
#define LCD               slave_0
#define CONTROLLER1       0x00
#define CONTROLLER2       0xff
#define GRAPHICS          0x30
#define NONE              0x40

#define PROGMEM           0
#define DATAMEM           1

#define SPRITEAMOUNT      3 //32
#define MAPSAMOUNT        0 //4

//--------------------------------------------------
//ADDRESSES
//--------------------------------------------------
#define SPRITEBASEADDRESS 0x00080//0x103FF 
#define STRINGBASEADDRESS 0x000A0//0x10FFF
#define MAPSBASEADDRESS   0x000C0//0x107FF

//--------------------------------------------------
//COMMANDS
//--------------------------------------------------
#define LOADSPRITE        0x10
#define LOADMAP           0x12
#define LOADSTRING        0x14

#define SETSPRITE         0x16
#define UPDATESPRITE      0x18
#define CLEARSPRITE       0x1A
#define DRAWMAP           0x1C
#define DRAWSTRING        0x1E

#define CLEARALL          0x20
#define CLEARGRAPHICS     0x22
#define CLEARSTRINGS      0x24

#define redraw            0x26

#define ALLON             0x28
#define ALLOFF            0x2A
#define INVERT            0x2C
#define NORMAL            0x2E

void io_slaveSelect(unsigned char slave);
void io_initStrings(unsigned short startAddress);
void io_initMaps(unsigned short startAddress, unsigned short amount);
void io_initSprites(unsigned short startAddress, unsigned short amount);

//--------------------------------------------------
//GENERAL IO
//--------------------------------------------------

void io_initIO()
{
    rd_initRAM(); //Setup the RAM driver
    ss_initSPI(); //Setup Software SPI
    io_initSlaveSelect();//Setup the Slave Select pins for SPI
    io_initSprites(SPRITEBASEADDRESS, SPRITEAMOUNT);
    io_initStrings(SPRITEBASEADDRESS);
    //io_initMaps(MAPSBASEADDRESS,MAPSAMOUNT);
}

void io_initSlaveSelect()
{
    //This function initialize the slave selects for the SPI. 
    trisa.3 = 0; //controller 1 Slave Select
    trisa.4 = 0; //controller 2 Slave Select
    trisa.5 = 0; //graphics Slave Select 

    //Slave is active low --> set al pins high, no slave selected at setup.
    porta.3 = 0;
    porta.4 = 0;
    porta.5 = 1;
}

//--------------------------------------------------
//MEMORY READER/WRITER
//--------------------------------------------------

unsigned char io_readProgramMemory(unsigned short address)
{
    //PROGMEM = default --> it's already set
    //read a byte from ram at given address. 
    
    //rd_setBank(PROGMEM);
    //return rd_readFromRAM(address);
    //delay_ms(200);
    
    //unsigned char code[100] = {55, 0, 0,      68, 72/*, 63, 250, 63, 250*/, 69, 72/*, 63, 250, 63, 250*/, 71, 72, 73, 56, 0, 10, 10, 72, 70, 72, 73, 56, 0, 66, 30, 72/*, 63, 250, 63, 250*/, 39, 00, 03};
    //unsigned char code[100] = {55, 0, 0, 56, 0, 10, 10, 72, 39, 0, 3};
    //unsigned char code[100] = { 1, 0, 40, 1, 1, 20, 52, 0, 255, 21, 0, 73, 56, 0, 0, 1, 72, 39, 0, 6};
    //unsigned char code[100] = { 55, 0, 0, 1, 0, 0, 1, 1, 20, /*52, 0, 255, 21, 0,*/21, 0, /*or 14,0,4*/ 73, 56, 0, 0, 1, 72, 39, 0, 9};
    unsigned char code[100] = { 55, 0, 0, 1, 0, 0, 1, 1, 20, /*52, 0, 255, 21, 0,*/21, 0, /*or 14,0,4*/ 73, 61, 0, 0, 0, 56, 0, 0, 1, 72, 39, 0, 9};
    
    
    //unsigned char code[100] = {1, 0, 38, 1, 1, 20, 55, 0, 0, 51, 0, 255, 42, 0, 3, 21, 0, 73, 56, 0, 0, 1, 72, 39, 0, 9};
    return code[address];
}

unsigned char io_readDataMemory(unsigned short address)
{
    rd_setBank(DATAMEM);
    unsigned char temp = rd_readFromRAM(address);
    return temp;
}

void io_writeToDataMemory(unsigned short address, unsigned char data)
{
    rd_setBank(DATAMEM);
    rd_writeToRAM(address,data);
}

//--------------------------------------------------
//CONTROLLER READER
//--------------------------------------------------

unsigned char io_readController(unsigned char controllerNumber)
{
    io_slaveSelect(controllerNumber);
    delay_us(6); //NINTENDO NES CONTROLLER NEEDS SHORT LATCH 
    io_slaveSelect(NONE);
    unsigned char data = ss_readByte();
    portb = ~data;
    return ~data;
}

//--------------------------------------------------
//GRAPHICS
//--------------------------------------------------

void io_allOn()
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(ALLON);
    //send arguments
    //none
    //SS back to high
    io_slaveSelect(NONE);
}

void io_allOff()
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(ALLOFF);
    //send arguments
    //none
    //SS back to high
    io_slaveSelect(NONE);
}

void io_invert()
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(INVERT);
    //send arguments
    //none
    //SS back to high
    io_slaveSelect(NONE);
}

void io_updateSprite(unsigned char indexVram,unsigned char x, unsigned char y)
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(UPDATESPRITE);
    //send arguments
    ss_sendByte(indexVram);
    ss_sendByte(x);
    ss_sendByte(y);
    //SS back to high
    io_slaveSelect(NONE);
}

void io_setSprite(unsigned char index, unsigned char indexVram)
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(SETSPRITE);
    //send arguments
    ss_sendByte(index);
    ss_sendByte(indexVram);
    //SS back to high
    io_slaveSelect(NONE);
}

void io_clearSprite(unsigned char index)
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(CLEARSPRITE);
    //send arguments
    ss_sendByte(index);
    //SS back to high
    io_slaveSelect(NONE);
}

void io_drawString(unsigned char index, unsigned char x, unsigned char y)
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(DRAWSTRING);
    //send arguments
    ss_sendByte(index);
    ss_sendByte(x);
    ss_sendByte(y);
    //SS back to high
    io_slaveSelect(NONE);
}

void io_drawMap(unsigned char index)
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(DRAWMAP);
    //send arguments
    ss_sendByte(index);
    //SS back to high
    io_slaveSelect(NONE);
}

void io_clearAll()
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(CLEARALL);
    //send arguments
    //no argumens
    //SS back to high
    io_slaveSelect(NONE);
}

void io_clearStrings()
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(CLEARSTRINGS);
    //send arguments
    //no argumens
    //SS back to high
    io_slaveSelect(NONE);
}

void io_clearGraphics()
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(CLEARGRAPHICS);
    //send arguments
    //no argumens
    //SS back to high
    io_slaveSelect(NONE);
}

void io_redraw()
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(redraw);
    //send arguments
    //no argumens
    //SS back to high
    io_slaveSelect(NONE);
}

void io_normalMode()
{
    //set SS pin to low
    io_slaveSelect(GRAPHICS);
    //send command
    ss_sendByte(NORMAL);
    //send arguments
    //no argumens
    //SS back to high
    io_slaveSelect(NONE);
}

//--------------------------------------------------
//PRIVATES
//--------------------------------------------------

void io_initSprites(unsigned short startAddress, unsigned short amount)
{
	/*
    unsigned short currentAddress = SPRITEBASEADDRESS;

    rd_setBank(DATAMEM);

    for(int i = 0; i < amount; i++)
    {
        io_slaveSelect(GRAPHICS);
        ss_sendByte(LOADSPRITE);
        //1sprite = 8 bytes
        for(int j = 0; j < 8;j++)
        {
            ss_sendByte(rd_readFromRAM(currentAddress));
            currentAddress++;
        } 
        io_slaveSelect(NONE);
    }
    */
    //DEBUG
    io_slaveSelect(GRAPHICS);
    ss_sendByte(LOADSPRITE);
    ss_sendByte(0);
    unsigned char sprite[] = {255,129,129,129,129,129,129,255};
    //1sprite = 8 bytes
    for(int j = 0; j < 8; j++)
    {
        ss_sendByte(sprite[j]);
    } 
    io_slaveSelect(NONE);
    delay_s(2);
    io_slaveSelect(GRAPHICS);
    io_normalMode();
    ss_sendByte(LOADSTRING);
    ss_sendByte(0);
    io_slaveSelect(NONE);
}

void io_initMaps(unsigned short startAddress, unsigned short amount)
{
    unsigned short currentAddress = MAPSBASEADDRESS;

    rd_setBank(DATAMEM);

    for(int i = 0; i < amount; i++)
    {
        io_slaveSelect(GRAPHICS);
        ss_sendByte(LOADMAP);
        //1 map = 128 bytes
        for(int j = 0; j < 128;j++)
        {
            ss_sendByte(rd_readFromRAM(currentAddress));
            currentAddress++;
        } 
        io_slaveSelect(NONE);
    }
}

void io_initStrings(unsigned short startAddress)
{
    unsigned short currentAddress = STRINGBASEADDRESS;

    rd_setBank(DATAMEM);

    //escaping char = \0 --> null \0\0 no more strings
    
    unsigned char previous = 0xFF;
    unsigned char readedChar = 0xFF;
        
    while(1)
    {
        //set SS pin to low
        io_slaveSelect(GRAPHICS);
        //send command
        ss_sendByte(LOADSTRING);
        //send arguments
        while((readedChar = rd_readFromRAM(currentAddress)) != '\0')
        {
            ss_sendByte(readedChar);
            currentAddress++;
        }
        if(readedChar == '\0' && previous == '\0')
        {
            currentAddress++;
            //SS back to high
            io_slaveSelect(NONE);
            break;
        }
        //if(readedChar == '\0') previous = '\0';
        previous = readedChar;

        currentAddress++;
        //SS back to high
        io_slaveSelect(NONE);
    }

    io_slaveSelect(NONE);
}

void io_slaveSelect(unsigned char slave)
{
    //LOW to send
    //C1 = d.3
    //C2 = d.4
    //GFX = d.5
    trisd = 0b00000100;
    
    switch(slave)
    {
        case CONTROLLER1:
        /*porta.4 = 0;
        porta.5 = 1;
        porta.3 = 1;*/
        portd.3 = 1;
        portd.4 = 0;
        portd.5 = 1;
        break;
        case CONTROLLER2: 
        /*porta.3 = 0;
        porta.5 = 1;
        porta.4 = 1;*/
        portd.3 = 0;
        portd.4 = 1;
        portd.5 = 1;
        break;
        case GRAPHICS:
        /*porta.3 = 0;
        porta.4 = 0;
        porta.5 = 0;*/
        portd.3 = 0;
        portd.4 = 0;
        portd.5 = 0;
        break;
        case NONE:
        /*porta.3 = 0;
        porta.4 = 0;
        porta.5 = 1;*/
        portd.3 = 0;
        portd.4 = 0;
        portd.5 = 1;
        break;
        default:
        /*porta.3 = 0;
        porta.4 = 0;
        porta.5 = 1;*/
        portd.3 = 0;
        portd.4 = 0;
        portd.5 = 1;
        break;
    }
}