#include "wobj.h"

// declare Dog object, has no private
wobj(Dog,
    public
    (
        uint32_t weight; // property
        char *name;
        void func(speak, ()); // method
    )
);

// define 'speak' method
void wobj_def(Dog, speak, (void),
    {
        printf("I'm %s, my weight is %dkg.\n",
            self->name, self->weight);
    }
)

// init Dog
wobj_init(Dog, (const char* name, uint32_t weight), {
    // set method
    wobj_set(Dog, speak); // set 'sepak' method

    // set property
    self->weight = weight;

    size_t len = strlen(name);
    // allocate by `wobj_*alloc*`
    self->name = wobj_alloc(len + 1); // if want to free, must use wobj_unalloc(), do not use free()
    memcpy(self->name, name, len); // copy
}, {
    // on free, called by wobj_free()  
})

int main(void) {
    // create mydog as Dog object
    wobj_new(Dog, dog_foo, "Crazy Dog!", 32);
    // call method
    dog_foo->speak();
    // << I'm Crazy Dog!, my weight is 32kg.
    wobj_free(Dog, dog_foo); // free dog_foo

    puts("done!");
}
