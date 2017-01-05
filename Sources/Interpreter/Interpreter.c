#include "Interpreter.h"
#include "IOcontrol.h"

//--------------------------------------------------
//DEFINES
//--------------------------------------------------
#define HL reg[15]
#define RANDOMSEED 0xC9
#define SCREENHEIGHT 48
#define SCREENWIDTH 84

//--------------------------------------------------
//VARIABLES
//--------------------------------------------------
unsigned char random_nr = RANDOMSEED;
unsigned char random_nr_2 = RANDOMSEED/2;
unsigned char powerLUT[16] = {0,1,4,9,16,25,36,49,64,81,100,121,144,169,196,225};
unsigned char stack[64];
unsigned char reg[16];
unsigned char RAM[256];
unsigned char sprites[256];
unsigned char timerCounter;
unsigned short stackPointer;
unsigned short programCounter;

#ifdef DEBUGASSERT
unsigned short instructionCounter;
#endif

//--------------------------------------------------
//INLINES
//--------------------------------------------------

//Push a 16 bit value to the stack
//Not accessible via interpreter, only used internally
inline void ip_push16(unsigned short value)
{
    stack[stackPointer++] = (unsigned char)(reg[value]&0x00FF);
    stack[stackPointer++] = (unsigned char)((reg[value]>>8)&0x00FF);
}

//Push a 16 bit value to the stack
//Not accessible via interpreter, only used internally
inline unsigned short ip_pop16()
{
    unsigned short tmp = 0;
    tmp = stack[stackPointer--]<<8;
    tmp += stack[stackPointer--];
    return tmp;
}

/// <summary> 
/// This function has exceptionally well performance at doing nothing
/// </summary>
/// <mnemonic>
/// NOP
/// </mnemonic>
/// <opcode>
/// 00
/// </opcode>
inline void ip_nop()
{

}

/// <summary>
/// Loads a fixed value into a specific register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// LD
/// </mnemonic>
/// <opcode>
/// 01
/// </opcode>
inline void ip_load(unsigned char address, unsigned char value)
{
    reg[address] = value;
}

/// <summary>
/// Copies te value from a register to another register.
/// The value in the source register is retained.
/// </summary>
/// <param name="destaddr">
/// The destaddr parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <param name="srcaddr">
/// The srcaddr parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// CPY
/// </mnemonic>
/// <opcode>
/// 02
/// </opcode>
inline void ip_copy(unsigned char destaddr, unsigned char srcaddr)
{
    reg[destaddr] = reg[srcaddr];
}

/// <summary>
/// Store the value of a register to RAM.
/// </summary>
/// <param name="destaddr">
/// The destaddr parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <param name="srcaddr">
/// The srcaddr parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// ST
/// </mnemonic>
/// <opcode>
/// 03
/// </opcode>
inline void ip_store(unsigned char destaddr, unsigned char srcaddr)
{
    RAM[destaddr] = reg[srcaddr];
}

/// <summary>
/// Store a fixed value to RAM.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// STI
/// </mnemonic>
/// <opcode>
/// 04
/// </opcode>
inline void ip_storeImmediate(unsigned char address, unsigned char value)
{
    RAM[address] = value;
}

/// <summary>
/// Load a value from RAM into a register.
/// </summary>
/// <param name="destaddr">
/// The destaddr parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <param name="srcaddr">
/// The srcaddr parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// LDR
/// </mnemonic>
/// <opcode>
/// 05
/// </opcode>
inline void ip_loadRam(unsigned char destaddr, unsigned char srcaddr)
{
    reg[destaddr] = RAM[srcaddr];
}

/// <summary>
/// Compare two values of registers.
/// Two results are possible:
/// 0 means not equal,
/// not 0 means equal.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// CMP
/// </mnemonic>
/// <opcode>
/// 06
/// </opcode>
inline void ip_compare(unsigned char firstaddress, unsigned char secondaddress)
{
    HL = (unsigned char)(reg[firstaddress] == reg[secondaddress]);
}

/// <summary> 
/// Check if the first register's value is greater than the second register's value.
/// Two results are possible:
/// 0 means register 2's value is bigger,
/// not 0 means register 1's value is bigger.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// GT
/// </mnemonic>
/// <opcode>
/// 07
/// </opcode>
inline void ip_greaterThan(unsigned char firstaddress, unsigned char secondaddress)
{
    HL = (unsigned char)(reg[firstaddress] > reg[secondaddress]);
}

/// <summary> 
/// Check if a register value is greater than a fixed value
/// Two results are possible:
/// 0 means the fixed value value is bigger,
/// not 0 means the register's value is bigger.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// GTI
/// </mnemonic>
/// <opcode>
/// 08
/// </opcode>
inline void ip_greaterThanImmediate(unsigned char address, unsigned char value)
{
    HL = (unsigned char)(reg[address] > value);
}

/// <summary> 
/// Check if the first register's value is smaller than the second register's value.
/// Two results are possible:
/// 0 means register 2's value is smaller,
/// not 0 means register 1's value is smaller.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// LT
/// </mnemonic>
/// <opcode>
/// 09
/// </opcode>
inline void ip_lessThan(unsigned char firstaddress, unsigned char secondaddress)
{
    HL = (unsigned char)(reg[firstaddress] < reg[secondaddress]);
}

