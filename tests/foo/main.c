#include "wobj.h"

wobj(Foo,
{
    int bar;
    wobj_fn(void, set, (int value));
    wobj_fn(int, get, (void));
}, {
    int value;
}, _Foo)

wobj_def(Foo, void, set, (int value), {
    self->value = value;
})

wobj_def(Foo, int, get, (void), {
    return self->value;
})

wobj_init(Foo, (int bar), {

    wobj_set(Foo, set);
    wobj_set(Foo, get);

    self->bar = bar;

}, {
    printf("\nfreeing, Foo = {\n"
        "public:\n"
        "    bar = %d;\n"
        "private:\n"
        "    value = %d;\n"
        "}\n\n",
        self->bar, self->value
    );
})

int main()
{
    wobj_new(Foo, my_foo, 15); // bar: 15

    my_foo->set(100);
    printf("bar is %d\n", my_foo->bar);
    printf("got value, %d\n", my_foo->get()); //value: 100
    
    wobj_free(Foo, my_foo);

    puts("done!");
    return 0;
}
