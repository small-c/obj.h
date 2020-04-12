#include "shape.h"

int main()
{
#if 0
	// Abstraction
    Shape rc = (Shape)new(Rect)(15, 25);
    Shape ci = (Shape)new(Circle)(30);

    rc->area();
    ci->area();
#else
	Rect rc = new(Rect)(15, 25);
    Circle ci = new(Circle)(30);

    rc->Shape.area();
    ci->Shape.area();
#endif

    rc->base.release();
    ci->base.release();

    return 0;
}
