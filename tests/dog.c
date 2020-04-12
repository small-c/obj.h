#include "obj.h"

classdef(Dog);
class(Dog,
    public
    (
        const char *sound;
        void (* say)();
    )
    // No private
);

classdef(LoudDog);
class(LoudDog,
    public
    (
        // Just extends Dog
        extend(Dog);
    )
);

// Dog::say()
method(Dog, void, say)()
{
    obj_prepare(Dog);

    printf("I say %s.\n",
        self->sound);
}

// LoudDog::Dog::say()
method(LoudDog, void, say)()
{
    obj_prepare(LoudDog);

    printf("%s .. %s.\n",
        self->Dog.sound,
        self->Dog.sound);
}

// Dog::Dog(const char *)
ctor(Dog)(const char *sound)
{
    obj_setup(Dog);
    // Bind method say
    obj_bind(Dog, say);
    
    // Set sound
    self->sound = sound;

    obj_done(Dog);
}

// LoudDog::LoudDog(const char *)
ctor(LoudDog)(const char *sound)
{
    obj_setup(LoudDog);
    // Override method say
    obj_override(LoudDog, Dog, say);

    // Set sound
    self->Dog.sound = sound;

    obj_done(LoudDog);
}

int main(void)
{
    // Create Dog instance, with 'woof' sound
    Dog dog = new(Dog)("woof");
    // Create LoudDog instance, with 'ruff' sound
    LoudDog lddog = new(LoudDog)("ruff");

    // Call methods
    dog->say();
    lddog->Dog.say();

    // Release objects
    dog->base.release();
    lddog->base.release();
}
