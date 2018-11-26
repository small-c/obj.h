# wobj [![Build Status](https://travis-ci.org/wy3/wobj.svg?branch=master)](https://travis-ci.org/wy3/wobj)
**C oriented object based on struct and [closure function](https://github.com/yulon/clofn).**

- [x] Easy to use, fast and high performance
- [x] Crossplatform: Win32 / Linux / OSX
- [x] Supported GC

### usage
Add `#include "wobj.h"` to your source code.

### marco

Declare new object
```c
wobj(name, body);
```

Define method function
```c
wobj_def(name, type, func, args, body)
```

Init object
```c
wobj_init(name, args_new, body_init, body_free)
```

Set method function (in init)
```c
wobj_set(name, func)
```

Allocate memory for object, it has auto GC, no need to free()
```c
wobj_alloc(name, size)
```

Create new object
```c
wobj_new(name, var_name, ...)
```

Free object and call GC
```c
wobj_free(name, var_name)
```

### my GC

```
alloc: mem -> node[mem, next]
                |
                v
free:  static.next-->|
          ^          |
          |          |
          |__________|
```

### example
Comparison between **C** and **C++**, see [**test.c**](https://github.com/wy3/wobj/blob/master/test.c) for details.

```c++
// C                                                  |  // C++
#include "wobj.h"                                     |  #include <stdio.h>
                                                      |  #include <stdlib.h>
                                                      |  #include <stdint.h>
                                           // declare |
wobj(Dog, {                                           |  class Dog {
    uint32_t weight;                                  |  public:
    const char *name;                                 |      Dog(const char *name, uint32_t weight);
    void(*speak)();                                   |      ~Dog();
});                                                   |      uint32_t weight;
                                                      |      const char *name;
                                                      |      void speak();
                                                      |  };
                                            // method |  
wobj_def(Dog, void, speak, (void), {                  |  void Dog::speak() {
    printf("I'm %s, my weight is %dkg.\n",            |      printf("I'm %s, my weight is %dkg.\n",
        self->name, self->weight);                    |          this->name, this->weight);
})                                                    |  }
                                             // init  |  
wobj_init(Dog, (const char* name, uint32_t weight), { |  Dog::Dog(const char *name, uint32_t weight) {
    wobj_set(Dog, speak);                             |      this->name = name;
    self->name = name;                                |      this->weight = weight;
    self->weight = weight;                            |  }
}, {                                          // free |  
                                                      |  Dog::~Dog() {
})                                                    |  
                                                      |  }
                                              // main |  
int main(void) {                                      |  int main(void) {
    const char* name = "Crazy Dog!";                  |      const char *name = "Crazy Dog!";
    wobj_new(Dog, dog_foo, name, 32);                 |      Dog *dog_foo = new Dog(name, 32);
    dog_foo->speak();                                 |      dog_foo->speak();
                                                      |      
    wobj_free(Dog, dog_foo);                          |      detele dog_foo;
    return 0;                                         |      return 0;
}                                                     |  }
```
