# wobj
C oriented object based on closure function.

### usage
Add `#include "wobj.h"` to your source code.

### example
Comparison between **C** and **C++**, see [**test.c**](https://github.com/wy3/wobj/blob/master/test.c) for code guide.

```c++
// C                                                    |  // C++
#include "wobj.h"                                       |  #include <stdio.h>
                                                        |  #include <stdlib.h>
                                                        |  #include <stdint.h>
                                                        |  
wobj(Dog, {                                             |  class Dog {
    uint32_t weight;                                    |  public:
    const char *name;                                   |      Dog(const char *name, uint32_t weight);
    void(*speak)();                                     |      ~Dog();
                                                        |      uint32_t weight;
});                                                     |      const char *name;
                                                        |      void speak();
                                                        |  };
                                                        |  
wobj_def(Dog, void, speak, (void), {                    |  void Dog::speak() {
    printf("I'm %s, my weight is %dkg.\n",              |      printf("I'm %s, my weight is %dkg.\n",
        self->name, self->weight);                      |          this->name, this->weight);
})                                                      |  }
                                                        |  
wobj_init(Dog, (const char* name, uint32_t weight), {   |  Dog::Dog(const char *name, uint32_t weight) {
    wobj_set(Dog, speak);                               |      this->name = name;
    self->name = name;                                  |      this->weight = weight;
    self->weight = weight;                              |  }
}, {                                                    |  
    free(self->speak);                                  |  Dog::~Dog() {
    free(self);                                         |  
})                                                      |  }
                                                        |  
int main(void) {                                        |  int main(void) {
    const char* name = "Crazy Dog!";                    |      const char *name = "Crazy Dog!";
    wobj_new(Dog, dog_foo, name, 32);                   |      Dog *dog_foo = new Dog(name, 32);
    dog_foo->speak();                                   |      dog_foo->speak();
                                                        |      
    wobj_free(Dog, dog_foo);                            |      detele dog_foo;
    return 0;                                           |      return 0;
}                                                       |  }
```
