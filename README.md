<p align="center">
    <h1 align="center">obj.h</h1>
    <h4 align="center">
        A single-header supports oriented object programming in pure C
       <br>
        - <i>just a hacking on assembly</i> -
    </h4>
    <p align="center">
        <a href="https://travis-ci.org/wy3/wobj" target="_blank"><img src="https://travis-ci.org/small-c/obj.h.svg?branch=master"></a>
        <a href="#"><img src="https://img.shields.io/badge/cc-multiple-blue.svg"></a>
        <a href="#"><img src="https://img.shields.io/badge/sloc-375-lightgrey.svg"></a>
    </p>
</p>

```cpp
Foo f = new(Foo)(10);
assert(f->get() == 10);
f->base.release();
```

[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/L3L6W74V)

### Features
- Simple syntax (inspired by **C++** and **Java**)
- Classes, objects
- Public, private members
- Constructor, destructor
- Data abstraction
- Inheritance

### Usage

- Just add `obj.h` to your **C** source.
- See tests for more.

### Compilers supported

OS\CC (minimum)  | GCC 4 | MSVC 14 | Clang 5
:-------|:---:|:----:|:----:
Windows | ✅  | ✅   | ✅
Linux   | x86 / x86_64 ✓ | _ | x86 ✓
Mac OSX | ✅  | _    | ✅

### How it works?

```elm
$object {
    public:  (...) +---------------> [USER/EXTERNAL]
    private: (...)                         ^
    mem:    [linkedlist]                   |
} *self;      -- + --         +------------+
     +----->     |            |
dynamic _        |    <--+ method: closure [self] (args, ...)
allocate +--->   |                    |       \-----> { function }
                 v                    v
constructor -> object  | +-----> [INTERNAL]
destructor() ->  X
```
#### Closure function?
- This is a fork of [clofn](https://github.com/yulon/clofn).
- We just copy function header and inject some code to pre-allocate **self** inside method.
- And use power of **C macro** and struct to provide `obj.h`.
- Currently, support `x86` and `x86_64` only.

```asm
; on x86
jmp  $addr

; on x86_64
push rax
mov  rax, $addr
jmp  rax
```

### C++ comparison
```c++
// C++ native OOP               // C with obj.h
class Foo {                 |   class(Foo, public(
public:                     |       int (* get)()
    Foo(int bar);           |   ), private(
    int get();              |       int bar;
private:                    |   ));
    int bar;                |
}                           |   ctor(Foo)(int bar) {
                            |       obj_setup(Foo);
Foo::Foo(int bar) {         |       obj_bind(Foo, get);
    this->bar = bar;        |       self->bar = bar;
}                           |       obj_done(Foo);
                            |   }
int Foo::get() {            |
    return this->bar;       |   method(Foo, int, get)() {
}                           |       obj_prepare(Foo);
                            |       return self->bar;
Foo *f = new Foo(15);       |   }
f->get();                   |
delete f;                   |   Foo f = new(Foo)(15);
                            |   f->get();
                            |   f->base.release();
```
