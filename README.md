<p align="center">
    <h1 align="center">obj.h</h1>
    <h4 align="center">
        A single-header supports oriented object programming in pure C
       <br>
        - <i>just a hacking on assembly</i> -
    </h4>
    <p align="center">
        <a href="https://travis-ci.org/small-c/obj.h" target="_blank"><img src="https://travis-ci.org/small-c/obj.h.svg?branch=master"></a>
    </p>
</p>

```cpp
Foo f = new(Foo)(10);
assert(f->get() == 10);
f->base.release();
```

### Features
- Support **OOP** in pure **C** (inspired by **C++** and **Java**).
- Support classes, objects
- Support cublic, private members
- Support constructor, destructor
- Support data abstraction
- Support inheritance
- No dependencies

### Usage

- Just add `obj.h` to your **C** source.
- See tests for more.

### Platform support

|        | GCC 4+  | MSVC 14+ | Clang 5+
|:-------|:-------:|:--------:|:----:
|Windows | ✅      | ✅      | ✅
|Linux   | x86 / x86_64   | _ | x86
|Mac OSX | ✅      | _        | ✅

### How it works?

```elm
$object {
    public:  (...) +---------------> [USER/EXTERNAL]
    private: (...)                         ^
    mem:      [pool]                       |
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
- Just copy function header and inject some code to pre-allocate **self** inside method.
- Using power of **C macro** and **struct** to provide `obj.h`.
- Currently, support `x86` and `x86_64` only.

```asm
.data = $self
> x86
  | jmp $addr
> x86_64  
  | push rax
  | mov  rax, $addr
  | jmp  rax
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

<br>

[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/L3L6W74V)