/// <summary> 
/// Check if a register value is smaller than a fixed value.
/// Two results are possible:
/// 0 means the fixed value is smaller,
/// not 0 means the register's value is smaller.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// LTI
/// </mnemonic>
/// <opcode>
/// 10
/// </opcode>
inline void ip_lessThanImmediate(unsigned char address, unsigned char value)
{
    HL = (unsigned char)(reg[address] < value);
}

/// <summary> 
/// Swap the values of two registers.
/// Each register gets the value of the other register in one instruction.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// SWAP
/// </mnemonic>
/// <opcode>
/// 11
/// </opcode>
inline void ip_swap(unsigned char firstaddress, unsigned char secondaddress)
{
    unsigned char tmp = reg[firstaddress];
    reg[firstaddress] = reg[secondaddress];
    reg[secondaddress] = tmp;
}

/// <summary> 
/// Swap the values of two RAM addresses.
/// Each RAM address gets the value of the other RAM address in one instruction.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// SWAPR
/// </mnemonic>
/// <opcode>
/// 12
/// </opcode>
inline void ip_swapRam(unsigned char firstaddress, unsigned char secondaddress)
{
    unsigned char tmp = RAM[firstaddress];
    RAM[firstaddress] = RAM[secondaddress];
    RAM[secondaddress] = tmp;
}

/// <summary> 
/// Add the values of two registers.
/// The result is stored in the first register.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// ADD
/// </mnemonic>
/// <opcode>
/// 13
/// </opcode>
inline void ip_add(unsigned char firstaddress, unsigned char secondaddress)
{
    reg[firstaddress] += reg[secondaddress];
}

/// <summary> 
/// Add a fixed value to the value of a register.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// ADDI
/// </mnemonic>
/// <opcode>
/// 14
/// </opcode>
inline void ip_addImmediate(unsigned char address, unsigned char value)
{
    reg[address] += value;
}

/// <summary> 
/// Subtract the values of two registers.
/// The result is stored in the first register.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// SUB
/// </mnemonic>
/// <opcode>
/// 15
/// </opcode>
inline void ip_subtract(unsigned char firstaddress, unsigned char secondaddress)
{
    reg[firstaddress] -= reg[secondaddress];
}

/// <summary> 
/// Subtract a fixed value from the value of a register.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// SUBI
/// </mnemonic>
/// <opcode>
/// 16
/// </opcode>
inline void ip_subtractImmediate(unsigned char address, unsigned char value)
{
    reg[address] -= value;
}

/// <summary> 
/// Multiply the values of two registers.
/// The result is stored in the first register.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// MUL
/// </mnemonic>
/// <opcode>
/// 17
/// </opcode>
inline void ip_multiply(unsigned char firstaddress, unsigned char secondaddress)
{
    reg[firstaddress] *= reg[secondaddress];
}

/// <summary> 
/// Multiply a fixed value with the value of a register.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// MULI
/// </mnemonic>
/// <opcode>
/// 18
/// </opcode>
inline void ip_multiplyImmediate(unsigned char address, unsigned char value)
{
    reg[address] *= value;
}

/// <summary> 
/// Divide the value of the first register by the value of the second register.
/// The result is stored in the first register.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// DIV
/// </mnemonic>
/// <opcode>
/// 19
/// </opcode>
inline void ip_divide(unsigned char firstaddress, unsigned char secondaddress)
{
    reg[firstaddress] /= reg[secondaddress];
}

/// <summary> 
/// Divide the value of a register by a fixed value.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// DIVI
/// </mnemonic>
/// <opcode>
/// 20
/// </opcode>
inline void ip_divideImmediate(unsigned char address, unsigned char value)
{
    reg[address] /= value;
}

/// <summary> 
/// Increment the value of a register by one.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// INC
/// </mnemonic>
/// <opcode>
/// 21
/// </opcode>
inline void ip_increment(unsigned char address)
{
    reg[address]++;
}

/// <summary> 
/// Decrement the value of a register by one.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// DEC
/// </mnemonic>
/// <opcode>
/// 22
/// </opcode>
inline void ip_decrement(unsigned char address)
{
    reg[address]++;
}

/// <summary> 
/// Calculate the power of the value of a register.
/// The power is the value of a register with the address in the second argument.
/// The result is stored in the first register.
/// </summary>
/// <param name="destaddr">
/// The destaddr parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="pwraddr">
/// The pwraddr parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// POW
/// </mnemonic>
/// <opcode>
/// 23
/// </opcode>
inline void ip_power(unsigned char destaddr, unsigned char pwraddr)
{
    unsigned char tmp = reg[destaddr];
    for(unsigned char i = 0; i<reg[pwraddr]; i++)
        reg[destaddr] *= tmp;
}

/// <summary> 
/// Calculate the power of the value of a register.
/// The power is a fixed value given as the second argument.
/// The result is stored in the register.
/// </summary>
/// <param name="destaddr">
/// The destaddr parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="pwrval">
/// The pwrval parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// POWI
/// </mnemonic>
/// <opcode>
/// 24
/// </opcode>
inline void ip_powerImmediate(unsigned char destaddr, unsigned char pwrval)
{
    unsigned char tmp = reg[destaddr];
    for(unsigned char i = 0; i<pwrval; i++)
        reg[destaddr] *= tmp;
}

/// <summary> 
/// Calculate the square root of the value of a register.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// SQRT
/// </mnemonic>
/// <opcode>
/// 25
/// </opcode>
inline void ip_squareRoot(unsigned char address)
{
    unsigned char verschil = 255;
    unsigned char vorigeVerschil = 255;
    unsigned char i = 0;

    while(vorigeVerschil>=verschil)
    {
        vorigeVerschil = verschil;
        verschil = abs(address-powerLUT[programCounter++]);
    }

    reg[address] = i-2;
}

