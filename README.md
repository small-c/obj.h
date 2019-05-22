<p align="center">
    <h1 align="center">wobj</h1>
    <h4 align="center">
        <i>C Oriented Object</i> with single header file, based on <i>struct</i> and <a href="https://github.com/yulon/clofn" target="_blank"><i>closure function</i></a>
    </h4>
    <p align="center">
        <a href="https://travis-ci.org/wy3/wobj" target="_blank"><img src="https://travis-ci.org/wy3/wobj.svg?branch=master"></a>
        <a href="#"><img src="https://img.shields.io/badge/gc-✓-brightgreen.svg"></a>
        <a href="#"><img src="https://img.shields.io/badge/os-cross--platform-blue.svg"></a>
    </p>
</p>

### features
- Self allocate with garbage collector
- Objects, classes
- Public, private member
- Data abstraction
- Inheritance

### platforms supported

_ | GCC | MSVC | Clang
-|:-:|:-:|:-:
Windows | ✓ | ✓ | ✓
Linux   | x86 / x86_64 | ✗ | x86
Mac OSX | ✓ | ✗ | x86

### my technologies

```elm
$object {
    public:  (...) +---------------> [USER/EXTERNAL]
    private: (...)                         ^
    mem:   [linked list]                   |
} *self;      -- + --         +------------+
     +----->   [ v ]          |
dynamic _      [ v ]  <--+ method: closure [self] (arg_1, arg_2, ...)
allocate +---> [ v ]                  |       \-----> { function }
                ...                   v
#new: init() -> object  | +-----> [INTERNAL]
#free: uninit() + GC()  |
```

### example

- [w2d](https://github.com/wy3/wui/tree/master/w2d) - mini graphics object library for Windows
- Comparison between **C** and **C++**, see [**foo.c**](https://github.com/wy3/wobj/blob/master/tests/foo.c) for details.

```c++
// C + wobj                           |   // C++
#include "wobj.h"                     |   #include <stdio.h>
                                      |   
wobj(FOO,                             |   class FOO {
public (                              |   public:
                                      |       FOO(int bar);
                                      |       ~FOO();
    int  bar;                         |       int  bar;
    void func(set, (int val));        |       void set(int val);
    int  func(get, ());               |       int  get();
),                                    |
private (                             |   private:
    int val;                          |       int val;
))                                    |   }
                                      |   
void wobj_def(FOO, set, (int val), {  |   void FOO::set(int val) {
    self->value = val;                |       this->val = val;
})                                    |   }
                                      |   
int wobj_def(FOO, get, (), {          |   int FOO:get() {
    return self->val;                 |       return this->val;
})                                    |   }
                                      |
wobj_init(FOO, (int bar), {           |   FOO:FOO(int bar) {
    self->bar = bar;                  |       this->bar = bar;
    wobj_set(FOO, get);               |   }
    wobj_set(FOO, set);               |       
}, {                                  |   FOO::~FOO() {
    puts("freeing...");               |        puts("freeing...");
})                                    |   }
                                      |
int main() {                          |   int main() {
    wobj_new(FOO, foo, 10);           |       FOO *foo = new FOO(10);
    foo->set(55);                     |       foo->set(55);
    int bar = foo->bar,               |       int bar = foo->bar,
        val = foo->get();             |           val = foo->get();
    wobj_free(FOO, foo);              |       delete foo;
}                                     |   }
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
wobj_decl(name);

wobj(name,
    public (
    
    ),
    private (
    
    )
)
```

### public from... (please see [shape example](https://github.com/small-c/wobj/tree/master/tests/shape))

```c
wobj_public(public_name);
```

### define method

```c
type wobj_def(name, func_name, (args), {
    // function body
})

// define method/function on struct
type func(func_name, (args, ...));
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
wobj_sets(name, func_1, func_2, ...); // 10 is maximium

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

### License: MIT
- Give me something if it is useful ([Paypal](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=CZ5C58TN4LY4L)).
