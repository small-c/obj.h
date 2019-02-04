#include "wobj.h"

wobj(Test,
    {
        const int value;
        
        wobj_fn(void*, get, ());
        wobj_fn(void, kill, ());
    },
    {
        void *reserved;
    }
)

wobj_def(Test, void*, get, (),
    {
        return self->reserved;
    }
)

wobj_def(Test, void, kill, (),
    {
        puts("kill itself");
        wobj_free(Test, self);
    }
)

wobj_init(Test, (void),
    {
        wobj_set(Test, get);
        wobj_set(Test, kill);

        *(int*)&self->value = rand();
        self->reserved = malloc(sizeof(32));
    },
    {
        puts("freeing");
        free(self->reserved);
    }
)

int main() {

    Test it = wobj_new4(Test)();

    printf("%d\n", it->value);
    it->get();
    it->kill();
    
    puts("done!");
    return 0;
}
