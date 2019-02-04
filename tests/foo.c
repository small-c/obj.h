#include "wobj.h"

// declare FOO object
wobj(FOO,
    public
    (
        int bar;
        void func(set, (int value));
        int func(get, ());
    ),
    private
    (
        int val;
    )
)

// like: void FOO::set(int val)
void wobj_def(FOO, set, (int val),
    {
        self->val = val;
    }
)

// like: int FOO::get()
int wobj_def(FOO, get, (),
    {
        return self->val;
    }
)

wobj_init(FOO, (int bar),
    { // initialize, FOO::FOO(int bar)

        // set method
        wobj_set(FOO, get);
        wobj_set(FOO, set);

        self->bar = bar; // set bar
        void *ptr = wobj_malloc(16); // self-allocate with GC, no need to free
    },
    { // freeing, FOO::~FOO()

        puts("freeing...");
    }
)

int main()
{
    // create `foo` as FOO object
    wobj_new(FOO, foo, 10);

    // set private val = 55, via set()
    foo->set(55);

    int bar = foo->bar; // get public bar
    int val = foo->get(); // get private val, via get()

    printf("bar = %d\n", bar);
    printf("val = %d\n", val);

    // free `foo`
    wobj_free(FOO, foo);

    puts("done!");
}
