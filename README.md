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
    <h1></h1>
    <p align="center">
        <i>No programming technique solves all problems.</i><br>
        <i>No programming language produces only correct results.</i><br>
        <i>No programmer should start each project from scratch.</i><br>
        <i>" OOC "</i>
        <br>
        <i>C++, Java, C# or other has OOP, why C does not?</i><br>
    </p>
    <h1></h1>
</p>

### features
- Self allocate with garbage collector
- Objects, classes
- Public, private member
- Data abstraction
- Inheritance

### my technologies

```elm
class:                                       /* God */
    - public:  {...}                             |
    - private: {...}                             |
    - mem:     [linked list]                     |
         ----                                    |
method: [data] cloure(#p1, #p2, ...)             |
           |    > mem   <-- [allocate]           v
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
        val = foo->get();             |           val = foo->get;
    wobj_free(FOO, foo);              |       delete foo;
}                                     |   }
```

### License: MIT
- Give me something if it is useful ([click here](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=CZ5C58TN4LY4L)).
