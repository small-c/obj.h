<p align="center">
    <h1 align="center">obj.h</h1>
    <h4 align="center">
        A single-header supports oriented object programming in pure C
       <br>
        - <i>just a hacking on assembly</i> -
    </h4>
    <p align="center">
        <a href="https://travis-ci.org/wy3/wobj" target="_blank"><img src="https://travis-ci.org/wy3/wobj.svg?branch=master"></a>
        <a href="#"><img src="https://img.shields.io/badge/cc-multiple-blue.svg"></a>
    </p>
</p>

```cpp
Foo f = new(Foo)(10);
assert(f->get() == 10);
f->base.release();
```

### Features
- Simple syntax (inspired by **C++** and **Java**)
- Objects, classes
- Public, private members
- Data abstraction
- Inheritance

### Usage

- Just add `obj.h` to your **C** source.
- See tests for more.

### Compilers supported

OS\CC   | GCC | MSVC | Clang
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
