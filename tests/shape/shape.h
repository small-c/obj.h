#include "wobj.h"

// tested on Windows, but many names same to function name

wobj(_Shape,
    public (
        void func(print_area, (void));
    )
);

wobj(_Rectangle,
    public (
        wobj_public(_Shape);

        int width;
        int height;
    )
);

wobj(_Circle,
    public (
        wobj_public(_Shape);

        int radius;
    )
);
