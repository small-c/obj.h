#include "wobj.h"

// tested on Windows, but many names same to function name

wobj_np(_Shape,
{
    wobj_fn (void, print_area, (void));

});

wobj_np(_Rectangle,
{
    wobj_public(_Shape);
    
    int width;
    int height;

});

wobj_np(_Circle,
{
    wobj_public(_Shape);

    int radius;

});
