#include <stdio.h>
#include <string.h>

struct Actor{
    char label[30]; // What kind of actor is it?
    byte x_position; // Current x-location
    byte y_position; // Current y-location
    sbyte dx; // Delta-x
    sbyte dy; // Delta-y
    bool is_alive; // Is the actor supposed to be 'alive' right now?
    byte lives; // How many lives does it have? (If N/A, then 0.)
}