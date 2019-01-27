#include "app.h"

// GUI ==========================================

#define WOBJ_GUICLASS ("wobj.GUI")

wobj_def(GUI, void, show, (bool is_show),
{
    ShowWindow(self->hwnd, (is_show ? SW_SHOW : SW_HIDE));
})

wobj_def(GUI, void, close, (void), {
    CloseWindow(self->hwnd);
})

wobj_def(GUI, void, set_title, (const char *title),
{
    PUTS("change title to `%s`\n", title);
    SetWindowTextA(self->hwnd, title);
})

wobj_def(GUI, LRESULT, proc, (HWND hwnd, UINT msg, WPARAM wp, LPARAM lp),
{
    switch (msg) {
        case WM_LBUTTONUP: {
            if (self->on_click) {
                int x = GET_X_LPARAM(lp);
                int y = GET_Y_LPARAM(lp);
                self->on_click((GUI)self, x, y);
            }
            break;
        }

        case WM_CLOSE: {
            if (self->on_close)
                self->on_close((GUI)self);

            PostQuitMessage(0);
            break;
        }
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
})

wobj_init(GUI, (const char *title, int x, int y, int width, int height),
{
    wobj_set(GUI, show);
    wobj_set(GUI, close);
    wobj_set(GUI, set_title);
    wobj_set(GUI, proc);

    WNDCLASSEXA wcex    = { sizeof(WNDCLASSEXA) };
    wcex.lpfnWndProc	= self->proc;
    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpszClassName  = WOBJ_GUICLASS;
    wcex.hCursor        = LoadCursorA(NULL, MAKEINTRESOURCEA(IDC_ARROW));
    RegisterClassExA(&wcex);

    self->on_click = NULL;
    self->on_close = NULL;

    self->hwnd = CreateWindowExA(
        0, WOBJ_GUICLASS, title, 
        WS_MINIMIZEBOX | WS_CAPTION | WS_POPUP | WS_SYSMENU,
        x, y, width, height,
        NULL, NULL, NULL, NULL
    );
}, {

})

GUI newGUI(const char *title, int x, int y, int width, int height)
{
    return wobj_new4(GUI)(title, x, y, width, height);
}

GUI freeGUI(GUI gui)
{
    wobj_free(GUI, gui);
}

// Button =======================================

wobj_def(Button, LRESULT, proc, (HWND hwnd, UINT msg, WPARAM wp, LPARAM lp),
{
    switch (msg) {
        case WM_LBUTTONUP: {
            if (self->on_click) {
                int x = GET_X_LPARAM(lp);
                int y = GET_Y_LPARAM(lp);
                self->on_click((Button)self);
            }
            break;
        }
    }

    return CallWindowProcA(self->proc_old, hwnd, msg, wp, lp);
})

wobj_init(Button, (GUI gui, const char *text, int x, int y, void(*on_click)(Button)),
{
    wobj_set(Button, proc);

    HWND hwnd = CreateWindowExA(
        0, "Button", text, WS_CHILD | WS_VISIBLE,
        x, y, 100, 35, 
        gui->hwnd, NULL, NULL, NULL
    );

    self->hwnd	   = hwnd;
    self->gui	   = gui;
    self->on_click = on_click;
    self->proc_old = (WNDPROC)SetWindowLongPtrA(hwnd, GWLP_WNDPROC, (LONG_PTR)self->proc);

}, {

})

Button newButton(GUI gui, const char *name, int x, int y, void (*on_click)(Button))
{
    return wobj_new4(Button)(gui, name, x, y, on_click);
}

void freeButton(Button btn)
{
    wobj_free(Button, btn);
}

// ==============================================

int PUTS(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(stdout, fmt, ap);
    va_end(ap);
    fflush(stdout);
    return ret;
}

void runMessageLoop()

{
    MSG msg = { 0 };
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}
