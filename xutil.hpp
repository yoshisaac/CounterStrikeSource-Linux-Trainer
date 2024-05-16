/*
 *                                                                      WARNING
 * ChatGPT basically wrote this entire file. I have no interest in working with a window protocol from 1984, although more recently X11 is from 2012.
 * Regardless, it's not like im using ChatGPT for everything. Just a little less pain with legacy software. This really isn't fun.
 */

#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdbe.h>
#include <unistd.h>
#include <climits>
#include <cstring>
#include <string>

//#include "engine/engine.hpp"

#define BASIC_EVENT_MASK (StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask)
#define NOT_PROPAGATE_MASK (KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ButtonMotionMask)

// Create a XColor from 3 byte tuple (0 - 255, 0 - 255, 0 - 255).
inline XColor createXColorFromRGBA(short red, short green, short blue, short alpha, Display* d, int screen) {
  XColor color;

  // m_color.red = red * 65535 / 255;
  color.red = (red * 0xFFFF) / 0xFF;
  color.green = (green * 0xFFFF) / 0xFF;
  color.blue = (blue * 0xFFFF) / 0xFF;
  color.flags = DoRed | DoGreen | DoBlue;

  if (!XAllocColor(d, DefaultColormap(d, screen), &color)) {
    printf("createXColorFromRGB: Cannot create color\n");
  }

  *(&color.pixel) = ((*(&color.pixel)) & 0x00ffffff) | (alpha << 24);
  return color;
}

// Create a XColor from 3 byte tuple (0 - 255, 0 - 255, 0 - 255).
inline XColor createXColorFromRGB(short red, short green, short blue, Display* d, int screen) {
  return createXColorFromRGBA(red, green, blue, 255, d, screen);
}



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

//double buffering examples: https://github.com/tsoding/x11-double-buffering/blob/master/db_xdbe.c
inline void db_clear(XdbeBackBuffer back_buffer, Display* d, Window win, GC gc)
{
  XSetForeground(d, gc, 0);
  XFillRectangle(d, back_buffer, gc, 0, 0, 1920, 1080);
}

inline void db_rect(XdbeBackBuffer back_buffer, Display* d, GC gc, int x, int y, unsigned int w, unsigned int h)
{
  XDrawRectangle(d, back_buffer, gc, x, y, w, h);
}

inline void db_fillrect(XdbeBackBuffer back_buffer, Display* d, GC gc, int x, int y, unsigned int w, unsigned int h)
{
  XFillRectangle(d, back_buffer, gc, x, y, w, h);
}

inline void db_line(XdbeBackBuffer back_buffer, Display* d, GC gc, int x1, int y1, int x2, int y2)
{
  XDrawLine(d, back_buffer, gc, x1, y1, x2, y2);
}

//chatgipidy assisted
inline void db_thickline(XdbeBackBuffer back_buffer, Display* d, GC gc, int x1, int y1, int x2, int y2, int thickness, int distance, bool sides=false) {
  for (int i = 0; i < thickness; i++) {
    if (y1 == y2) {
      XDrawLine(d, back_buffer, gc, x1, y1+(i*((distance*0.66f)/distance)), x2, y2+(i*((distance*0.66f)/distance)));
      XDrawLine(d, back_buffer, gc, x1, y1-(i*((distance*0.66f)/distance)), x2, y2-(i*((distance*0.66f)/distance)));
    } else {
      XDrawLine(d, back_buffer, gc, x1+(i*((distance*0.66f)/distance)), y1, x2+(i*((distance*0.66f)/distance)), y2);
      XDrawLine(d, back_buffer, gc, x1-(i*((distance*0.66f)/distance)), y1, x2-(i*((distance*0.66f)/distance)), y2);
    }
  }
}
  
inline void db_swap_buffers(Display* d, Window win)
{
  XdbeSwapInfo swap_info;
  swap_info.swap_window = win;
  swap_info.swap_action = 0;
  XdbeSwapBuffers(d, &swap_info, 1);
}
