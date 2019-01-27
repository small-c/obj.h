#include "app.h"

void gui__on_click(GUI self, int x, int y)
{
    PUTS("click: %d - %d\n", x, y);
}

void gui__on_close(GUI self)
{
    PUTS("close gui!\n");
}

void btn__on_click(Button self)
{
    MessageBoxA(
        self->gui->hwnd,
        "Hello, I'm MessageBox",
        "Test",
        MB_ICONINFORMATION
    );
}

int main()
{
    GUI gui = newGUI("Hello!", CW_USEDEFAULT, CW_USEDEFAULT, 500, 350);

    gui->on_click = gui__on_click;
    gui->on_close = gui__on_close;

    Button btn = newButton(gui, "My Button", 30, 30, btn__on_click);

    gui->set_title("Hello, world!");
    gui->show(true);

    runMessageLoop();

    freeButton(btn);
    freeGUI(gui);
    return 0;
}
