#include "obj.h"

classdef(Animal);

class(Animal,
    public(
        int  weight;
        char *name;
        void (* speak)();
    ),
    private(
        int   power;
    )
);

// Wrappers
Animal newAnimal(const char *name, int weight);
void   freeAnimal(Animal self);

void threesomeAnimal(Animal x, Animal y, Animal z);