/// <summary> 
/// Calculate the log (base 2) of the value of a register.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// LOG2
/// </mnemonic>
/// <opcode>
/// 26
/// </opcode>
inline void ip_log2(unsigned char address)
{
    if(address < 4)
        reg[address] = 1;
    else if(address < 6)
        reg[address] = 2;
    else if(address < 12)
        reg[address] = 3;
    else if(address < 23)
        reg[address] = 4;
    else if(address < 45)
        reg[address] = 5;
    else if(address < 91)
        reg[address] = 6;
    else if(address < 182)
        reg[address] = 7;
    else
        reg[address] = 8;
}

/// <summary> 
/// Calculate the log (base 10) of the value of a register.
/// The result is stored in the register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// LOG10
/// </mnemonic>
/// <opcode>
/// 27
/// </opcode>
inline void ip_log10(unsigned char address)
{
    if(address < 32)
        reg[address] = 1;
    else
        reg[address] = 2;
}

/// <summary> 
/// Generate a random value in a given range.
/// The value is an unsigned char.
/// The range is defined by the first and the second argument as addresses of registers.
/// </summary>
/// <param name="rangemin">
/// The rangemin parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="rangemax">
/// The rangemax parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// RND
/// </mnemonic>
/// <opcode>
/// 28
/// </opcode>
inline void ip_rand(unsigned char rangemin, unsigned char rangemax)
{
// http://www.dontronics.com/psbpix/random.html
// XOR 0-2 and 5-6
// XOR results of these XORs
    unsigned char tmp = (random_nr.1^random_nr.4)^(random_nr.5^random_nr.6);
    random_nr <<= 1;
    random_nr.0 = tmp.0;

    tmp = (random_nr_2.2^random_nr_2.3)^(random_nr_2.6^random_nr_2.7);
    random_nr_2 <<= 1;
    random_nr_2 = tmp.0;

    unsigned char tmpRand = random_nr^random_nr_2;
    HL = reg[rangemin]+(tmpRand-0)*(reg[rangemax]-reg[rangemin])/(255-0);
}

/// <summary> 
/// Generate a random value in a given range.
/// The value is an unsigned char.
/// The range is defined by the first and the second argument as fixed values.
/// </summary>
/// <param name="rangemin">
/// The rangemin parameter requires an unsigned short containing an 8 bit value.
/// </param> 
/// <param name="rangemax">
/// The rangemax parameter requires an unsigned short containing an 8 bit value.
/// </param> 
/// <mnemonic>
/// RNDI
/// </mnemonic>
/// <opcode>
/// 29
/// </opcode>
inline void ip_randImmediate(unsigned char rangemin, unsigned char rangemax)
{
// http://www.dontronics.com/psbpix/random.html
// XOR 0-2 and 5-6
// XOR results of these XORs
    unsigned char tmp = (random_nr.1^random_nr.4)^(random_nr.5^random_nr.6);
    random_nr <<= 1;
    random_nr.0 = tmp.0;

    tmp = (random_nr_2.2^random_nr_2.3)^(random_nr_2.6^random_nr_2.7);
    random_nr_2 <<= 1;
    random_nr_2 = tmp.0;

    unsigned char tmpRand = random_nr^random_nr_2;
    HL = rangemin+(tmpRand-0)*(rangemax-rangemin)/(255-0);
}

/// <summary> 
/// Performs a bitwise AND on the values of two registers.
/// The result is stored in the HL register.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// AND
/// </mnemonic>
/// <opcode>
/// 30
/// </opcode>
inline void ip_bitwiseAnd(unsigned char firstaddress, unsigned char secondaddress)
{
    HL = reg[firstaddress] & reg[secondaddress];
}

/// <summary> 
/// Performs a bitwise AND on the value of a register and a fixed value.
/// The result is stored in the HL register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param> 
/// <mnemonic>
/// ANDI
/// </mnemonic>
/// <opcode>
/// 31
/// </opcode>
inline void ip_bitwiseAndImmediate(unsigned char address, unsigned char value)
{
    HL = reg[address] & value;
}

/// <summary> 
/// Performs a bitwise OR on the values of two registers.
/// The result is stored in the HL register.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// OR
/// </mnemonic>
/// <opcode>
/// 32
/// </opcode>
inline void ip_bitwiseOr(unsigned char firstaddress, unsigned char secondaddress)
{
    HL = reg[firstaddress] | reg[secondaddress];
}

/// <summary> 
/// Performs a bitwise OR on the value of a register and a fixed value.
/// The result is stored in the HL register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param> 
/// <mnemonic>
/// ORI
/// </mnemonic>
/// <opcode>
/// 33
/// </opcode>
inline void ip_bitwiseOrImmediate(unsigned char address, unsigned char value)
{
    HL = reg[address] | value;
}

/// <summary> 
/// Performs a bitwise XOR on the values of two registers.
/// The result is stored in the HL register.
/// </summary>
/// <param name="firstaddress">
/// The firstaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="secondaddress">
/// The secondaddress parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// XOR
/// </mnemonic>
/// <opcode>
/// 34
/// </opcode>
inline void ip_bitwiseXor(unsigned char firstaddress, unsigned char secondaddress)
{
    HL = reg[firstaddress] ^ reg[secondaddress];
}

