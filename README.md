obj.h
===
A single-header supports OOP in pure C.<br>
Using power of preprocessor and hacking on assembly to unlock the limits.

![](https://travis-ci.org/small-c/obj.h.svg?branch=master)

```cpp
Foo f = new(Foo)(10);       // create Foo instance
assert(f->get() == 10);     // get value
f->base.release();          // release
```

## Features
- [x] C++/Java inspired OOP
    - [x] Public, private members
    - [x] Constructor, destructor
    - [x] Abstraction
    - [x] Inheritance
- [x] Zero dependency
- [x] No breaking editor's intellisense
- [x] `x86/_64` target
- [ ] `arm` target

## Usage

- Just add `obj.h` to your **C** source
- See [tests](./tests/) for more

#### C++ comparison:
```c++
// C++ native OOP               // C with obj.h
class Foo {                 |   class(Foo, public(
public:                     |       int (*get)();
    Foo(int bar);           |   ), private(
    int get();              |       int bar;
private:                    |   ));
    int bar;                |
};                           |   ctor(Foo)(int bar) {
                            |       obj_setup(Foo);
Foo::Foo(int bar) {         |       obj_bind(Foo, get);
    this->bar = bar;        |       self->bar = bar;
}                           |       obj_done(Foo);
                            |   }
int Foo::get() {            |
    return this->bar;       |   method(Foo, int, get)() {
}                           |       obj_prepare(Foo);
                            |       return self->bar;
                            |   }
                            |
Foo *f = new Foo(15);       |   Foo f = new(Foo)(15);
f->get();                   |   f->get();
delete f;                   |   f->base.release();
```

#### Platform support:
|                         | GCC 4+  | MSVC 14+ | Clang 5+ | TCC 0.9
|:------------------------|:-------:|:--------:|:--------:| :-----:
|Windows (x86 / x64)      | ✅      | ✅      | ✅      | ✅
|Linux   (i386 / x86_x64) | ✅      | _        | ✅      | ✅
|Mac OSX (i386 / x86_64)  | ✅      | _        | ✅      | _

- On **Visual Studio 2017 15.8+**, please disable [**Just My Code debugging**](https://docs.microsoft.com/en-us/cpp/build/reference/jmc)

## How it works?

> We can't explain in detail, but something like binding `this` to a function in **JavaScript**.

Simulate a simple class with `struct`:

```c
struct A {
    void (* todo)();  // method
};
```

And we have a static function:

```c
static void fn_todo() {}
```

Next, bind `A` instance (aka `this`) to `fn_todo` 🙄

```c
binded_todo = bind(fn_todo, myA);
```

Finally 😎
```c
myA->todo = binded_todo;
myA->todo();    // call it like a method
```

#### Closure function?
- This is a fork of [yulon/clofn](https://github.com/yulon/clofn)
- Just copy function header and inject some code to pre-allocate **this** inside
- Currently, support `x86` and `x86_64` only

Function template:
```c
static void fn_todo() {
    volatile size_t self = ...;
    ...
```

Disassemble:
```asm
; prolog
    mov     rax, ...
    mov     QWORD PTR [rbp-8], rax
    ...
```

Generated function:
```asm
 [copied prolog]
> x86
| mov     eax, [data]
| jmp     [addr]
> x86_64
| mov     rax, [data]
| push    rax
| mov     rax, [addr]
| jmp     rax
```

Refs:
- [https://en.wikipedia.org/wiki/Function_prologue_and_epilogue](https://en.wikipedia.org/wiki/Function_prologue_and_epilogue)
- [https://stackoverflow.com/a/9819716](https://stackoverflow.com/a/9819716)

#### How we provide classes, public/private?

See [obj.h](./obj.h) for more.

<br>

[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/L3L6W74V)
