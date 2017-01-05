#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <system.h>

//--------------------------------------------------
//HEADERS
//--------------------------------------------------

/// <summary>
/// Inits the interpreter by resetting all its properties.
/// This should be called before each ip_startInterpreter().
/// </summary>
void ip_initInterpreter();

/// <summary>
/// Starts the actual interpreter.
/// Before each ip_startInterpreter call, ip_initInterpreter should be called unless the state of the interpreter is to be saved.
/// To be able to execute code, a RAM module should be attached to the controller containing the code, sprites, maps and strings, carefully mapped to the memory.
/// define DEBUGASSERT at the top to enable assert functions to be used, turned off by default.
/// These asserts can check if a method actually does what it is supposed to do, and can help debugging of written code.
/// </summary>
void ip_startInterpreter();

#endif