/// <summary> 
/// Performs a bitwise XOR on the value of a register and a fixed value.
/// The result is stored in the HL register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param> 
/// <mnemonic>
/// XORI
/// </mnemonic>
/// <opcode>
/// 35
/// </opcode>
inline void ip_bitwiseXorImmediate(unsigned char address, unsigned char value)
{
    HL = reg[address] ^ value;
}

/// <summary> 
/// Performs a bitwise NOT of the value of a registers.
/// The result is stored in the HL register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <mnemonic>
/// NOT
/// </mnemonic>
/// <opcode>
/// 36
/// </opcode>
inline void ip_bitwiseNot(unsigned char address)
{
    HL = ~reg[address];
}

/// <summary> 
/// Sets a bit in a register's value, sepcified by the second argument.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="position">
/// The position parameter requires an unsigned short containing an 8 bit value.
/// </param> 
/// <mnemonic>
/// SBIT
/// </mnemonic>
/// <opcode>
/// 37
/// </opcode>
inline void ip_setBit(unsigned char address, unsigned char position)
{ 
    set_bit(reg[address], position);
}

/// <summary> 
/// Clears a bit in a register's value, sepcified by the second argument.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param> 
/// <param name="position">
/// The position parameter requires an unsigned short containing an 8 bit value.
/// </param> 
/// <mnemonic>
/// CBIT
/// </mnemonic>
/// <opcode>
/// 38
/// </opcode>
inline void ip_clearBit(unsigned char address, unsigned char position)
{
    clear_bit(reg[address], position);
}

/// <summary> 
/// Jumps inconditionally to the instruction address specified in the first argument.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param> 
/// <mnemonic>
/// JMP
/// </mnemonic>
/// <opcode>
/// 39
/// </opcode>
inline void ip_jump(unsigned short address)
{
    programCounter = address;
}

/// <summary> 
/// Jumps inconditionally by incrementing the program counter with the value of the first argument.
/// If the the program counter overflows, execution of the program is aborted.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param> 
/// <mnemonic>
/// JMPR
/// </mnemonic>
/// <opcode>
/// 40
/// </opcode>
inline void ip_jumpRelative(unsigned short address)
{
    programCounter += address;
}

/// <summary> 
/// Jumps if the HL register's value equals zero to the instruction address specified in the first argument.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param> 
/// <mnemonic>
/// JMPZ
/// </mnemonic>
/// <opcode>
/// 41
/// </opcode>
inline void ip_jumpIfZero(unsigned short address)
{
    if(HL == 0)
        programCounter = address;
}

/// <summary> 
/// Jumps if the HL register's value equals zero by incrementing the program counter with the value of the first argument.
/// If the the program counter overflows, execution of the program is aborted.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param>
/// <mnemonic>
/// JMPRZ
/// </mnemonic>
/// <opcode>
/// 42
/// </opcode>
inline void ip_jumpRelativeIfZero(unsigned short address)
{
    if(HL == 0)
        programCounter += address;
}

/// <summary> 
/// Jumps if the HL register's value does not equal zero to the instruction address specified in the first argument.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param> 
/// <mnemonic>
/// JMPNZ
/// </mnemonic>
/// <opcode>
/// 43
/// </opcode>
inline void ip_jumpIfNotZero(unsigned short address)
{
    if(HL != 0)
        programCounter = address;
}

/// <summary> 
/// Jumps if the HL register's value does not equal zero by incrementing the program counter with the value of the first argument.
/// If the the program counter overflows, execution of the program is aborted.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param>
/// <mnemonic>
/// JMPRNZ
/// </mnemonic>
/// <opcode>
/// 44
/// </opcode>
inline void ip_jumpRelativeIfNotZero(unsigned short address)
{
    if(HL != 0)
        programCounter += address;
}

/// <summary> 
/// Executes a subroutine at the address specified in the first argument.
/// Starts by pushing the program counter to the stack.
/// At the end of a subroutine, RET should be called to pop the program counter from the stack and return to the last execution point.
/// If the the program counter overflows, execution of the program is aborted.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param>
/// <mnemonic>
/// CALL
/// </mnemonic>
/// <opcode>
/// 45
/// </opcode>
inline void ip_call(unsigned short address)
{
    ip_push16(programCounter);
    programCounter = address;
}

/// <summary> 
/// Performs a call if and only if the HL register does not contain zero.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param>
/// <mnemonic>
/// CALLNZ
/// </mnemonic>
/// <opcode>
/// 66
/// </opcode>
inline void ip_callIfNotZero(unsigned short address)
{
    if(HL != 0)
    {
        ip_call(address);
    }
}

/// <summary> 
/// Performs a call if and only if the HL register contains zero.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// CALLZ
/// </mnemonic>
/// <opcode>
/// 67
/// </opcode>
inline void ip_callIfZero(unsigned short address)
{
    if(HL == 0)
    {
        ip_call(address);
    }
}


/// <summary> 
/// Pops the program counter from the stack and returns to execution after a subroutine was called.
/// </summary>
/// <mnemonic>
/// RET
/// </mnemonic>
/// <opcode>
/// 46
/// </opcode>
inline void ip_ret()
{
    programCounter = ip_pop16();
}

/// <summary> 
/// Pushes a register's value to the stack.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing an 8 bit address.
/// </param>
/// <mnemonic>
/// PUSH
/// </mnemonic>
/// <opcode>
/// 47
/// </opcode>
inline void ip_push(unsigned char address)
{
    stack[stackPointer++] = reg[address];
}

