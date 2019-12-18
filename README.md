<p align="center">
    <h1 align="center">obj.h</h1>
    <h4 align="center">
        A simple one header supports oriented object programming in C.
    </h4>
    <p align="center">
        <a href="https://travis-ci.org/wy3/wobj" target="_blank"><img src="https://travis-ci.org/wy3/wobj.svg?branch=master"></a>
        <a href="#"><img src="https://img.shields.io/badge/os-cross--platform-blue.svg"></a>
    </p>
</p>

```c
foo f = new(foo)(10);
assert(f->get() == 10);
f->base.release();
```

### features
- Simple syntax (inspired by C++ and Java)
- Objects, classes
- Public, private members
- Data abstraction
- Inheritance

### platforms supported

_ | GCC | MSVC | Clang
-|:-:|:-:|:-:
Windows | ✓ | ✓ | ✓
Linux   | x86 / x86_64 |  | x86
Mac OSX | ✓ |  | x86

### how it works?

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

### example
### documentations

updating...
