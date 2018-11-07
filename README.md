# wobj
C oriented object based on closure function.

### usage
Add `#include "wobj.h"` to your source code.

### example

```c++
// C                                                    | // C++
#include "wobj.h"                                       |  #include <stdio.h>
                                                        |  #include <stdlib.h>
/*declare Dog object*/                                  |  #include <stdint.h>
wobj(Dog, {                                             |  
    uint32_t weight; /*property*/                       |  class Dog {
    const char *name;                                   |  public:
    void(*speak)(); /*method*/                          |      Dog(const char *name, uint32_t weight);
                                                        |      ~Dog() {};
} /*, you can define type here as struct*/);            |  
                                                        |      uint32_t weight;
/*define 'speak' method, like: void Dog.speak(void)*/   |      const char *name;
wobj_def(Dog, void, speak, (void), {                    |      void speak();
    printf("I'm %s, my weight is %dkg.\n",              |  };
        self->name, self->weight);                      |  
})                                                      |  Dog::Dog(const char *name, uint32_t weight) {
                                                        |      this->name = name;
/*init Dog*/                                            |      this->weight = weight;
wobj_init(Dog, (const char* name, uint32_t weight), {   |  }
    wobj_set(Dog, speak); /*set 'sepak', important!*/   |  
    /*set property*/                                    |  void Dog::speak() {
    self->name = name;                                  |      printf("I'm %s, my weight is %dkg.\n",
    self->weight = weight;                              |          this->name, this->weight);
}, { /*free*/                                           |  }
    free(self->speak); /*necessary*/                    |  
    free(self);                                         |  int main(void) {
})                                                      |      const char *name = "Crazy Dog!";
                                                        |      Dog *dog_foo = new Dog(name, 32);
int main(void) {                                        |      
    const char* name = "Crazy Dog!";                    |      dog_foo->speak();
    /*create mydog as Dog object*/                      |      return 0;
    wobj_new(Dog, dog_foo, name, 32);                   |  }
    /*call method*/                                     |  
    dog_foo->speak();                                   |  
    /*<< I'm Crazy Dog, my weight is 32kg.*/            |  
                                                        |  
    wobj_free(Dog, dog_foo); /*free dog_foo*/           |  
    return 0;                                           |  
}                                                       |  
```