/// <summary> 
/// Pushes a fixed value to the stack.
/// </summary>
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// PUSHI
/// </mnemonic>
/// <opcode>
/// 48
/// </opcode>
inline void ip_pushImmediate(unsigned char value)
{
    stack[stackPointer++] = value;
}

/// <summary> 
/// Pops a value from the stack to a register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 8 bit address.
/// </param>
/// <mnemonic>
/// POP
/// </mnemonic>
/// <opcode>
/// 49
/// </opcode>
inline void ip_pop(unsigned char address)
{
    reg[address] = stack[stackPointer--];
}

/// <summary> 
/// Reads a controller and places the input in the HL register.
/// The id as the first argument is either 0 or 1, depending on the controller that is to be read.
/// </summary>
/// <param name="id">
/// The id parameter requires an unsigned short containing a 8 bit id.
/// </param>
/// <mnemonic>
/// BRD
/// </mnemonic>
/// <opcode>
/// 50
/// </opcode>
inline void ip_buttonRead(unsigned char id)
{
    HL = io_readController(id);
}

/// <summary> 
/// Reads a controller and places the input of a specific button in the HL register.
/// If the button is pressed, 1 is placed in HL, else 0.
/// The id as the first argument is either 0 or 1, depending on the controller that is to be read.
/// The button as the second argument is the button that is to be read, for more info, see the controller chapter.
/// </summary>
/// <param name="id">
/// The id parameter requires an unsigned short containing a 8 bit id.
/// </param>
/// <param name="btn">
/// The btn parameter requires an unsigned short containing a 8 bit button id.
/// </param>
/// <mnemonic>
/// BRDS
/// </mnemonic>
/// <opcode>
/// 51
/// </opcode>
inline void ip_buttonReadSingle(unsigned char id, unsigned char btn)
{
    if((io_readController(id) & btn)>0)
        HL = 1;
    else
        HL = 0;
}

/// <summary>
/// Pauses execution of a program until a specific button or buttoncombination is pressed.
/// As long as the buttoncombination does not equal the given one, the program halts.
/// </summary>
/// <param name="id">
/// The id parameter requires an unsigned short containing a 8 bit id.
/// </param>
/// <param name="btn">
/// The btn parameter requires an unsigned short containing a 8 bit button id.
/// </param>
/// <mnemonic>
/// WAITF
/// </mnemonic>
/// <opcode>
/// 52
/// </opcode>
inline void ip_waitFor(unsigned char id, unsigned char btn)
{
    while((io_readController(id) & btn)==btn);
}

/// <summary>
/// Places the screenheight in the HL register.
/// </summary>
/// <mnemonic>
/// SCRH
/// </mnemonic>
/// <opcode>
/// 53
/// </opcode>
inline void ip_screenHeight()
{
    HL = SCREENHEIGHT;
}

/// <summary>
/// Places the screenwidth in the HL register.
/// </summary>
/// <mnemonic>
/// SCRW
/// </mnemonic>
/// <opcode>
/// 54
/// </opcode>
inline void ip_screenWidth()
{
    HL = SCREENWIDTH;
}

/// <summary>
/// Places a sprite from code to VRAM.
/// The first argument is the index in the sprite array in code.
/// The second argument is the index in the on-screen sprite array.
/// </summary>
/// <param name="index">
/// The index parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <param name="indexonscreen">
/// The indexonscreen parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <mnemonic>
/// SSPR
/// </mnemonic>
/// <opcode>
/// 55
/// </opcode>
inline void ip_setSprite(unsigned char index, unsigned char indexonscreen)
{
    io_setSprite(indexonscreen, index);
}

/// <summary>
/// Updates the position of the sprite on the screen.
/// Positioning the sprite out of the bounds of the screen makes the sprite invisible.
/// The second and third argument contain respectively the register with the x position and the y position.
/// </summary>
/// <param name="indexonscreen">
/// The indexonscreen parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <param name="x">
/// The x parameter requires an unsigned short containing a 8 bit address.
/// </param>
/// <param name="y">
/// The y parameter requires an unsigned short containing a 8 bit address.
/// </param>
/// <mnemonic>
/// USPR
/// </mnemonic>
/// <opcode>
/// 56
/// </opcode>
inline void ip_updateSprite(unsigned char indexonscreen, unsigned char x, unsigned char y)
{
    io_updateSprite(indexonscreen, reg[x], reg[y]);
    
    sprites[indexonscreen*2] = reg[x];
    sprites[indexonscreen*2+1] = reg[y];
}

/// <summary>
/// Updates the position of the sprite on the screen.
/// Positioning the sprite out of the bounds of the screen makes the sprite invisible.
/// The second and third argument contain respectively the x position and the y position.
/// </summary>
/// <param name="indexonscreen">
/// The indexonscreen parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <param name="x">
/// The x parameter requires an unsigned short containing a 8 bit value.
/// </param>
/// <param name="y">
/// The y parameter requires an unsigned short containing a 8 bit value.
/// </param>
/// <mnemonic>
/// USPRI
/// </mnemonic>
/// <opcode>
/// 74
/// </opcode>
inline void ip_updateSpriteImmediate(unsigned char indexonscreen, unsigned char x, unsigned char y)
{
    io_updateSprite(indexonscreen, x, y);
    
    sprites[indexonscreen*2] = x;
    sprites[indexonscreen*2+1] = y;
}

