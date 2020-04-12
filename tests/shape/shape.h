#include "obj.h"

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

classdef(Shape);
classdef(Rect);
classdef(Circle);

class(Shape,
    public (
        void (* area)();
    )
);

class(Rect,
    public (
        extend(Shape);
        int w;
        int h;
    )
);

class(Circle,
    public (
        extend(Shape);
        int r;
    )
);

// void Rect::area()
method(Rect, void, area)()
{
	obj_prepare(Rect);

	double S = self->w * self->h;
	printf("Area of rectangle = %g\n", S);
}

// Rect::Rect(int, int)
ctor(Rect)(int width, int height)
{
	obj_setup(Rect);
	obj_override(Rect, Shape, area);

	self->w = width;
	self->h = height;

	obj_done(Rect);
}

// void Circle::area()
method(Circle, void, area)()
{
	obj_prepare(Circle);

	double S = self->r * self->r * M_PI;
	printf("Area of circle = %g\n", S);
}

// Circle::Circle(int)
ctor(Circle)(int radius)
{
	obj_setup(Circle);
	obj_override(Circle, Shape, area);

	self->r = radius;

	obj_done(Circle);
}
