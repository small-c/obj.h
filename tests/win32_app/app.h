#ifndef _wobj_test_app_
#define _wobj_test_app_
#pragma once
#include <windows.h>
#include <windowsx.h>
#include "wobj.h"

wobj(GUI, {

    HWND	hwnd;
    // event handler, user custom
    wobj_fn(void, on_click, (GUI self, int x, int y));
    wobj_fn(void, on_close, (GUI self));

    // method
    wobj_fn(void, show, (bool is_show));
    wobj_fn(void, close, (void));
    wobj_fn(void, set_title, (const char *title));
}, {
    // window proc
    WNDPROC proc;
})

wobj(Button, {

    HWND hwnd;
    GUI gui; // gui parent

}, {

    wobj_fn(void, on_click, (Button self)); // users'
    WNDPROC proc_old;

    WNDPROC proc;

})

// ==============================================

GUI newGUI(const char *title, int x, int y, int width, int height);
GUI freeGUI(GUI gui);
Button newButton(GUI gui, const char *name, int x, int y, void(*on_click)(Button));
void freeButton(Button btn);
int PUTS(const char *fmt, ...);
void runMessageLoop();

#endif // !_wobj_test_app_
