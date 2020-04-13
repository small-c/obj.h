#include "app.h"

#define FORM_CLASS  "xForm"

// void Form::show()
method(Form, void, show)()
{
    obj_prepare(Form);
    ShowWindow(self->hwnd, SW_SHOW);
}

// void Form::hide()
method(Form, void, hide)()
{
    obj_prepare(Form);
    ShowWindow(self->hwnd, SW_HIDE);
}

// void Form::close()
method(Form, void, close)()
{
    obj_prepare(Form);
    CloseWindow(self->hwnd);
}

// void Form::setTitle(const char *)
method(Form, void, setTitle)(const char *title)
{
    obj_prepare(Form);
    SetWindowTextA(self->hwnd, title);
}

// We just inject self inside proc, no need to use userdata
// LRESULT CALLBACK Form::proc(HWND, UINT, WPARAM, LPARAM)
method(Form, LRESULT CALLBACK, proc)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    obj_prepare(Form);

    switch (msg) {
        // Finally, non-client being destroyed,
        // then we release all memory and quit message loop
        case WM_NCDESTROY: {
            self->base.release();
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

// Form::Form(...)
ctor(Form)(const char *title, int width, int height)
{
    obj_setup(Form);

    obj_bind(Form, show);
    obj_bind(Form, hide);
    obj_bind(Form, close);
    obj_bind(Form, setTitle);
    obj_bind(Form, proc);

    // Register class
    WNDCLASSEXA wcex    = { sizeof(WNDCLASSEXA) };
    wcex.lpfnWndProc    = self->proc;
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpszClassName  = FORM_CLASS;
    wcex.hCursor        = LoadCursorA(NULL, MAKEINTRESOURCEA(IDC_ARROW));
    RegisterClassExA(&wcex);

    // Compute middle screen
    RECT rc;
    GetClientRect(GetDesktopWindow(), &rc);
    int x = (rc.right  - width)  / 2;
    int y = (rc.bottom - height) / 2;

    // Create window
    self->hwnd = CreateWindowExA(
        0, FORM_CLASS, title, 
        WS_MINIMIZEBOX | WS_CAPTION | WS_POPUP | WS_SYSMENU,
        x, y, width, height,
        NULL, NULL, NULL, NULL
    );

    obj_done(Form);
}

// Form constructor wrapper
Form createForm(const char *title, int width, int height)
{
    return new(Form)(title, width, height);
}

// void Button::setOnClick(...)
method(Button, void, setOnClick)(void (* onClick)(Button self))
{
    obj_prepare(Button);
    self->onClick = onClick;
}

// void Button::setText(...)
method(Button, void, setText)(const char *text)
{
    obj_prepare(Form);
    SetWindowTextA(self->hwnd, text);
}

// Same to window proc, but here's custom
// LRESULT CALLBACK Button::proc(...)
method(Button, LRESULT CALLBACK, proc)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    obj_prepare(Button);

    switch (msg) {
        // Release memory
        case WM_NCDESTROY: {
            self->base.release();
            return 0;
        }
        // Mouse up event
        case WM_LBUTTONUP: {
            if (self->onClick != NULL) {
                self->onClick((Button)self);
            }
            break;
        }
    }

    return CallWindowProcA(self->oldProc, hwnd, msg, wp, lp);
}

// Button::Button(...)
ctor(Button)(Form parent, const char *text, int x, int y)
{
    obj_setup(Button);
    obj_bind(Button, setText);
    obj_bind(Button, setOnClick);
    obj_bind(Button, proc);

    // Create system button
    HWND hwnd = CreateWindowExA(
        0, "Button", text, WS_CHILD | WS_VISIBLE,
        x, y, 100, 35, 
        parent->hwnd, NULL, NULL, NULL
    );

    self->hwnd    = hwnd;
    self->form    = parent;
    self->onClick = NULL;
    // Custom wndproc
    self->oldProc = (WNDPROC)SetWindowLongPtrA(hwnd,
        GWLP_WNDPROC, (LONG_PTR)self->proc);

    obj_done(Button);
}

// Button constructor wrapper
Button createButton(Form parent, const char *name, int x, int y)
{
    return new(Button)(parent, name, x, y);
}

// Main message loop
void runApplication()
{
    MSG msg = { 0 };
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}
