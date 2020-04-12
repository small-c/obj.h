#include "animal.h"

int main()
{
    // Create 3 animals
    Animal cat = newAnimal("hunter cat", 8);
    Animal dog = newAnimal("super dog", 23);
    Animal cow = newAnimal("lazy cow", 299);

    // Let them speak
    cat->speak();
    dog->speak();
    cow->speak();

    // ...
    threesomeAnimal(cat, dog, cow);

    // Release them
    freeAnimal(cat);
    freeAnimal(dog);
    freeAnimal(cow);

    return 0;
}
