#include "shape.h"

// rectangle
#define RECTANGLE_AREA(width, height) (width * height)

wobj_def(_Rectangle, void, print_area, (void),
{
    printf("area of rectangle is %d\n", RECTANGLE_AREA(self->width, self->height));
})

wobj_init(_Rectangle, (int width, int height),
{
    wobj_setp(_Rectangle, _Shape, print_area);

    self->width = width;
    self->height = height;

},{})

// circle
#define PI 3.1415926
#define CIRCLE_AREA(radius) (radius * radius * PI)

wobj_def(_Circle, void, print_area, (void),
{
    printf("area of circle is %g\n", CIRCLE_AREA(self->radius));
})

wobj_init(_Circle, (int radius),
{
    wobj_setp(_Circle, _Shape, print_area);

    self->radius = radius;

},{})

int main()
{
    wobj_new(_Rectangle, rectangle, 15, 25);
    wobj_new(_Circle, circle, 30);

    rectangle->_Shape.print_area();
    circle->_Shape.print_area();

    wobj_free(_Rectangle, rectangle);
    wobj_free(_Circle, circle);

    puts("done!\n");
    return 0;
}