/// <summary>
/// Removes a sprite from the onscreen sprite index.
/// </summary>
/// <param name="indexonscreen">
/// The indexonscreen parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <mnemonic>
/// CSPR
/// </mnemonic>
/// <opcode>
/// 57
/// </opcode>
inline void ip_clearSprite(unsigned char indexonscreen)
{
    io_clearSprite(indexonscreen);
    
    sprites[indexonscreen*2] = 0;
    sprites[indexonscreen*2+1] = 0;
}

/// <summary>
/// Calculates the horizontal distance between two sprites.
/// The distance (in pixels) is placed in the HL register.
/// </summary>
/// <param name="firstindexonscreen">
/// The firstindexonscreen parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <param name="secondindexonscreen">
/// The secondindexonscreen parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <mnemonic>
/// SPRX
/// </mnemonic>
/// <opcode>
/// 58
/// </opcode>
inline void ip_checkSpriteX(unsigned char firstindexonscreen, unsigned char secondindexonscreen)
{
    HL = sprites[firstindexonscreen*2] - sprites[secondindexonscreen*2];
}

/// <summary>
/// Calculates the vertical distance between two sprites.
/// The distance (in pixels) is placed in the HL register.
/// </summary>
/// <param name="firstindexonscreen">
/// The firstindexonscreen parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <param name="secondindexonscreen">
/// The secondindexonscreen parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <mnemonic>
/// SPRY
/// </mnemonic>
/// <opcode>
/// 59
/// </opcode>
inline void ip_checkSpriteY(unsigned char firstindexonscreen, unsigned char secondindexonscreen)
{
    HL = sprites[firstindexonscreen*2+1] - sprites[secondindexonscreen*2+1];
}

/// <summary>
/// Draws a map on the screen.
/// The first argument is the index of the map in the map memory.
/// </summary>
/// <param name="index">
/// The index parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <mnemonic>
/// SMAP
/// </mnemonic>
/// <opcode>
/// 60
/// </opcode>
inline void ip_setMap(unsigned char index)
{
    io_drawMap(index);
}

/// <summary>
/// Draws a string on the screen at a specified position.
/// The first argument is the index of the string in the string memory.
/// </summary>
/// <param name="index">
/// The index parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <param name="x">
/// The x parameter requires an unsigned short containing a 8 bit value.
/// </param>
/// <param name="y">
/// The y parameter requires an unsigned short containing a 8 bit value.
/// </param>
/// <mnemonic>
/// SSTR
/// </mnemonic>
/// <opcode>
/// 61
/// </opcode>
inline void ip_setString(unsigned char index, unsigned char x, unsigned char y)
{
    io_drawString(index, x, y);
}

/// <summary>
/// Waits for an amount of milliseconds equal to the value of the register given in the first argument.
/// </summary>
/// <param name="index">
/// The index parameter requires an unsigned short containing a 8 bit index.
/// </param>
/// <mnemonic>
/// DLY
/// </mnemonic>
/// <opcode>
/// 62
/// </opcode>
inline void ip_delay(unsigned char index)
{
    delay_ms(reg[index]);
}

/// <summary>
/// Waits for an amount of milliseconds equal to the fixed value given in the first argument.
/// </summary>
/// <param name="value">
/// The value parameter requires an unsigned short containing a 8 bit value.
/// </param>
/// <mnemonic>
/// DLYI
/// </mnemonic>
/// <opcode>
/// 63
/// </opcode>
inline void ip_delayImmediate(unsigned char value)
{
    delay_ms(value);
}

/// <summary>
/// Clears the timer by resetting its value to zero.
/// </summary>
/// <mnemonic>
/// CTMR
/// </mnemonic>
/// <opcode>
/// 64
/// </opcode>
inline void ip_clearTimer()
{
    timerCounter = 0;
}

/// <summary>
/// Places the value of the timer in the HL register.
/// </summary>
/// <mnemonic>
/// RTMR
/// </mnemonic>
/// <opcode>
/// 65
/// </opcode>
inline void ip_readTimer()
{
    HL = timerCounter;
}

/// <summary>
/// Sets all pixels on the screen to the on-state.
/// The actual framebuffer is not affected.
/// To get back to drawing the buffer, call normalMode().
/// No redraw is needed to change the state.
/// </summary>
/// <mnemonic>
/// DON
/// </mnemonic>
/// <opcode>
/// 68
/// </opcode>
inline void allOn()
{
    io_allOn();
}

/// <summary>
/// Sets all pixels on the screen to the off-state.
/// The actual framebuffer is not affected.
/// To get back to drawing the buffer, call normalMode().
/// No redraw is needed to change the state.
/// </summary>
/// <mnemonic>
/// DOFF
/// </mnemonic>
/// <opcode>
/// 69
/// </opcode>
inline void allOff()
{
    io_allOff();
}

/// <summary>
/// Sets the screen to drawing the framebuffer inverted.
/// The actual framebuffer is not affected, nor the operation of its states.
/// A cleared pixel will be lit, and a lit pixel will be cleared on screen.
/// No redraw is needed to change the state.
/// </summary>
/// <mnemonic>
/// DINV
/// </mnemonic>
/// <opcode>
/// 70
/// </opcode>
inline void invertedMode()
{
    io_invert();
}

