#include "wobj.h"

// declare Dog object
wobj(Dog) {
    uint32_t weight; // property
    const char *name;
    void(*speak)(); // method

} /*can define type here*/ ;

// define 'speak' method, like: void Dog.speak(void)
wobj_def(Dog, void, speak, (void), {
    printf("I'm %s, my weight is %dkg.\n", self->name, self->weight);
})

// init Dog
wobj_init(Dog, (const char* name, uint32_t weight), {
    wobj_set(Dog, speak); // set 'speak' for Dog, important!
    // set property
    self->name = name;
    self->weight = weight;
}, { // free
    free(self->speak); // necessary
    free(self);
})

int main(void)
{
    const char* name = "Crazy Dog!";
    wobj_new(Dog, dog_foo, name, 32); // create mydog as Dog object
    // call method
    dog_foo->speak(); // << I'm Crazy Dog, my weight is 32kg.

    wobj_free(Dog, dog_foo); // free dog_foo

    return 0;
}
