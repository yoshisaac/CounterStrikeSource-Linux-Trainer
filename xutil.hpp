/*
 *                                                                      WARNING
 * ChatGPT basically wrote this entire file. I have no interest in working with a window protocol from 1984, although more recently X11 is from 2012.
 * Regardless, it's not like im using ChatGPT for everything. Just a little less pain with legacy software. This really isn't fun.
 */


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
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
inline bool isKeyDown(Display* d, int key) {
    char keys[32];
    XQueryKeymap(d, keys);

    KeyCode keyCode = XKeysymToKeycode(d, key);
    return (keys[keyCode / 8] & (1 << (keyCode % 8))) != 0;
}

//chatgpt assisted
inline bool isMouseDown(Display* d, int mouse) {
  unsigned int button_state;
  Window root = DefaultRootWindow(d);
  XEvent event;
  XQueryPointer(d, root, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
  if (event.xbutton.state & mouse) {
    return true;
  }
  else {
    return false;
  }
}

//Halts the program until that pid is in focus, yes, the only purpose is to get the Window object of the game
//chatgpt assisted
inline Window waitUntilPidIsFocus(Display *display, pid_t pid) {
    Atom prop = XInternAtom(display, "_NET_WM_PID", False);

    Atom actualType;
    int format;
    unsigned long nItems, bytesAfter;
    unsigned char *propData = NULL;

    while (true) {
      Window focusedWindow;
      int revert;
      XGetInputFocus(display, &focusedWindow, &revert);
      if (XGetWindowProperty(display, focusedWindow, prop, 0, LONG_MAX / 4, False, AnyPropertyType,
			     &actualType, &format, &nItems, &bytesAfter, &propData) == Success) {
        if (propData != NULL) {
	  pid_t *pidArray = reinterpret_cast<pid_t *>(propData);
	  pid_t windowPid = pidArray[0];

	  if (windowPid == pid) {
	    XFree(propData);
	    return focusedWindow;
	  }
        }
      }
    }

    return -1; // Unable to retrieve PID
}

//chatgpt wrote this
inline Window getFocusedWindow(Display* d) {
    Window focusedWindow;
    int revert;
    XGetInputFocus(d, &focusedWindow, &revert);
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
