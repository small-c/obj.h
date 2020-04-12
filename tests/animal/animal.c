#include "animal.h"

// void Animal::speak()
method(Animal, void, speak)()
{
    obj_prepare(Animal);

    printf("# The %s:\n"
        "  + Wt. %d\n"
        "  + Pw. %d\n",
        self->name,
        self->weight,
        self->power
    );
}

// Animal::Animal(const char *, int)
ctor(Animal)(const char *name, int weight)
{
    obj_setup(Animal);
    obj_bind(Animal, speak);

    // Self allocate name
    size_t len = strlen(name);
    self->name = self->base.alloc(len + 1);
    memcpy(self->name, name, len);
    self->name[len] = '\0';

    // Init properties
    self->power = 0;
    self->weight = weight;

    // Set power by name
    for (int i = 0; i < len; i++)
        self->power += name[i] + ((i % 2 == 0) ? weight : 0);

    obj_done(Animal);
}

// Wrap constructor
Animal newAnimal(const char *name, int weight)
{
    return new(Animal)(name, weight);
}

// Wrap destructor
void freeAnimal(Animal self)
{
    self->base.release();
}

// Just a threesom'in
void threesomeAnimal(Animal x, Animal y, Animal z)
{
    char name[100];
    memset(name, '\0', sizeof(name));

    strcat(name, x->name);
    strcat(name, z->name);
    strcat(name, y->name);

    for (int i = 0; name[i]; i++)
        if (name[i] == ' ')
            name[i] = '-';

    int weight = (x->weight % 2) +
        (y->weight % 3) + z->weight;

    printf("Creating a new animal...\n");
    Animal child = newAnimal(name, weight);
    child->speak();

    freeAnimal(child);
}