/// <summary>
/// Sets the screen to drawing the framebuffer.
/// No redraw is needed to change the state.
/// </summary>
/// <mnemonic>
/// DNRM
/// </mnemonic>
/// <opcode>
/// 71
/// </opcode>
inline void normalMode()
{
    io_normalMode();
}

/// <summary>
/// Redraws the framebuffer to the screen.
/// If allOn or allOff has been called right before a redraw, it is needed to call normalMode to draw the framebuffer.
/// </summary>
/// <mnemonic>
/// DRDRW
/// </mnemonic>
/// <opcode>
/// 72
/// </opcode>
inline void redraw()
{
	io_redraw();
}

/// <summary>
/// Clears the entire framebuffer by resetting its pixelvalues to zero.
/// Will only be seen after a redraw.
/// After a clearAll, the framebuffer can be written to immediately to update the screen.
/// </summary>
/// <mnemonic>
/// DCLR
/// </mnemonic>
/// <opcode>
/// 73
/// </opcode>
inline void clearAll()
{
	io_clearAll();
}

/// <summary>
/// Reads a byte from the cartridge data memory to the HL register.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param>
/// <mnemonic>
/// RDAT
/// </mnemonic>
/// <opcode>
/// 75
/// </opcode>
inline void ip_readDataMemory(unsigned short address)
{
    HL = io_readDataMemory(address);
}

/// <summary>
/// Writes a register's value to the cartridge data memory.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param>
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit index.
/// </param>
/// <mnemonic>
/// WDAT
/// </mnemonic>
/// <opcode>
/// 76
/// </opcode>
inline void ip_writeToDataMemory(unsigned short address, unsigned char value)
{
    io_writeToDataMemory(address, reg[value]);
}

/// <summary>
/// Writes a fixed value to the cartridge data memory.
/// </summary>
/// <param name="address">
/// The address parameter requires an unsigned short containing a 16 bit address.
/// </param>
/// <param name="value">
/// The value parameter requires an unsigned short containing an 8 bit value.
/// </param>
/// <mnemonic>
/// WDATI
/// </mnemonic>
/// <opcode>
/// 77
/// </opcode>
inline void ip_writeToDataMemoryImmediate(unsigned short address, unsigned char value)
{
    io_writeToDataMemory(address, value);
}



#ifdef DEBUGASSERT

// Checks if the value of a register is equal to a fixed value.
// If not, execution is halted.
// Accessible through the interpreter as instruction, yet not to be used as such.
// mnemonic: AEQ
// opcode: 252
inline void ip_assertEqual(unsigned char address, unsigned char value)
{
    if(reg[address] != value)
        while(1);
}

// Checks if the value of a register is not equal to a fixed value.
// If not, execution is halted.
// Accessible through the interpreter as instruction, yet not to be used as such.
// mnemonic: ANEQ
// opcode: 253
inline void ip_assertNotEqual(unsigned char address, unsigned char value)
{
    if(reg[address] == value)
        while(1);
}

// Checks if the value of a RAM address is equal to a fixed value.
// If not, execution is halted.
// Accessible through the interpreter as instruction, yet not to be used as such.
// mnemonic: AEQR
// opcode: 254
inline void ip_assertEqualRam(unsigned char address, unsigned char value)
{
    if(RAM[address] != value)
        while(1);
}

// Checks if the value of a RAM address is not equal to a fixed value.
// If not, execution is halted.
// Accessible through the interpreter as instruction, yet not to be used as such.
// mnemonic: ANEQR
// opcode: 255
inline void ip_assertNotEqualRam(unsigned char address, unsigned char value)
{
    if(RAM[address] == value)
        while(1);
}
#endif

//--------------------------------------------------
//INTERPRETER
//--------------------------------------------------

void ip_initInterpreter()
{
    io_initIO();

    programCounter = 0;
    
#ifdef DEBUGASSERT
    trisb = 0;
    trisc = 0;
    portb = 0;
    portc = 0;
    instructionCounter = 0;
#endif
    
    int i = 0;
    for(i = 0; i<16; i++)
    {
        reg[i] = 0;
    }
    for(i = 0; i<256; i++)
    {
        RAM[i] = 0;
    }
    for(i = 0; i<256; i++)
    {
        sprites[i] = 0;
    }
    ip_clearTimer();
}

