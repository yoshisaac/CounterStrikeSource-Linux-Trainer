#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <climits>

//chatgpt wrote this
bool isKeyDown(Display *display, int key) {
    char keys[32];
    XQueryKeymap(display, keys);

    KeyCode keyCode = XKeysymToKeycode(display, key);
    return (keys[keyCode / 8] & (1 << (keyCode % 8))) != 0;
}

//chatgpt wrote this
Window getFocusedWindow(Display *display) {
    Window focusedWindow;
    int revert;
    XGetInputFocus(display, &focusedWindow, &revert);
    return focusedWindow;
}

//chatgpt wrote this
pid_t getWindowPID(Display *display, Window window) {
    Atom prop = XInternAtom(display, "_NET_WM_PID", False);

    Atom actualType;
    int format;
    unsigned long nItems, bytesAfter;
    unsigned char *propData = NULL;

    if (XGetWindowProperty(display, window, prop, 0, LONG_MAX / 4, False, AnyPropertyType,
                           &actualType, &format, &nItems, &bytesAfter, &propData) == Success) {
        if (propData != NULL) {
            pid_t *pidArray = reinterpret_cast<pid_t *>(propData);
            pid_t pid = pidArray[0];
            XFree(propData);
            return pid;
        }
    }

    return -1; // Unable to retrieve PID
}
