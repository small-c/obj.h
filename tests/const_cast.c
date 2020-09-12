#include "obj.h"

// Access public property is faster than getter.
// So const_cast helps you modify value of const property.

classdef(Val);

class(Val,
    public
    (
        // Property 'value' with const qualifier
        const int value;
        void (* set)(int);
    ),
    private
    (
    )
);

// Setter
method(Val, void, set)(int value)
{
    obj_prepare(Val);

    // self->value = value; => error
    // Assign const prop by using 'const_cast'
    const_cast(int, value) = value;
}

// Constructor
ctor(Val)(int init)
{
    obj_setup(Val);
    obj_bind(Val, set);

    // Call setter
    self->set(init);

    obj_done(Val);
}

int main()
{
    // Create object
    Val my_val = new(Val)(15);

    // Get init value
    printf("old val: %d\n", my_val->value);

    // Set new value and print out
    // my_val->value = 52; => error
    my_val->set(52);
    printf("new val: %d\n", my_val->value);

    return 0;
}