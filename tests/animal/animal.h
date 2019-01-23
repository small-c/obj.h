#include "wobj.h"

wobj(Animal, {
// public:
    char    *name;
    int     weight;

    wobj_fn(void, speak, (void));

}, {
// private:
    int     power;

});

Animal newAnimal(const char *name, int weight);
void freeAnimal(Animal animal);
