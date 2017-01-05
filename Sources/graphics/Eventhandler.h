#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <system.h>
#include "CommandControl.h"
//#include every .h file that can generate an event

void eventhandler(void);

#endif

/* How to work with the eventhandler?********************************
**************************************************
KEEP ALL INTERRUPT RELATED STUFF IN THE EVENTHANDLER.
ALL INTERRUPT RELATED BITS MUST BE CHANGED HERE AND
NOWHERE ELSE
**************************************************

ALL ITEMS WITH => NEED ACTION FROM THE PROGRAMMER. THE REST
WORKS AUTOMATICALLY.

1) When an interrupt occurs, the function "void interrupt(void)"
in main.c will be called. There is only one line of code in this
function: eventhandler();

2) eventhandler() is called.

3) eventhandler switches off the GIE (global interrupt enable)

4) eventhandler checks who generated the interrupt

5) => eventhandler calls the appropriate function of the block that
generated the interrupt.
(if two blocks would use the same interrupt, it will be detected here.
Solve your issue together!)

6) => the appropriate function does the necessary event handling

7) => the appropriate function calls a middleware event function, that
on its turn calls the state machine to set a flag that signals the event

8) => return of the middleware function

9) => return of the appropriate function of the block that
generated the interrupt

10) Back in eventhandler function, the correct flag will be automatically
cleared.

11) Eventhandler, switches the GIE back on

12) return of the eventhandler

13) return of the interrupt function in main.c

14) => state machine will control the flags on a polling basis and
switches state in response to the flags that were set.

**********************************************************************/