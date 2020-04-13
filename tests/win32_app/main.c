#include "app.h"

void btn_onClick(Button self)
{
    MessageBoxA(
        self->form->hwnd,
        "Hello, I'm a message box.",
        "Test",
        MB_ICONINFORMATION
    );

    self->setText("Text changed");
}

int main()
{
    Form form = createForm("Hello!", 500, 350);
    Button btn = createButton(form, "Click me", 30, 30);

    btn->setOnClick(btn_onClick);
    form->setTitle("Hello, world!");
    form->show();

    runApplication();
    return 0;
}
