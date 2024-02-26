#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <climits>
#include <cstring>
#include <string>

//chatgpt wrote this
inline void getWindowPosition(Display* d, Window win, int& x, int& y) {
  XWindowAttributes attr;
  if (XGetWindowAttributes(d, win, &attr)) {
    x = attr.x;
    y = attr.y;
  }
}

//chatgpt wrote this
inline bool isKeyDown(Display *display, int key) {
    char keys[32];
    XQueryKeymap(display, keys);

    KeyCode keyCode = XKeysymToKeycode(display, key);
    return (keys[keyCode / 8] & (1 << (keyCode % 8))) != 0;
}

//Get window from pid and processes name
//chatgpt wrote this
inline Window getWindowByPid(Display* display, pid_t pid) {
    Window root = XDefaultRootWindow(display);
    Window parent, *children;
    unsigned int nchildren;

    if (XQueryTree(display, root, &root, &parent, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; ++i) {
            char* window_pid;
            int status = XFetchName(display, children[i], &window_pid);
            if (status) {
                if (window_pid != nullptr) {
                    std::string window_pid_str(window_pid);
                    if (window_pid_str == std::to_string(pid)) {
                        XFree(window_pid);
                        XFree(children);
                        return children[i];
                    }
                }
                XFree(window_pid);
            }
        }
        XFree(children);
    }
    return 0;
}

//chatgpt wrote this
inline Window getFocusedWindow(Display *display) {
    Window focusedWindow;
    int revert;
    XGetInputFocus(display, &focusedWindow, &revert);
    return focusedWindow;
}

//Get pid from window
//chatgpt wrote this
inline pid_t getPidByWindow(Display *display, Window window) {
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