void ip_startInterpreter()
{
    unsigned char a0;
    unsigned char a1;
    unsigned char a2;
    unsigned char a3;
    unsigned char a4;
    unsigned short a0_16 = 0;

    unsigned char inst = 0x00;
    
    while(programCounter<=65535)
    {
        inst = io_readProgramMemory(programCounter++);
        
        delay_ms(200);
        
#ifdef DEBUGASSERT
        instructionCounter++;
#endif

        switch(inst)
        {
            case 0:
                ip_nop();
                break;
            case 1:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_load(a0, a1);
                break;
            case 2:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_copy(a0,a1);
                break;
            case 3:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_store(a0,a1);
                break;
            case 4:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_storeImmediate(a0,a1);
                break;
            case 5:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_loadRam(a0,a1);
                break;
            case 6:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_compare(a0,a1);
                break;
            case 7:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_greaterThan(a0,a1);
                break;
            case 8:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_greaterThanImmediate(a0,a1);
                break;
            case 9:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_lessThan(a0,a1);
                break;
            case 10:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_lessThanImmediate(a0,a1);
                break;
            case 11:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_swap(a0,a1);
                break;
            case 12:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_swapRam(a0,a1);
                break;
            case 13:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_add(a0,a1);
                break;
            case 14:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_addImmediate(a0,a1);
                break;
            case 15:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_subtract(a0,a1);
                break;
            case 16:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_subtractImmediate(a0,a1);
                break;
            case 17:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_multiply(a0,a1);
                break;
            case 18:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_multiplyImmediate(a0,a1);
                break;
            case 19:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_divide(a0,a1);
                break;
            case 20:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_divideImmediate(a0,a1);
                break;
            case 21:
                a0 = io_readProgramMemory(programCounter++);
                ip_increment(a0);
                break;
            case 22:
                a0 = io_readProgramMemory(programCounter++);
                ip_decrement(a0);
                break;
            case 23:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_power(a0,a1);
                break;
            case 24:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_powerImmediate(a0,a1);
                break;
            case 25:
                a0 = io_readProgramMemory(programCounter++);
                ip_squareRoot(a0);
                break;
            case 26:
                a0 = io_readProgramMemory(programCounter++);
                ip_log2(a0);
                break;
            case 27:
                a0 = io_readProgramMemory(programCounter++);
                ip_log10(a0);
                break;
            case 28:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_rand(a0,a1);
                break;
            case 29:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_randImmediate(a0,a1);
                break;
            case 30:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_bitwiseAnd(a0,a1);
                break;
            case 31:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_bitwiseAndImmediate(a0,a1);
                break;
            case 32:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_bitwiseOr(a0,a1);
                break;
            case 33:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_bitwiseOrImmediate(a0,a1);
                break;
            case 34:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_bitwiseXor(a0,a1);
                break;
            case 35:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_bitwiseXorImmediate(a0,a1);
                break;
            case 36:
                a0 = io_readProgramMemory(programCounter++);
                ip_bitwiseNot(a0);
                break;
            case 37:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_setBit(a0,a1);
                break;
            case 38:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_clearBit(a0,a1);
                break;
            case 39:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_jump(a0_16);
                break;
            case 40:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_jumpRelative(a0_16);
                break;
            case 41:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_jumpIfZero(a0_16);
                break;
            case 42:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_jumpRelativeIfZero(a0_16);
                break;
            case 43:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_jumpIfNotZero(a0_16);
                break;
            case 44:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_jumpRelativeIfNotZero(a0_16);
                break;
            case 45:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_call(a0_16);
                break;
            case 46:
                ip_ret();
                break;
            case 47:
                a0 = io_readProgramMemory(programCounter++);
                ip_push(a0);
                break;
            case 48:
                a0 = io_readProgramMemory(programCounter++);
                ip_pushImmediate(a0);
                break;
            case 49:
                a0 = io_readProgramMemory(programCounter++);
                ip_pop(a0);
                break;
            case 50:
                a0 = io_readProgramMemory(programCounter++);
                ip_buttonRead(a0);
                break;
            case 51:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_buttonReadSingle(a0, a1);
                break;
            case 52:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_waitFor(a0, a1);
                break;
            case 53:
                ip_screenHeight();
                break;
            case 54:
                ip_screenWidth();
                break;
            case 55:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_setSprite(a0, a1);
                break;
            case 56:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                a2 = io_readProgramMemory(programCounter++);
                ip_updateSprite(a0, a1, a2);
                break;
            case 57:
                a0 = io_readProgramMemory(programCounter++);
                ip_clearSprite(a0);
                break;
            case 58:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_checkSpriteX(a0, a1);
                break;
            case 59:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_checkSpriteY(a0, a1);
                break;
            case 60:
                a0 = io_readProgramMemory(programCounter++);
                ip_setMap(a0);
                break;
            case 61:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                a2 = io_readProgramMemory(programCounter++);
                ip_setString(a0, a1, a2);
                break;
            case 62:
                a0 = io_readProgramMemory(programCounter++);
                ip_delay(a0);
                break;
            case 63:
                a0 = io_readProgramMemory(programCounter++);
                ip_delayImmediate(a0);
                break;
            case 64:
                ip_clearTimer();
                break;
            case 65:
                ip_readTimer();
                break;
            case 66:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_callIfNotZero(a0_16);
                break;
            case 67:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_callIfZero(a0_16);
                break;
            case 68:
                allOn();
                break;
            case 69:
                allOff();
                break;
            case 70:
                invertedMode();
                break;
            case 71:
                normalMode();
                break;
            case 72:
                redraw();
                break;
            case 73:
				clearAll();
				break;
			case 74:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                a2 = io_readProgramMemory(programCounter++);
                ip_updateSpriteImmediate(a0, a1, a2);
                break;
            case 75:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                ip_readDataMemory(a0_16);
                break;
            case 76:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                a2 = io_readProgramMemory(programCounter++);
                ip_writeToDataMemory(a0_16, a2);
                break;
            case 77:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                MAKESHORT(a0_16, a1, a0);
                a2 = io_readProgramMemory(programCounter++);
                ip_writeToDataMemoryImmediate(a0_16, a2);
                break;
#ifdef DEBUGASSERT
            case 252:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_assertEqual(a0, a1);
                break;
            case 253:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_assertNotEqual(a0, a1);
                break;
            case 254:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_assertEqualRam(a0, a1);
                break;
            case 255:
                a0 = io_readProgramMemory(programCounter++);
                a1 = io_readProgramMemory(programCounter++);
                ip_assertNotEqualRam(a0, a1);
                break;
#endif
                default:
                    ip_nop();
                    break;
        }
    }
}