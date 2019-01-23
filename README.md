# wobj
**C oriented object based on struct and [closure function](https://github.com/yulon/clofn).**

[![Build Status](https://travis-ci.org/wy3/wobj.svg?branch=master)](https://travis-ci.org/wy3/wobj)
<br>
<p>
<a href="#"><img src="https://img.shields.io/badge/auto--GC-✓-brightgreen.svg"></a>
<a href="#"><img src="https://img.shields.io/badge/os-cross--platform-blue.svg"></a>
</p>

## usage
Add `#include "wobj.h"` to your source code.

## macros

### Root name (self, this...)

Before `#include ..`, add `#define wobj_this` to use `this` instead of `self`

```c
#define wobj_use_other XXX // to use XXX
#define wobj_this // or `this` or `use_this` to use this
#include "wobj.h"
```

#### Declare object

```c
wobj(name, {
// public:

}, {
// private:

})

wobj_np(name, {
// public
})
```

### Define method

```c
wobj_def(name, return_type, func_name, (args), {
    // function body
})
```

### Init object

```c
wobj_init(name, (args_for_init), {
    // body new
}, {
    // body free
})
```

### Set method function (in init)
```c
wobj_set(name, func)
```

### Create new object
```c
wobj_new(name, var_name, ...)
wobj_new2(name, var_name)(...)
name var_name = wobj_new3(name, ...)
name var_name = wobj_new(name)(...)
```

### Free object and call GC

```c
wobj_free(name, var_name)
```

### Allocate memory with auto GC (internal)

```c
wobj_alloc   (size)           // value is set by zero
wobj_malloc  (size)           // like malloc
wobj_calloc  (count, size)    // like calloc
wobj_unalloc (ptr)            // like free, but the name is `unalloc`
wobj_ralloc  (ptr, new_size)  // like realloc, if size == 0 then memory would be freed
```

### my GC

```
alloc: [ ] -> [object] --+-----------------+-----------------+
position:         ...  3 |               2 |               1 |
                         v                 v                 v
linked list: [...] -> [ mem | next ] -> [ mem | next ] -> [ mem | null ]
                       1 |                 |                 |
                       2 | <---------------+                 |
                       3 | <---------------------------------+
                     ... v 
free: [x] <--------- [object]
```

### example

- Simple string object, based on [wobj](https://github.com/small-c/wobj) & **C string**: [strs](https://github.com/small-c/strs)

- Comparison between **C** and **C++**, see [**test.c**](https://github.com/wy3/wobj/blob/master/test.c) for details.

```c++
// C + wobj                                           |  // C++
#include "wobj.h"                                     |  #include <stdio.h>
                                                      |  #include <stdlib.h>
                                                      |  #include <stdint.h>
                                                      |  
wobj(Dog) {                                           |  class Dog {
                                                      |  public:
                                                      |      Dog(const char *name, uint32_t weight);
                                                      |      ~Dog();
    uint32_t weight;                                  |      uint32_t weight;
    char *name;                                       |      const char *name;
    void(*speak)();                                   |      void speak();
};                                                    |  };
                                                      |
wobj_def(Dog, void, speak, (void), {                  |  void Dog::speak() {
    printf("I'm %s, my weight is %dkg.\n",            |      printf("I'm %s, my weight is %dkg.\n",
        self->name, self->weight);                    |          this->name, this->weight);
})                                                    |  }
                                                      |  
wobj_init(Dog, (const char* name, uint32_t weight), { |  Dog::Dog(const char *name, uint32_t weight) {
    wobj_set(Dog, speak);                             |      
    self->weight = weight;                            |      this->weight = weight;
                                                      |
    int len = strlen(name);                           |      int len = strlen(name);
    self->name = wobj_alloc(Dog, len + 1);            |      this->name = new char[len + 1]();
    memcpy(self->name, name, len);                    |      memcpy(this->name, name, len);
                                                      |      self->name[len] = '\0';
}, {                                                  |  }
    //                                                |
                                                      |  Dog::~Dog() {
                                                      |      delete this->name;
})                                                    |  }
                                                      |  
int main(void) {                                      |  int main(void) {
    wobj_new(Dog, dog_foo, "Crazy Dog!", 32);         |      Dog *dog_foo = new Dog("Crazy Dog!", 32);
    dog_foo->speak();                                 |      dog_foo->speak();
                                                      |      
    wobj_free(Dog, dog_foo);                          |      detele dog_foo;
    return 0;                                         |      return 0;
}                                                     |  }
```

### License: MIT
