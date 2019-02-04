#include "wobj.h"

// tested on Windows, but many names same to function name

wobj(_Shape,
    public (
        wobj_fn (void, print_area, (void));
    )
);

wobj_np(_Rectangle,
    public (
        wobj_public(_Shape);

        int width;
        int height;
    )
);

wobj_np(_Circle,
    public (
        wobj_public(_Shape);

        int radius;
    )
);
