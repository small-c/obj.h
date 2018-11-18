#include "wobj.h"                                 
                                                     
// declare Dog object                                
wobj(Dog, {                                          
    uint32_t weight; // property                     
    char *name;                                
    void(*speak)(); // method                        
                                                     
} /*, you can define type here as struct*/ );         
                                                     
// define 'speak' method, like: void Dog.speak(void) 
wobj_def(Dog, void, speak, (void), {                 
    printf("I'm %s, my weight is %dkg.\n",           
        self->name, self->weight);                   
})                                                   
                                                     
// init Dog                                          
wobj_init(Dog, (const char* name, uint32_t weight), {
	// set method
    wobj_set(Dog, speak); // set 'sepak'

    // set property  
	self->weight = weight;

	int len = strlen(name) + 1;
	self->name = wobj_alloc(Dog, len); // mustn't free() when alloc with wobj_alloc()
	memset(self->name, '\0', len);
	memcpy(self->name, name, len); // copy                      
}, {                                  
	// on free, called by wobj_free()                                         
})                                                   
                                                     
int main(void) {                                                     
    // create mydog as Dog object                    
    wobj_new(Dog, dog_foo, "Crazy Dog!", 32);
    // call method                                   
    dog_foo->speak();                                
    // << I'm Crazy Dog!, my weight is 32kg.          
                                                     
    wobj_free(Dog, dog_foo); // free dog_foo         
    return 0;                                        
}
