# wobj
**C oriented object** with single header file, based on **struct** and [**closure function**](https://github.com/yulon/clofn).

<p>
    <a href="https://travis-ci.org/wy3/wobj" target="_blank"><img src="https://travis-ci.org/wy3/wobj.svg?branch=master"></a>
    <br>
    <a href="#"><img src="https://img.shields.io/badge/auto--GC-✓-brightgreen.svg"></a>
    <a href="#"><img src="https://img.shields.io/badge/os-cross--platform-blue.svg"></a>
</p>

### features
- Objects, classes
- Public, private member
- Data abstraction
- Inheritance

### my technologies

```elm
class:
    - public:  {...}                             |
    - private: {...}                             |
    - mem:     [linked list]                     |
         ----                                    |
method: [data] cloure(#p1, #p2, ...)             |
           |    > mem   <-- [allocate]           |
           +----> public, private -> [internal] --------+
-----------------+-------------------+-----------+      v
                 |                   v              ----------
                 v               interface: public, init, free
alloc: [ ] -> [object] --+-----------------+-----------------+
position:         ...  3 |               2 |               1 |
                         v                 v                 v
linked list: [...] -> [ mem | next ] -> [ mem | next ] -> [ mem | null ]
                       1 |                 |                 |
                       2 | <---------------+                 |
                       3 | <---------------------------------+
                     ... v 
free:  [x] <-------- [object]
```

## usage
Add `#include "wobj.h"` to your source code.

## macros

### root name (self, this...)

Before `#include ..`, add `#define wobj_this` to use `this` instead of `self`

```c
#define wobj_use_other XXX // to use XXX
#define wobj_this // or `this` or `use_this` to use this
#include "wobj.h"
```

### declare object

```c
wobj(name, {
// public:

}, {
// private:

})

wobj_np(name, {
// public:

})
```

### public from... (please see [shape example](https://github.com/small-c/wobj/tree/master/tests/shape))

```c
wobj_public(public_name);
```

### define method

```c
wobj_def(name, return_type, func_name, (args), {
    // function body
})

// define method/function on struct
wobj_fn(return_type, func_name, (args, ...));
```

### init object

```c
wobj_init(name, (args_for_init), {
    // body new
}, {
    // body free
})
```

### set method function (in init)
```c
wobj_set(name, func);

// set method public from other, see tests/sphape example
wobj_setp(name, public_name, func);
```

### create new object
```c
wobj_new(name, var_name, ...);
wobj_new2(name, var_name)(...);
name var_name = wobj_new3(name, ...);
name var_name = wobj_new(name)(...);
```

### free object and call GC

```c
wobj_free(name, var_name);
```

### allocate memory with auto GC (in init/method)

```c
wobj_alloc   (size);           // value is set by zero
wobj_malloc  (size);           // like malloc
wobj_calloc  (count, size);    // like calloc
wobj_unalloc (ptr);            // like free, but the name is `unalloc`
wobj_ralloc  (ptr, new_size);  // like realloc, if size == 0 then memory would be freed
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
wobj(Dog, {                                           |  class Dog {
                                                      |  public:
                                                      |      Dog(const char *name, int weight);
                                                      |      ~Dog();
    int    weight;                                    |      int    weight;
    char   *name;                                     |      char   *name;
    void  (*speak)();                                 |      void   speak();
}, {                                                  |  private:
    int    power;                                     |      int    power;
})                                                    |  };
                                                      |
wobj_def(Dog, void, speak, (void), {                  |  void Dog::speak() {
    printf("The %s, weight: %d, power: %d.\n",        |      printf("The %s, weight: %d, power: %d.\n",
        self->name, self->weight, self->power);       |          this->name, this->weight, this->power);
})                                                    |  }
                                                      |  
wobj_init(Dog, (const char* name, int weight), {      |  Dog::Dog(const char *name, int weight) {
    wobj_set(Dog, speak);                             |      
    self->weight = weight;                            |      this->weight = weight;
    self->power = 999 * weight;                       |      this->power = 999 * weight;
    size_t len = strlen(name);                        |      size_t len = strlen(name);
    self->name = wobj_alloc(Dog, len + 1);            |      this->name = new char[len + 1]();
    memcpy(self->name, name, len);                    |      memcpy(this->name, name, len);
                                                      |      self->name[len] = '\0';
},                                                    |  }
                                                      |
{                                                     |  Dog::~Dog() {
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
