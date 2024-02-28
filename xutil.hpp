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

//Get window from pid and processes name
//chatgpt wrote this
inline Window getWindowByPid(Display* display, int pid) {
    Window root = DefaultRootWindow(display);
    Window parent, *children;
    unsigned int nchildren;

    XQueryTree(display, root, &root, &parent, &children, &nchildren);
    for (unsigned int i = 0; i < nchildren; ++i) {
        XWindowAttributes attrs;
        XGetWindowAttributes(display, children[i], &attrs);


	pid_t child_pid;
	XWMHints *wm_hints = XGetWMHints(display, children[i]);
	if (wm_hints && (child_pid = wm_hints->initial_state) != WithdrawnState) {
	  XFree(wm_hints);
	  char *window_name = nullptr;
	  XFetchName(display, children[i], &window_name);
	  if (window_name != nullptr) {
	    if (child_pid == pid) {
	      XFree(children);
	      XFree(window_name);
	      return children[i];
	    }
	    XFree(window_name);
	  }
	}
    
    }
    XFree(children);
    return 0;
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
