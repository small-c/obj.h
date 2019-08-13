#include "wobj.h"

wobj(Animal,
    public (
        char  *name;
        int   weight;
        void  func(speak, ());
    ),
    private (
        int   power;
    )
);

Animal newAnimal(const char *name, int weight);
void freeAnimal(Animal animal);
