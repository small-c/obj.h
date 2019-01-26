# wobj
[https://wobj.wy3.space](https://wobj.wy3.space)<br>
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

### allocate memory with GC

```c
// internal
void *wobj_alloc   (size_t size);           // value is set by zero
void *wobj_malloc  (size_t size);           // like malloc
void *wobj_calloc  (size_t count, size_t size);    // like calloc
void *wobj_ralloc  (void *ptr, size_t new_size);  // like realloc, if size == 0 then memory would be freed
void  wobj_unalloc (void *ptr);            // like free, but the name is `unalloc`

// external
void *wobj_alloco   (name, var_name, size_t size);
void *wobj_malloco  (name, var_name, size_t size);
void *wobj_calloco  (name, var_name, size_t count, size_t size);
void *wobj_ralloco  (name, var_name, void *ptr, size_t new_size);
void  wobj_unalloco (name, var_name, void *ptr);
```

### example

- Simple string object, based on [wobj](https://github.com/small-c/wobj) & **C string**: [strs](https://github.com/small-c/strs)

- Comparison between **C** and **C++**, see [**test.c**](https://github.com/wy3/wobj/blob/master/tests/test.c) for details.

```c++
// C + wobj                                   |  // C++
                                              |
wobj(Dog, {                                   |  class Dog {
                                              |  public:
                                              |      Dog(const char *name, int weight);
                                              |      ~Dog();
    char   *name;                             |      char   *name;
    wobj_fn(void, speak, (void));             |      void   speak(void);
}, {                                          |  private:
    int    power;                             |      int    power;
})                                            |  };
                                              |
wobj_def(Dog, void, speak, (void), {          |  void Dog::speak(void) {
    printf("The %s, power: %d.\n",            |      printf("The %s, power: %d.\n",
        self->name, self->power);             |          this->name, this->power);
})                                            |  }
                                              |  
wobj_init(Dog, (const char* name), {          |  Dog::Dog(const char *name) {
    wobj_set(Dog, speak);                     |      
    self->power = 999 * name[0];              |      this->power = 999 * name[0];
    size_t len = strlen(name);                |      size_t len = strlen(name);
    self->name = wobj_alloc(len + 1);         |      this->name = new char[len + 1]();
    memcpy(self->name, name, len);            |      memcpy(this->name, name, len);
                                              |      self->name[len] = '\0';
},                                            |  }
                                              |
{                                             |  Dog::~Dog() {
                                              |      delete this->name;
})                                            |  }
                                              |  
int main(void) {                              |  int main(void) {
    wobj_new(Dog, dog_foo, "Crazy Dog!");     |      Dog *dog_foo = new Dog("Crazy Dog!");
    dog_foo->speak();                         |      dog_foo->speak();
                                              |      
    wobj_free(Dog, dog_foo);                  |      detele dog_foo;
    return 0;                                 |      return 0;
}                                             |  }
```

### License: MIT
