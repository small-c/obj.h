#ifndef _WOBJ_TEST_APP_
#define _WOBJ_TEST_APP_
#pragma once

#include <windows.h>
#include <windowsx.h>
#include "wobj.h"

// declare all
wobj_decl(GUI);
wobj_decl(Button);

// GUI
wobj(GUI,
    public
    (
        HWND    hwnd;
        // event handler, user custom
        void func(on_click, (GUI self, int x, int y));
        void func(on_close, (GUI self));

        // method
        void func(show, (bool is_show));
        void func(close, (void));
        void func(set_title, (const char *title));
    ),
    private
    (
        // window proc
        WNDPROC proc;
    )
)

// Button
wobj(Button,
    public
    (
        HWND hwnd;
        GUI gui; // gui parent
    ),
    private 
    (
        void func(on_click, (Button self)); // users'
        WNDPROC proc_old;

        WNDPROC proc;
    )
)

// ==============================================

GUI newGUI(const char *title, int x, int y, int width, int height);
GUI freeGUI(GUI gui);
Button newButton(GUI gui, const char *name, int x, int y, void(*on_click)(Button));
void freeButton(Button btn);
int PUTS(const char *fmt, ...);
void runMessageLoop();

#endif // !_WOBJ_TEST_APP_
