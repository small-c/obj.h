#include <windows.h>
#include <windowsx.h>
#include "obj.h"

classdef(Form);
classdef(Label);
classdef(Button);

class(Form,
    public (
        HWND    hwnd;
        void (* show)();
        void (* hide)();
        void (* close)();
        void (* setTitle)(const char *title);
    ),
    private (
        WNDPROC proc;
    )
);

class(Button,
    public (
        HWND hwnd;
        Form form;
        void (* setText)(const char *text);
        void (* setOnClick)(void (* onClick)(Button self));
    ),
    private (
        WNDPROC oldProc;
        WNDPROC proc;
        void (* onClick)(Button self);
    )
);

Form   createForm(const char *title, int width, int height);
Button createButton(Form parent, const char *text, int x, int y);
void   runApplication();
