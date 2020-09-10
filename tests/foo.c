#include "obj.h"

// Class declaration
classdef(Foo);

// Class definition
class(Foo,
    public
    (
        int  (*get)();
        void (*set)(int value);
    ),
    private
    (
        int bar;
    )
);

// Forward declarations
// Notes: on GNU compiler, these declarations must
//   be placed here (after class definition), but MSVC not.
ctor(Foo)(int bar);                 // Constructor
dtor(Foo);                          // Destructor (optional)
method(Foo, int, get)();            // Method
method(Foo, void, set)(int val);    // Method

// Constructor definition
ctor(Foo)(int bar)
{
    // IMPORTANT! To initialize instance
    obj_setup(Foo);
    // Bind destructor (optional)
    obj_dtor(Foo);
    // IMPORTANT! Bind methods, can call obj_bind upto 14 params at once
    obj_bind(Foo, set, get);

    // Set bar from arg
    self->bar = bar;
    
    // Self allocate
    void *some = self->base.alloc(16);
    // 'some' is managed, no need to free and DO NOT use free()!
    // But you can free it immediately by using
    // self->base.free(some);
    
    // Just reallocate it, upto 48 bytes
    some = self->base.realloc(some, 48);
    memset(some, '\0', 48);
    
    // self->base.free(some);

    // IMPORTANT! For error handing
    obj_done(Foo);
}

// Destructor definition (optional)
dtor(Foo)
{
    // Just body here, no macro requires
    printf("freeing... instance %p\n", self);
}

// Method Foo::get
method(Foo, int, get)()
{
    // IMPORTANT! To prepare self
    obj_prepare(Foo);

    return self->bar;
}

// Method Foo::set
method(Foo, void, set)(int val)
{
    // IMPORTANT! To prepare self
    obj_prepare(Foo);

    self->bar = val;
}

int main()
{
    // Create new object instance
    Foo foo = new(Foo)(10);

    // Test methods call
    printf("bar: %d\n", foo->get());
    foo->set(55);
    printf("re-bar: %d\n", foo->get());

    // Release object with base.release
    foo->base.release();
}
