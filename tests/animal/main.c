#include <string.h>
#include "animal.h"

void threesomeAnimal(Animal animal_1, Animal animal_2, Animal animal_3)
{
    char name[100] = { 0 };
    strcat(name, animal_1->name);
    strcat(name, animal_3->name);
    strcat(name, animal_2->name);

    for (int i = 0; name[i]; i++)
        if (name[i] == ' ')
            name[i] = '-';

    int weight = (animal_1->weight % 2) + (animal_2->weight % 3) + animal_3->weight;

    printf("Creating a new animal...\n-> ");
    Animal _new = newAnimal(name, weight);
    _new->speak();

    freeAnimal(_new);
}

int main()
{
    Animal cat = newAnimal("hunter cat", 8);
    Animal dog = newAnimal("super dog", 23);
    Animal cow = newAnimal("lazy cow", 299);

    cat->speak();
    dog->speak();
    cow->speak();

    threesomeAnimal(cat, dog, cow);

    freeAnimal(cat);
    freeAnimal(dog);
    freeAnimal(cow);

    puts("done!");
    return 0;
}
