#include "animal.h"

wobj_def(Animal, void, speak, (void),
{
    printf("The %s :: weight: %d, power: %d.\n",
        self->name,
        self->weight,
        self->power
    );
})

wobj_init(Animal, (const char *name, int weight),
{
    wobj_set(Animal, speak);

    size_t len = strlen(name);
    self->name = wobj_alloc(len + 1);
    memcpy(self->name, name, len);

    self->weight = weight;
    self->power = 0;

    for (int i = 0; i < len; i++)
        self->power += name[i] + ((i % 2 == 0) ? weight : 0);
},
{

})

Animal newAnimal(const char *name, int weight)
{
    return wobj_new4(Animal)(name, weight);
}

void freeAnimal(Animal animal)
{
    wobj_free(Animal, animal);
}
